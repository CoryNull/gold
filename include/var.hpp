#pragma once

#include <memory>
#include <string>

#include "types.hpp"

namespace gold {
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
		var(array v);
		var(object v);
		var(void* v, types t);
		var(method v);
		var(func v);
		var(genericError v);
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
		bool isObject(object proto) const;
		bool isArray() const;
		bool isEmpty() const;
		bool isError() const;
		bool isFunction() const;
		bool isMethod() const;

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
		array* getArray() const;
		object* getObject() const;
		method getMethod() const;
		func getFunction() const;
		void* getPtr() const;
		genericError* getError() const;

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
		operator array*() const;
		operator object*() const;
		operator method() const;
		operator func() const;
		operator void*() const;
		operator genericError*() const;
		friend ostream& operator<<(ostream& os, const var& v);
		var operator()(object&, varList) const;
		var operator()(object&) const;
		var operator()(varList) const;
	};

}  // namespace gold