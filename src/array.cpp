
#include "array.hpp"
#include "object.hpp"

namespace red {

	array::array() : items() {}

	array::~array() {
		for (uint64_t i = 0; i < this->items.size(); ++i) {
			auto item = this->items[i];
			if (isCopyable(item.type))
				free(item.data);
		}
	}

	uint64_t array::getSize() { return this->items.size(); }

	void array::pop() { this->items.pop_back(); }

	types array::getType(uint64_t index) { return this->items[index].type; }

	char* array::getJSON(uint64_t& size) {
		bson_t doc;
		bson_init(&doc);
		this->getBSON(doc);
		size_t jsonSize = 0;
		char* json = bson_array_as_json(&doc, &jsonSize);
		void* dataPtr = new char[jsonSize + 1];
		memset(dataPtr, 0, jsonSize + 1);
		memcpy(dataPtr, json, jsonSize);
		bson_free(json);
		bson_destroy(&doc);
		return (char*)dataPtr;
	}

	void array::getBSON(bson_t& doc) {
		for (uint32_t i = 0; i < this->items.size(); ++i) {
			char iStr[16];
			const char* iStrPtr;
			size_t iSize = bson_uint32_to_string(i, &iStrPtr, iStr, 16);
			void* dataPtr = this->items[i].data;
			bson_t child;
			switch (this->items[i].type) {
				case typeArray:
					bson_append_array_begin(&doc, iStrPtr, iSize, &child);
					((array*)dataPtr)->getBSON(child);
					bson_append_array_end(&doc, &child);
					break;
				case typeObject:
					bson_append_document_begin(&doc, iStrPtr, iSize, &child);
					((object*)dataPtr)->getBSON(child);
					bson_append_document_end(&doc, &child);
					break;
				case typeBool:
					bson_append_bool(&doc, iStrPtr, iSize, this->getBool(i));
					break;
				case typeDouble:
					bson_append_double(&doc, iStrPtr, iSize, this->getDouble(i));
					break;
				case typeFloat:
					bson_append_double(&doc, iStrPtr, iSize, (double)this->getFloat(i));
					break;
				case typeInt64:
					bson_append_int64(&doc, iStrPtr, iSize, this->getInt64(i));
					break;
				case typeInt32:
					bson_append_int32(&doc, iStrPtr, iSize, this->getInt32(i));
					break;
				case typeInt16:
					bson_append_int32(&doc, iStrPtr, iSize, (int32_t)this->getInt16(i));
					break;
				case typeInt8:
					bson_append_int32(&doc, iStrPtr, iSize, (int32_t)this->getInt8(i));
					break;
				case typeUInt64:
					bson_append_double(&doc, iStrPtr, iSize, (double)this->getUInt64(i));
					break;
				case typeUInt32:
					bson_append_int64(&doc, iStrPtr, iSize, (int64_t)this->getUInt32(i));
					break;
				case typeUInt16:
					bson_append_int32(&doc, iStrPtr, iSize, (int32_t)this->getUInt16(i));
					break;
				case typeUInt8:
					bson_append_int32(&doc, iStrPtr, iSize, (int32_t)this->getUInt8(i));
					break;
				case typeString:
					bson_append_utf8(&doc, iStrPtr, iSize, this->getString(i), -1);
					break;
				case typeNull:
					bson_append_null(&doc, iStrPtr, iSize);
					break;
				default:
					break;
			}
		}
	}

	void array::pushData(void* dataPtr, uint64_t size, types type) {
		void* data = nullptr;
		if (isCopyable(type)) {
			void* data = new char[size + 1];
			memset(data, 0, size + 1);
			memcpy(data, dataPtr, size);
		} else {
			memcpy(&data, dataPtr, size);
		}
		this->items.push_back({data, type});
	}

	void array::pushString(char* value) {
		this->pushData(value, strlen(value), typeString);
	}

	void array::pushInt64(int64_t value) {
		this->pushData(&value, sizeof(int64_t), typeInt64);
	}

	void array::pushInt32(int32_t value) {
		this->pushData(&value, sizeof(int32_t), typeInt32);
	}

	void array::pushInt16(int16_t value) {
		this->pushData(&value, sizeof(int16_t), typeInt16);
	}

	void array::pushInt8(int8_t value) {
		this->pushData(&value, sizeof(int8_t), typeInt8);
	}

	void array::pushUInt64(uint64_t value) {
		this->pushData(&value, sizeof(uint64_t), typeUInt64);
	}

	void array::pushUInt32(uint32_t value) {
		this->pushData(&value, sizeof(uint32_t), typeUInt32);
	}

	void array::pushUInt16(uint16_t value) {
		this->pushData(&value, sizeof(uint16_t), typeUInt16);
	}

	void array::pushUInt8(uint8_t value) {
		this->pushData(&value, sizeof(uint8_t), typeUInt8);
	}

	void array::pushDouble(double value) {
		this->pushData(&value, sizeof(double), typeDouble);
	}

	void array::pushFloat(float value) {
		this->pushData(&value, sizeof(float), typeFloat);
	}

