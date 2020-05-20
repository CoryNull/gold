#pragma once

#include <filesystem>

#include "types.hpp"

namespace gold {
	using namespace std;
	using path = std::filesystem::path;
	struct file : public object {
	 protected:
		static object& getPrototype();

	 public:
		file();
		file(path p);
		file(binary data);

		var save(list args = {});
		var load(list args = {});
		var trash(list args = {});
		var getWriteTime(list args = {});
		var hash(list args = {});
		var extension();
		var asJSON();
		var asBSON();
		var asCBOR();
		var asMsgPack();
		var asUBJSON();

		operator binary();
		operator string();
		operator string_view();

		static file readFile(path p);
		static file saveFile(path p, binary data);
		static object& recursiveReadDirectory(
			path p, object& results);
		static var parseJSON(binary data);
		static var parseBSON(binary data);
		static var parseCBOR(binary data);
		static var parseMsgPack(binary data);
		static var parseUBJSON(binary data);
		static string serializeJSON(var data, bool pretty = false);
		static binary serializeBSON(var data);
		static binary serializeCBOR(var data);
		static binary serializeMsgPack(var data);
		static binary serializeUBJSON(var data);
		static binary decodeDataURL(string v);
		static binary decodeBase64(string v);
	};
}  // namespace gold