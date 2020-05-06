
#include <stdarg.h>
#include <string.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>

#include "types.hpp"

namespace gold {
	using namespace std;
	using value_t = nlohmann::detail::value_t;
	struct objData {
		object::omap items;
		gold::object parent;
		uint64_t id;
		mutex omutex;
	};

	object::ptr object::newObjData(omap m, uint64_t id) {
		auto obj = object();
		auto p = new objData{m, obj, id, mutex()};
		p->id = id != 0 ? id : (uint64_t)p;
		return object::ptr(p);
	}

	void object::initMemory() {
		if (!data) data = newObjData();
	}

	void object::findParent() {
		auto parentIt = data->items.find("proto");
		if (parentIt != data->items.end()) {
			auto parent = parentIt->second.getObject();
			if (parent) {
				setParent(parent);
				data->items.erase(parentIt);
			}
		}
	}

	object::object(json value) : data(newObjData()) {
		if (value.is_object()) {
			for (auto it = value.begin(); it != value.end(); ++it) {
				auto name = it.key();
				auto value = it.value();
				var toSet = var();
				switch (value.type()) {
					case value_t::object: {
						toSet = object(value);
						break;
					}
					case value_t::array: {
						toSet = list(value);
						break;
					}
					case value_t::string:
						toSet = var((char*)((string)value).c_str());
						break;
					case value_t::boolean:
						toSet = var((bool)value);
						break;
					case value_t::number_integer:
						toSet = var((int64_t)value);
						break;
					case value_t::number_unsigned:
						toSet = var((uint64_t)value);
						break;
					case value_t::number_float:
						toSet = var((double)value);
						break;
					default:
					case value_t::null:
						break;
				}
				setVar(name, toSet);
			}
		}
	}

	object::object(var value) {
		auto obj = value.getObject();
		if (bool(obj.data))
			data = obj.data;
		else
			data = newObjData();
	}

	object::object(initList list)
		: data(newObjData(object::omap(list), 0)) {
		findParent();
	}

	object::object() : data(nullptr) {}

	object::~object() {
		if (data && data.use_count() == 1) {
			data->items.clear();
			data->parent = object();
		}
		data = nullptr;
	}

	object::omap::iterator object::begin() {
		unique_lock<mutex> gaurd(data->omutex);
		return std::begin(data->items);
	}

	object::omap::iterator object::end() {
		unique_lock<mutex> gaurd(data->omutex);
		return std::end(data->items);
	}

	uint64_t object::size() {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		return data->items.size();
	}

	types object::getType(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getType();
		if (data->parent.data) return data->parent.getType(name);
		return typeNull;
	}

	json object::getJSON() {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		json j = json::object();
		auto end = data->items.end();
		for (auto i = data->items.begin(); i != end; ++i) {
			auto name = i->first;
			auto value = i->second;
			switch (value.getType()) {
				case typeNull:
					j[name] = (nullptr);
					break;
				case typeList: {
					auto arr = value.getList();
					j[name] = (arr.getJSON());
					break;
				}
				case typeObject: {
					auto obj = value.getObject();
					j[name] = (obj.getJSON());
					break;
				}
				case typeString:
					j[name] = (value.getString());
					break;
				case typeInt64:
					j[name] = (value.getInt64());
					break;
				case typeInt32:
					j[name] = (value.getInt32());
					break;
				case typeInt16:
					j[name] = (value.getInt16());
					break;
				case typeInt8:
					j[name] = (value.getInt8());
					break;
				case typeUInt64:
					j[name] = (value.getUInt64());
					break;
				case typeUInt32:
					j[name] = (value.getUInt32());
					break;
				case typeUInt16:
					j[name] = (value.getUInt16());
					break;
				case typeUInt8:
					j[name] = (value.getUInt8());
					break;
				case typeDouble:
					j[name] = (value.getDouble());
					break;
				case typeFloat:
					j[name] = (value.getFloat());
					break;
				case typeBool:
					j[name] = (value.getBool());
					break;
				default:
					break;
			}
		}
		return j;
	}

	binary object::getBSON() { return json::to_bson(getJSON()); }

	binary object::getCBOR() { return json::to_cbor(getJSON()); }

	binary object::getMsgPack() {
		return json::to_msgpack(getJSON());
	}

	binary object::getUBJSON() {
		return json::to_ubjson(getJSON());
	}

	var object::callMethod(string name) {
		initMemory();
		auto method = this->getMethod(name);
		var resp;
		if (method != nullptr) {
			auto empty = list();
			resp = (this->*method)(empty);
		}
		return resp;
	}