	void array::pushBool(bool value) {
		this->pushData(&value, sizeof(bool), typeBool);
	}

	void array::pushArray(array* value) {
		this->pushData(value, sizeof(array), typeArray);
	}

	void array::pushObject(object* value) {
		this->pushData(value, sizeof(object), typeObject);
	}

	void array::setString(uint64_t index, char* value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		uint64_t size = strlen(value);
		void* data = new char[size + 1];
		memset((void*)(((uint64_t)data) + size), 0, 1);
		memcpy(data, value, size);
		this->items[index] = {data, typeString};
	}

	void array::setInt64(uint64_t index, int64_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(int64_t));
		this->items[index] = {data, typeInt64};
	}

	void array::setInt32(uint64_t index, int32_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(int32_t));
		this->items[index] = {data, typeInt32};
	}

	void array::setInt16(uint64_t index, int16_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(int16_t));
		this->items[index] = {data, typeInt16};
	}

	void array::setInt8(uint64_t index, int8_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(int8_t));
		this->items[index] = {data, typeInt8};
	}

	void array::setUInt64(uint64_t index, uint64_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(uint64_t));
		this->items[index] = {data, typeUInt64};
	}

	void array::setUInt32(uint64_t index, uint32_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(uint32_t));
		this->items[index] = {data, typeUInt32};
	}

	void array::setUInt16(uint64_t index, uint16_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(uint16_t));
		this->items[index] = {data, typeUInt16};
	}

	void array::setUInt8(uint64_t index, uint8_t value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(uint8_t));
		this->items[index] = {data, typeUInt8};
	}

	void array::setDouble(uint64_t index, double value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		if (isCopyable(typeDouble)) {
			data = new double;
			memcpy(data, &value, sizeof(double));
		} else
			memcpy(&data, &value, sizeof(double));
		this->items[index] = {data, typeDouble};
	}

	void array::setFloat(uint64_t index, float value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(float));
		this->items[index] = {data, typeFloat};
	}

	void array::setBool(uint64_t index, bool value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(bool));
		this->items[index] = {data, typeBool};
	}

	void array::setArray(uint64_t index, array* value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(array*));
		this->items[index] = {data, typeArray};
	}

	void array::setObject(uint64_t index, object* value) {
		if (this->items.size() <= index)
			this->items.resize(index);
		void* data = nullptr;
		memcpy(&data, &value, sizeof(object*));
		this->items[index] = {data, typeObject};
	}

	char* array::getString(uint64_t index, char* def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeString)
			return (char*)item.data;
		return def;
	}

	int64_t array::getInt64(uint64_t index, int64_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeInt64) {
			int64_t data = 0;
			if (isCopyable(typeInt64))
				memcpy(&data, (int64_t*)item.data, sizeof(int64_t));
			else
				memcpy(&data, &item.data, sizeof(int64_t));
			return data;
		}
		return def;
	}

	int32_t array::getInt32(uint64_t index, int32_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeInt32)
			return (int32_t)(int64_t)item.data;
		return def;
	}

	int16_t array::getInt16(uint64_t index, int16_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeInt16)
			return (int16_t)(int64_t)item.data;
		return def;
	}

	int8_t array::getInt8(uint64_t index, int8_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeInt8)
			return (int8_t)(int64_t)item.data;
		return def;
	}

	uint64_t array::getUInt64(uint64_t index, uint64_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeUInt64) {
			uint64_t data = 0;
			if (isCopyable(typeUInt64))
				memcpy(&data, (uint64_t*)item.data, sizeof(uint64_t));
			else
				memcpy(&data, &item.data, sizeof(uint64_t));
			return data;
		}
		return def;
	}

	uint32_t array::getUInt32(uint64_t index, uint32_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeUInt32)
			return (uint32_t)(uint64_t)item.data;
		return def;
	}

	uint16_t array::getUInt16(uint64_t index, uint16_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeUInt16)
			return (uint16_t)(uint64_t)item.data;
		return def;
	}

	uint8_t array::getUInt8(uint64_t index, uint8_t def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeUInt8)
			return (uint8_t)(uint64_t)item.data;
		return def;
	}

	double array::getDouble(uint64_t index, double def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeDouble) {
			double data = 0;
			if (isCopyable(typeDouble)) {
				memcpy(&data, (double*)item.data, sizeof(double));
			} else
				memcpy(&data, &item.data, sizeof(double));
			return data;
		}
		return def;
	}

	float array::getFloat(uint64_t index, float def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeFloat) {
			float data = 0;
			memcpy(&data, &item.data, sizeof(float));
			return data;
		}
		return def;
	}

	bool array::getBool(uint64_t index, bool def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeBool)
			return (bool)item.data;
		return def;
	}

	array* array::getArray(uint64_t index, array* def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeArray)
			return (array*)item.data;
		return def;
	}

	object* array::getObject(uint64_t index, object* def) {
		if (index >= this->items.size())
			return def;
		auto item = this->items[index];
		if (item.type == typeObject)
			return (object*)item.data;
		return def;
	}
}  // namespace red