#pragma once

/* <Includes> */
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "types.hpp"
#include "var.hpp"
/* </Includes> */

namespace red {
	using namespace std;
	using namespace nlohmann;
	typedef string key;
	/* <Object> */

	class object {
	 protected:
		friend class var;
		typedef map<key, var> omap;
		shared_ptr<omap> items;
		object* parent;
		uint64_t id;
		mutex omutex;

	 public:
		typedef initializer_list<omap::value_type> object_list;
		object(object* parent = nullptr);
		object(const object& copy);
		object(object copy, object* parent);
		object(object_list list, object* parent = nullptr);
		object(json value);
		~object();

		omap::iterator begin();
		omap::iterator end();

		uint64_t getSize();
		types getType(string name);
		json getJSON();
		binary getBSON();
		binary getCBOR();
		binary getMsgPack();
		binary getUBJSON();
		var callMethod(string name);
		var callMethod(string name, var args);
		void copy(object& other);
		void setParent(object* other);
		object* getParent();

		void setString(string name, char* value);
		void setInt64(string name, int64_t value);
		void setInt32(string name, int32_t value);
		void setInt16(string name, int16_t value);
		void setInt8(string name, int8_t value);
		void setUInt64(string name, uint64_t value);
		void setUInt32(string name, uint32_t value);
		void setUInt16(string name, uint16_t value);
		void setUInt8(string name, uint8_t value);
		void setDouble(string name, double value);
		void setFloat(string name, float value);
		void setBool(string name, bool value);
		void setArray(string name, array value);
		void setObject(string name, object value);
		void setMethod(string name, method value);
		void setPtr(string name, void* value);
		void setVar(string name, var var);
		void setNull(string name);

		const char* getString(string name, char* def = 0);
		int64_t getInt64(string name, int64_t def = 0);
		int32_t getInt32(string name, int32_t def = 0);
		int16_t getInt16(string name, int16_t def = 0);
		int8_t getInt8(string name, int8_t def = 0);
		uint64_t getUInt64(string name, uint64_t def = 0);
		uint32_t getUInt32(string name, uint32_t def = 0);
		uint16_t getUInt16(string name, uint16_t def = 0);
		uint8_t getUInt8(string name, uint8_t def = 0);
		double getDouble(string name, double def = 0);
		float getFloat(string name, float def = 0);
		bool getBool(string name, bool def = false);
		array* getArray(string name, array* def = 0);
		object* getObject(string name, object* def = 0);
		method getMethod(string name, method def = 0);
		void* getPtr(string name, void* def = 0);
		var getVar(string name);

		var operator[](string name);
		var operator()(string name, var);
		var operator()(string name);
		bool operator==(const object& other);
		object& operator=(const object rhs);

		static var loadJSON(string path);
		static void saveJSON(string path, object value);

		static var loadBSON(string path);
		static void saveBSON(string path, object value);

		static var loadCBOR(string path);
		static void saveCBOR(string path, object value);

		static var loadMsgPack(string path);
		static void saveMsgPack(string path, object value);

		static var loadUBJSON(string path);
		static void saveUBJSON(string path, object value);

		template <class oT>
		oT create(string name, object* config = nullptr) {
			oT o = config ? oT(*config) : oT();
			setObject(name, o);
			return *(oT*)getObject(name);
		}
	};

	/* </Object> */
}  // namespace red