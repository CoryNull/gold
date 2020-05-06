#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <experimental/source_location>
#include <functional>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gold {
	using namespace std;
	using json = nlohmann::json;
	using sourceLocation = std::experimental::source_location;
	/* <Types> */
	struct object;
	struct list;
	struct var;
	using method = var (object::*)(const list&);
	using func = function<var(const list&)>;
	using binary = vector<uint8_t>;
	using key = string;

	typedef enum types_t {
		typeNull = 0,
		typeList,
		typeObject,
		typeMethod,
		typeFunction,
		typePtr,
		typeString,
		typeBinary,
		typeInt64,
		typeInt32,
		typeInt16,
		typeInt8,
		typeUInt64,
		typeUInt32,
		typeUInt16,
		typeUInt8,
		typeDouble,
		typeFloat,
		typeBool,
		typeException
	} types;

	class genericError : public exception {
	 public:
		const string msg;
		const sourceLocation loc;

		genericError(
			const exception& copy,
			const sourceLocation& l = sourceLocation::current());
		genericError(const genericError& copy);
		genericError(
			string_view message,
			const sourceLocation& l = sourceLocation::current());
		operator string() const;
		friend ostream& operator<<(
			ostream& os, const genericError& dt);
	};

	struct var {
	 protected:
		struct varContainer {
		 public:
			union {
				void* ptr;
				string* str;
				list* a;
				object* o;
				double d;
				float f;
				int64_t i64;
				int32_t i32;
				int16_t i16;
				int8_t i8;
				uint64_t u64;
				uint32_t u32;
				uint16_t u16;
				uint8_t u8;
				bool b;
				binary* bv;
				method m;
				func fu;
				genericError* e;
			};
			types type;
			~varContainer();

			varContainer();
			varContainer(const varContainer& other);
		};
		typedef shared_ptr<varContainer> varPtr;
		static varPtr autoNull;
		varPtr sPtr;

	 public:
		var();
		var(const var& copy);
		var(const char* string);
		var(string string);
		var(const binary& bin);
		var(int64_t v);
		var(int32_t v);
		var(int16_t v);
		var(int8_t v);
		var(uint64_t v);
		var(uint32_t v);
		var(uint16_t v);
		var(uint8_t v);
		var(double v);
		var(float v);
		var(const list& v);
		var(const object& v);
		var(void* v, types t);
		var(method v);
		var(func v);
		var(const genericError& v);
		var(bool v);
		template <class T> var(T* v, types t) : var(v, t) {}
		~var();

		var& operator=(const var& rhs);
		bool operator==(const var& rhs) const;
		bool operator!=(const var& rhs) const;

		types getType() const;
		const char* getTypeString() const;

		bool isString() const;
		bool isNumber() const;
		bool isFloating() const;
		bool isSigned() const;
		bool isBool() const;
		bool isObject() const;
		bool isObject(object& proto) const;
		bool isList() const;
		bool isEmpty() const;
		bool isError() const;
		bool isFunction() const;
		bool isMethod() const;
		bool isBinary() const;

		string getString() const;
		int64_t getInt64() const;
		int32_t getInt32() const;
		int16_t getInt16() const;
		int8_t getInt8() const;
		uint64_t getUInt64() const;
		uint32_t getUInt32() const;
		uint16_t getUInt16() const;
		uint8_t getUInt8() const;
		double getDouble() const;
		float getFloat() const;
		bool getBool() const;
		list getList() const;
		object getObject() const;
		void returnList(list& result) const;
		void returnObject(object& result) const;
		method getMethod() const;
		func getFunction() const;
		void* getPtr() const;
		genericError* getError() const;
		binary getBinary() const;
		void returnBinary(binary& result) const;
		template <typename OT = object>
		void returnObject(OT& result) const {
			auto container = sPtr.get();
			if (container && container->type == typeObject)
				result = reinterpret_cast<OT&>(*container->o);
		}
		template <typename OT = object> OT getObject() const {
			auto container = sPtr.get();
			if (container && container->type == typeObject)
				return reinterpret_cast<OT&>(*container->o);
			return OT();
		}

		operator const char*() const;
		operator string() const;
		operator int64_t() const;
		operator int32_t() const;
		operator int16_t() const;
		operator int8_t() const;
		operator uint64_t() const;
		operator uint32_t() const;
		operator uint16_t() const;
		operator uint8_t() const;
		operator double() const;
		operator float() const;
		operator bool() const;
		operator method() const;
		operator func() const;
		operator void*() const;
		operator genericError*() const;
		friend ostream& operator<<(ostream& os, const var& v);
		var operator()(object, list) const;
		var operator()(object) const;
		var operator()(list) const;
	};
	/* <List> */
	struct list {
	 protected:
		friend struct var;
		typedef std::vector<var> avec;
		struct arrData {
			avec items;
			mutex amutex;
		};
		shared_ptr<arrData> data;

		void pushData(void* dataPtr, uint64_t size, types type);
		void initMemory();

	 public:
		typedef initializer_list<avec::value_type> initList;

		list();
		list(list& copy);
		list(json value);
		list(initList list);
		list(var value);
		~list();

		uint64_t size();
		void pop();
		types getType(uint64_t index);
		json getJSON();
		vector<uint8_t> getBSON();
		vector<uint8_t> getCBOR();
		vector<uint8_t> getMsgPack();
		vector<uint8_t> getUBJSON();

		avec::iterator begin();
		avec::iterator end();
		avec::reverse_iterator rbegin();
		avec::reverse_iterator rend();
		void erase(avec::iterator i);
		avec::iterator find(object& proto);
		avec::iterator find(object& proto, avec::iterator start);
		avec::iterator find(var item);
		avec::iterator find(var item, avec::iterator start);
		void resize(size_t newSize);

		list operator+=(list item);
		list operator+=(var item);
		list operator-=(var item);
		operator bool() const;
		var operator[](uint64_t index) const;

		void pushString(char* value);
		void pushString(const char* value);
		void pushString(string value);
		void pushInt64(int64_t value);
		void pushInt32(int32_t value);
		void pushInt16(int16_t value);
		void pushInt8(int8_t value);
		void pushUInt64(uint64_t value);
		void pushUInt32(uint32_t value);
		void pushUInt16(uint16_t value);
		void pushUInt8(uint8_t value);
		void pushDouble(double value);
		void pushFloat(float value);
		void pushBool(bool value);
		void pushList(list value);
		void pushObject(object value);
		void pushMethod(method& value);
		void pushFunc(func value);
		void pushPtr(void* value);
		void pushVar(var value);
		void pushNull();

		void setString(uint64_t index, char* value);
		void setString(uint64_t index, string value);
		void setInt64(uint64_t index, int64_t value);
		void setInt32(uint64_t index, int32_t value);
		void setInt16(uint64_t index, int16_t value);
		void setInt8(uint64_t index, int8_t value);
		void setUInt64(uint64_t index, uint64_t value);
		void setUInt32(uint64_t index, uint32_t value);
		void setUInt16(uint64_t index, uint16_t value);
		void setUInt8(uint64_t index, uint8_t value);
		void setDouble(uint64_t index, double value);
		void setFloat(uint64_t index, float value);
		void setBool(uint64_t index, bool value);
		void setList(uint64_t index, list value);
		void setObject(uint64_t index, object value);
		void setFunc(uint64_t index, func& value);
		void setMethod(uint64_t index, method& value);
		void setPtr(uint64_t index, void* value);
		void setNull(uint64_t index);

		const char* getString(uint64_t index, char* def = 0);
		string getString(uint64_t index, string def);
		int64_t getInt64(uint64_t index, int64_t def = 0);
		int32_t getInt32(uint64_t index, int32_t def = 0);
		int16_t getInt16(uint64_t index, int16_t def = 0);
		int8_t getInt8(uint64_t index, int8_t def = 0);
		uint64_t getUInt64(uint64_t index, uint64_t def = 0);
		uint32_t getUInt32(uint64_t index, uint32_t def = 0);
		uint16_t getUInt16(uint64_t index, uint16_t def = 0);
		uint8_t getUInt8(uint64_t index, uint8_t def = 0);
		double getDouble(uint64_t index, double def = 0);
		float getFloat(uint64_t index, float def = 0);
		bool getBool(uint64_t index, bool def = false);
		void returnList(uint64_t index, list& result);
		list getList(uint64_t index, list def = list());
		object getObject(uint64_t index, object def);
		method getMethod(uint64_t index, method def = 0);
		func getFunc(uint64_t index, func def = 0);
		void returnObject(uint64_t index, object& result);
		void* getPtr(uint64_t index, void* def = 0);

		template <typename OT = object>
		void returnObject(uint64_t index, OT& result) {
			returnObject(index, (object&)result);
		}
		template <typename OT = object>
		OT getObject(uint64_t index, OT def = OT());
	};
	using array = list;

	/* </List> */
	/* <Object> */
	struct object;
	struct objData;
	struct object {
	 protected:
		friend struct var;
		friend struct objData;
		typedef map<key, var> omap;
		typedef shared_ptr<struct objData> ptr;
		ptr data;

		static ptr newObjData(omap m = {}, uint64_t id = 0);
		static ptr newObjData(
			object& p, omap m = {}, uint64_t id = 0);

		void initMemory();
		void findParent();

	 public:
		typedef initializer_list<omap::value_type> initList;
		object(initList copy);
		object(json value);
		object(var value);
		object();

		omap::iterator begin();
		omap::iterator end();

		uint64_t size();
		types getType(string name);
		var callMethod(string name);
		var callMethod(string name, list args);
		void copy(object& other);
		void setParent(object other);
		object getParent();

		void setString(string name, string value);
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
		void setList(string name, list value);
		void setObject(string name, object value);
		void setMethod(string name, method& value);
		void setFunc(string name, func& value);
		void setPtr(string name, void* value);
		void setBinary(string name, binary value);
		void setVar(string name, var var);
		void setNull(string name);
		void erase(string name);

		string getString(string name, string def = "");
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
		list getList(string name, list def = list());
		void returnList(string name, list& result);
		void returnObject(string name, object& result);
		object getObject(string name, object def = object());
		method getMethod(string name);
		func getFunc(string name);
		void* getPtr(string name, void* def = 0);
		binary getBinary(string name, binary def = binary());
		void returnBinary(string name, binary& result);
		var getVar(string name);
		template <typename OT = object>
		void returnObject(string name, OT& result) {
			returnObject(name, (object&)result);
		}
		template <typename OT = object>
		OT getObject(string name, OT def = OT()) {
			auto obj = getObject(name);
			if (obj) return *reinterpret_cast<OT*>(&obj);
			return def;
		}

		var operator[](string name);
		var operator()(string name, list);
		var operator()(string name);
		bool operator==(object&);
		operator bool() const;

		static void parseURLEncoded(string value, object& result);

		json getJSON();
		static var loadJSON(string path);
		static void saveJSON(string path, object value);

		binary getBSON();
		static var loadBSON(string path);
		static void saveBSON(string path, object value);

		binary getCBOR();
		static var loadCBOR(string path);
		static void saveCBOR(string path, object value);

		binary getMsgPack();
		static var loadMsgPack(string path);
		static void saveMsgPack(string path, object value);

		binary getUBJSON();
		static var loadUBJSON(string path);
		static void saveUBJSON(string path, object value);

		template <class oT = object>
		oT create(string name, object config = object()) {
			auto o = config ? oT(config) : oT();
			setObject(name, o);
			return o;
		}
	};
	using obj = object;

	template <typename OT>
	OT list::getObject(uint64_t index, OT def) {
		auto obj = getObject(index, (object)def);
		return OT(*reinterpret_cast<OT*>(&obj));
	}

	/* </Object> */

	bool isCopyable(types type);
	const char* getTypeString(types type);

	/* </Types> */
}  // namespace gold
