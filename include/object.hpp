#pragma once

/* <Includes> */
#include <libbson-1.0/bson.h>
#include "types.hpp"
#include <vector>
/* </Includes> */

namespace red {
	using namespace std;
	/* <Object> */

	struct objectItem {
		char* name;
		void* data;
		types type;
	};
	class object {
	protected:
		vector<objectItem> items;
		object* parent;

		bool getKeyIndex(const char* name, uint64_t& index);
	 public:
		object(object* parent = nullptr);
		~object();

		uint64_t getSize();
		void getFromIndex(uint64_t i, char*& name, types& type);
		types getType(const char* name);
		char* getJSON(uint64_t& size);
		void getBSON(bson_t& doc);
		array* callMethod(const char* name, array* args = 0);

		void setString(const char* name, char* value);
		void setInt64(const char* name, int64_t value);
		void setInt32(const char* name, int32_t value);
		void setInt16(const char* name, int16_t value);
		void setInt8(const char* name, int8_t value);
		void setUInt64(const char* name, uint64_t value);
		void setUInt32(const char* name, uint32_t value);
		void setUInt16(const char* name, uint16_t value);
		void setUInt8(const char* name, uint8_t value);
		void setDouble(const char* name, double value);
		void setFloat(const char* name, float value);
		void setBool(const char* name, bool value);
		void setArray(const char* name, array* value);
		void setObject(const char* name, object* value);
		void setMethod(const char* name, method value);

		char* getString(const char* name, char* def = 0);
		int64_t getInt64(const char* name, int64_t def = 0);
		int32_t getInt32(const char* name, int32_t def = 0);
		int16_t getInt16(const char* name, int16_t def = 0);
		int8_t getInt8(const char* name, int8_t def = 0);
		uint64_t getUInt64(const char* name, uint64_t def = 0);
		uint32_t getUInt32(const char* name, uint32_t def = 0);
		uint16_t getUInt16(const char* name, uint16_t def = 0);
		uint8_t getUInt8(const char* name, uint8_t def = 0);
		double getDouble(const char* name, double def = 0);
		float getFloat(const char* name, float def = 0);
		bool getBool(const char* name, bool def = false);
		array* getArray(const char* name, array* def = 0);
		object* getObject(const char* name, object* def = 0);
		method getMethod(const char* name, method def = 0);
	};

	/* </Object> */
}  // namespace red