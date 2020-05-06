#include "file.hpp"

#include <chrono>
#include <fstream>
#include <functional>

namespace gold {
	namespace fs = std::filesystem;

	obj& file::getPrototype() {
		static auto proto = obj({
			{"save", method(&file::save)},
			{"load", method(&file::load)},
			{"trash", method(&file::trash)},
			{"getWriteTime", method(&file::getWriteTime)},
			{"hash", method(&file::hash)},
		});
		return proto;
	}

	file::file() : obj() { }

	file::file(path p) : obj() {
		setParent(getPrototype());
		setString("path", p.string());
	}

	file::file(binary data) : obj() {
		setParent(getPrototype());
		setBinary("data", data);
	}

	var file::save(list args) {
		string path = getString("path");
		binary data = getBinary("data");
		for (auto it = args.begin(); it != args.end(); ++it)
			if (it->isString())
				path = it->getString();
			else if (it->isBinary())
				data = it->getBinary();
		if (path.size() == 0)
			return genericError(
				"path is empty, supply as argument or set path on "
				"object");
		if (data.size() == 0)
			return genericError(
				"data is empty, supply as argument or set data on "
				"object");
		auto str = ofstream(path);
		if (str.is_open()) {
			str.write((char*)data.data(), data.size());
			str.close();
			auto writeTime = fs::last_write_time(path);
			auto wtms =
				(uint64_t)
					std::chrono::duration_cast<std::chrono::nanoseconds>(
						writeTime.time_since_epoch())
						.count();
			setUInt64("writeTime", wtms);
			return true;
		}
		return false;
	}

	var file::load(list args) {
		try {
			auto path = args.size() > 0 ? args[0].getString()
																	: getString("path");
			if (path.size() == 0)
				return genericError(
					"path is empty, supply as argument or set path on "
					"object");
			if (fs::exists(path)) {
				auto data = binary();
				getBinary("data", data);
				auto writeTime = fs::last_write_time(path);
				auto wtms = (uint64_t)std::chrono::duration_cast<
											std::chrono::nanoseconds>(
											writeTime.time_since_epoch())
											.count();
				auto lastWriteTime = getUInt64("writeTime");
				if (data.size() > 0 && wtms == lastWriteTime)
					return data;
				auto str = ifstream(path);
				if (str.is_open()) {
					str.seekg(0, str.end);
					auto size = size_t(str.tellg());
					auto bin = binary(size);
					str.seekg(0, str.beg);
					str.read((char*)bin.data(), bin.size());
					str.close();
					setUInt64("writeTime", wtms);
					setBinary("data", bin);
					return var(bin);
				}
			}
			return gold::var();
		} catch (exception e) {
			return genericError(e);
		}
	}

	var file::trash(list args) {
		auto path =
			args.size() > 0 ? args[0].getString() : getString("path");
		if (path.size() == 0)
			return genericError(
				"path is empty, supply as argument or set path on "
				"object");
		setNull("writeTime");
		return remove(fs::path(path));
	}

	var file::getWriteTime(list args) {
		auto path =
			args.size() > 0 ? args[0].getString() : getString("path");
		if (path.size() == 0)
			return genericError(
				"path is empty, supply as argument or set path on "
				"object");
		auto cur = getUInt64("writeTime");
		if (cur != 0) return cur;
		auto writeTime = fs::last_write_time(path);
		auto wtms =
			(uint64_t)
				std::chrono::duration_cast<std::chrono::nanoseconds>(
					writeTime.time_since_epoch())
					.count();
		return wtms;
	}

	var file::hash(list args) {
		auto data = binary();
		if (args.size() > 0)
			args[0].returnBinary(data);
		else
			returnBinary("data", data);
		auto strData = string_view((char*)data.data(), data.size());
		auto h = std::hash<string_view>();
		return to_string((uint64_t)h(strData));
	}

	file::operator binary() {
		auto data = load();
		return data.getBinary();
	}

	file::operator string() {
		auto data = load();
		return data.getString();
	}

	file& file::readFile(path p, file& results) {
		results = file(p);
		results.load();
		return results;
	}

	obj& file::recursiveReadDirectory(path p, obj& results) {
		try {
			auto point = fs::canonical(p);
			auto cwd = fs::current_path();
			auto pName = point.string();
			pName = pName.substr(pName.rfind('/') + 1);
			auto absPoint = fs::canonical(point);
			if (fs::exists(absPoint) && fs::is_directory(absPoint)) {
				for (auto& p : fs::recursive_directory_iterator(
							 absPoint,
							 fs::directory_options::
								 follow_directory_symlink)) {
					auto chop = p.path().string().find(pName);
					auto relURL = fs::relative(p, cwd);
					auto url =
						fs::path(p.path().string().substr(chop - 1));
					if (!fs::is_directory(p)) {
						auto urlStr = url.string();
						if (urlStr.find("index.html") != string::npos)
							urlStr = urlStr.substr(
								0, urlStr.size() - strlen("index.html"));
						auto f = file(p);
						results.setObject(urlStr, f);
					}
				}
			} else {
				auto e =
					"Directory doesn't exist. (" + point.string() + ")";
				results.setString("error", e);
			}
			return results;
		} catch (exception e) {
			return results;
		}
	}
}  // namespace gold