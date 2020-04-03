#pragma once

#include <memory>
#include <string>

#include "types.hpp"

namespace red {
	using namespace std;
	class var {
	 protected:
		typedef shared_ptr<class varContainer> varPtr;
		varPtr sPtr;

	 public:
		var();
		var(const var& copy);
		var(char* string);
		var(const char* string);
		var(string string);
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
		var(bool v);
		var(array v);
		var(object v);
		var(void* v);
		var(method v);
		var(exception v);
		~var();

		var& operator=(const var& rhs);
		bool operator==(const var& rhs);
		bool operator!=(const var& rhs);

		types getType();
		const char* getTypeString();

		bool isString();
		bool isNumber();
		bool isFloating();
		bool isSigned();
		bool isBool();
		bool isObject();
		bool isObject(object proto);
		bool isArray();
		bool isEmpty();
		bool isError();

		const char* getString();
		int64_t getInt64();
		int32_t getInt32();
		int16_t getInt16();
		int8_t getInt8();
		uint64_t getUInt64();
		uint32_t getUInt32();
		uint16_t getUInt16();
		uint8_t getUInt8();
		double getDouble();
		float getFloat();
		bool getBool();
		array* getArray();
		object* getObject();
		method getMethod();
		void* getPtr();

		operator const char*() const;
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
		operator array*() const;
		operator object*() const;
		operator method() const;
		operator void*() const;
		var operator()(object&, var);
		var operator()(object&);
	};

}  // namespace red