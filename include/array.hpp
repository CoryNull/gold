#pragma once

#include <libbson-1.0/bson.h>
#include <vector>
#include "types.hpp"

namespace red {
	/* <Array> */
	struct arrayItem {
		void* data;
		types type;
	};
	class array {
	protected:
		std::vector<arrayItem> items;

		void pushData(void* dataPtr, uint64_t size, types type);
	public:
		array();
		~array();

		uint64_t getSize();
		void pop();
		types getType(uint64_t index);
		char* getJSON(uint64_t& size);
		void getBSON(bson_t& doc);

		void pushString(char* value);
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
		void pushArray(array* value);
		void pushObject(object* value);

		void setString(uint64_t index, char* value);
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
		void setArray(uint64_t index, array* value);
		void setObject(uint64_t index, object* value);

		char* getString(uint64_t index, char* def = 0);
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
		array* getArray(uint64_t index, array* def = 0);
		object* getObject(uint64_t index, object* def = 0);
	};

	/* </Array> */
}  // namespace red