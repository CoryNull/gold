#pragma once

#include "types.hpp"
#include <memory>
#include <string>

namespace red {
	using namespace std;
	class var {
	 protected:
		shared_ptr<void*> sPtr;
		types type;
	 public:
		var();
		var(var& copy);
		var(char* string);
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

		var& operator=(const var& rhs);

		types getType();

		char* getString();
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

		operator char*() const;
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
	};
}  // namespace red