	var object::callMethod(string name, list args) {
		initMemory();
		auto method = this->getMethod(name);
		var resp;
		if (method != nullptr) resp = (this->*method)(args);
		return resp;
	}

	void object::copy(object& other) {
		initMemory();
		if (other) {
			unique_lock<mutex> gaurd(other.data->omutex);
			auto end = other.data->items.end();
			for (auto it = other.data->items.begin(); it != end; ++it)
				data->items[it->first] = var(it->second);
			findParent();
		}
	}

	void object::empty() {
		auto end = data->items.end();
		for (auto it = data->items.begin(); it != end; ++it)
			data->items.erase(it);
	}

	void object::setParent(object other) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		if (other.data) {
			data->parent.data = other.data;
		} else
			data->parent = object();
	}

	object object::getParent() {
		initMemory();
		return data->parent;
	}

	void object::setString(string name, string value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt64(string name, int64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt32(string name, int32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt16(string name, int16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt8(string name, int8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt64(string name, uint64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt32(string name, uint32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt16(string name, uint16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt8(string name, uint8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setDouble(string name, double value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setFloat(string name, float value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setBool(string name, bool value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setList(string name, list value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setObject(string name, object value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value);
	}

	void object::setMethod(string name, method& value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value);
	}

	void object::setFunc(string name, func& value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value);
	}

	void object::setPtr(string name, void* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value, typePtr);
	}

	void object::setBinary(string name, binary value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setVar(string name, var value) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setNull(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var();
	}

	void object::erase(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		data->items.erase(name);
	}

	string object::getString(string name, string def) {
		try {
			initMemory();
			unique_lock<mutex> gaurd(data->omutex);
			auto it = data->items.find(name);
			if (it != data->items.end())
				return it->second.getString();
			if (data->parent.data)
				return data->parent.getString(name, def);
			return def;
		} catch (exception& e) {
			return def;
		}
	}

	int64_t object::getInt64(string name, int64_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt64();
		if (data->parent.data)
			return data->parent.getInt64(name, def);
		return def;
	}

	int32_t object::getInt32(string name, int32_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt32();
		if (data->parent.data)
			return data->parent.getInt32(name, def);
		return def;
	}

	int16_t object::getInt16(string name, int16_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt16();
		if (data->parent.data)
			return data->parent.getInt16(name, def);
		return def;
	}

	int8_t object::getInt8(string name, int8_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt8();
		if (data->parent.data)
			return data->parent.getInt8(name, def);
		return def;
	}

	uint64_t object::getUInt64(string name, uint64_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt64();
		if (data->parent.data)
			return data->parent.getUInt64(name, def);
		return def;
	}

	uint32_t object::getUInt32(string name, uint32_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt32();
		if (data->parent.data)
			return data->parent.getUInt32(name, def);
		return def;
	}

	uint16_t object::getUInt16(string name, uint16_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt16();
		if (data->parent.data)
			return data->parent.getUInt16(name, def);
		return def;
	}

	uint8_t object::getUInt8(string name, uint8_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt8();
		if (data->parent.data)
			return data->parent.getUInt8(name, def);
		return def;
	}

	double object::getDouble(string name, double def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getDouble();
		if (data->parent.data)
			return data->parent.getDouble(name, def);
		return def;
	}

	float object::getFloat(string name, float def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getFloat();
		if (data->parent.data)
			return data->parent.getFloat(name, def);
		return def;
	}

	bool object::getBool(string name, bool def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getBool();
		if (data->parent.data)
			return data->parent.getBool(name, def);
		return def;
	}

	list object::getList(string name, list def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getList();
		if (data->parent.data) return data->parent.getList(name);
		return def;
	}

	void object::returnList(string name, list& result) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) it->second.returnList(result);
		if (data->parent.data)
			data->parent.returnList(name, result);
	}

	void object::returnObject(string name, object& result) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end())
			return it->second.returnObject(result);
		else if (data->parent)
			return data->parent.returnObject(name, result);
	}

	object object::getObject(string name, object def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end())
			return it->second.getObject();
		else if (data->parent)
			return data->parent.getObject(name);
		return def;
	}

	method object::getMethod(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto end = data->items.end();
		auto it = data->items.begin();
		while (it != end) {
			auto key = it->first;
			if (key == name) break;
			it++;
		}
		if (it != end) return it->second.getMethod();
		if (data->parent.data) return data->parent.getMethod(name);
		return nullptr;
	}

	func object::getFunc(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto end = data->items.end();
		auto it = data->items.begin();
		while (it != end) {
			auto key = it->first;
			if (key == name) break;
			it++;
		}
		if (it != end) return it->second.getFunction();
		if (data->parent.data) return data->parent.getFunc(name);
		return nullptr;
	}

	void* object::getPtr(string name, void* def) {
		initMemory();
		if (!data) return def;
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getPtr();
		if (data->parent.data)
			return data->parent.getPtr(name, def);
		return def;
	}

	binary object::getBinary(string name, binary def) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end())
			return it->second.getBinary();
		else if (data->parent.data)
			return data->parent.getBinary(name, def);
		return def;
	}

	void object::returnBinary(string name, binary& result) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end())
			it->second.returnBinary(result);
		else if (data->parent.data)
			return data->parent.returnBinary(name, result);
	}

	var object::getVar(string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second;
		if (data->parent.data) return data->parent.getVar(name);
		return var();
	}

	var object::operator[](string name) {
		initMemory();
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second;
		if (data->parent.data) return data->parent.operator[](name);
		return var();
	}

	var object::operator()(string name, list args) {
		return this->callMethod(name, args);
	}

	var object::operator()(string name) {
		return this->callMethod(name);
	}

	bool object::operator==(object& other) {
		if (data->id == other.data->id) return true;
		return false;
	}

	object::operator bool() const {
		return bool(this->data && (this->data->items.size() > 0));
	}

	void object::parseURLEncoded(string value, object& result) {
		auto it = value.begin();
		string buffer = "";
		string key = "";
		while (it != value.end()) {
			if (isalnum(*it))
				buffer += *it;
			else if (*it == '=') {
				key = buffer;
				buffer = "";
			} else if (*it == '&') {
				auto vType = result.getType(key);
				if (vType == typeString) {
					auto copy = result.getString(key);
					auto arr = list({copy, buffer});
					result.setList(key, arr);
				} else if (vType == typeList) {
					auto arr = list();
					result.returnList(key, arr);
					arr.pushString(buffer);
				} else {
					result.setString(key, buffer);
				}
				buffer = "";
				key = "";
			} else if (*it == '.') {
				buffer += ".";
			} else if (*it == '+') {
				buffer += " ";
			} else if (*it == '%') {
				string h = "0x";
				h += *(++it);
				h += *(++it);
				auto c = (char)stoul(h, nullptr, 16);
				buffer += string(&c, 1);
			} else {
				buffer += *it;
			}
			++it;
		}
		if (key.size() > 0 && buffer.size() > 0)
			result.setString(key, buffer);
	}

	var object::loadJSON(string path) {
		try {
			ifstream ss(path);
			if (!ss.good())
				return var(genericError("file doesn't exist"));
			json j = json();
			ss >> j;
			ss.close();
			if (j.is_object()) {
				object obj(j);
				return var(obj);
			} else
				return var(genericError("not object"));
		} catch (exception& err) {
			return var(genericError(err));
		}
	}

	void object::saveJSON(string path, object value) {
		ofstream ss(path);
		json j = value.getJSON();
		ss << j.dump(1, '\t') << endl;
		ss.close();
	}

	var object::loadBSON(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_bson(data);
			if (j.is_object()) {
				auto obj = object(j);
				return var(obj);
			} else
				return var(genericError("failed to parse"));
		} catch (exception& err) {
			return var(genericError(err));
		}
	}

	void object::saveBSON(string path, object value) {
		ofstream ss(path);
		auto data = value.getBSON();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadCBOR(string path) {
		try {
			ifstream ss(path);
			auto data = binary();
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_cbor(data);
			if (j.is_object()) {
				auto obj = object(j);
				return var(obj);
			} else
				return var();
		} catch (exception& err) {
			return genericError(err);
		}
	}

	void object::saveCBOR(string path, object value) {
		ofstream ss(path);
		auto data = value.getCBOR();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadMsgPack(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_msgpack(data);
			if (j.is_object()) {
				auto obj = object(j);
				return var(obj);
			} else
				return var();
		} catch (exception& err) {
			return var();
		}
	}

	void object::saveMsgPack(string path, object value) {
		ofstream ss(path);
		auto data = value.getMsgPack();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadUBJSON(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_ubjson(data);
			if (j.is_object()) {
				auto obj = object(j);
				return obj;
			} else
				return var();
		} catch (exception& err) {
			return var();
		}
	}

	void object::saveUBJSON(string path, object value) {
		ofstream ss(path);
		auto data = value.getUBJSON();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

}  // namespace gold