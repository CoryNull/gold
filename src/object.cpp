
#include "object.hpp"
#include "array.hpp"
#include "memory.hpp"
#include <string.h>

namespace red {

	object::object(object* parent) : items(), parent(parent) {}

	object::~object() {
		for (uint64_t i = 0; i < this->items.size(); ++i) {
			auto item = this->items[i];
			if (isCopyable(item.type))
				free(item.data);
		}
	}

	uint64_t object::getSize() { return this->items.size(); }

	void object::getFromIndex(uint64_t i, char*& name, types& type) {
		if (i >= this->items.size()) {
			name = nullptr;
			type = typeNull;
			return;
		}
		auto item = this->items[i];
		name = item.name;
		type = item.type;
	}

	bool object::getKeyIndex(const char* name, uint64_t& index) {
		if (this == NULL)
			return false;
		for (uint64_t i = 0; i < this->items.size(); ++i) {
			auto item = this->items[i];
			if (strcmp(item.name, name) == 0) {
				index = i;
				return true;
			}
		}
		return false;
	}

	types object::getType(const char* name) {
		uint64_t index;
		if (this->getKeyIndex(name, index)) {
			return this->items[index].type;
		}
		return typeNull;
	}

	char* object::getJSON(uint64_t& size) {
		bson_t doc;
		bson_init(&doc);
		this->getBSON(doc);
		size_t jsonSize = 0;
		char* json = bson_as_relaxed_extended_json(&doc, &jsonSize);
		void* dataPtr = alloc(jsonSize + 1, typeString);
		memset(dataPtr, 0, jsonSize + 1);
		memcpy(dataPtr, json, jsonSize);
		bson_free(json);
		bson_destroy(&doc);
		return (char*)dataPtr;
	}

	void object::getBSON(bson_t& doc) {
		for (uint32_t i = 0; i < this->items.size(); ++i) {
			auto item = this->items[i];
			char* key = item.name;
			void* dataPtr = item.data;
			bson_t child;
			switch (item.type) {
				case typeArray:
					bson_append_array_begin(&doc, key, -1, &child);
					((array*)dataPtr)->getBSON(child);
					bson_append_array_end(&doc, &child);
					break;
				case typeObject:
					bson_append_document_begin(&doc, key, -1, &child);
					((object*)dataPtr)->getBSON(child);
					bson_append_document_end(&doc, &child);
					break;
				case typeBool:
					bson_append_bool(&doc, key, -1, *(bool*)dataPtr);
					break;
				case typeDouble:
					bson_append_double(&doc, key, -1, *(double*)dataPtr);
					break;
				case typeFloat:
					bson_append_double(&doc, key, -1, (double)*(float*)dataPtr);
					break;
				case typeInt64:
					bson_append_int64(&doc, key, -1, *(int64_t*)dataPtr);
					break;
				case typeInt32:
					bson_append_int32(&doc, key, -1, *(int32_t*)dataPtr);
					break;
				case typeInt16:
					bson_append_int32(&doc, key, -1, (int32_t) * (int16_t*)dataPtr);
					break;
				case typeInt8:
					bson_append_int32(&doc, key, -1, (int32_t) * (int8_t*)dataPtr);
					break;
				case typeUInt64:
					bson_append_double(&doc, key, -1, (double)*(uint64_t*)dataPtr);
					break;
				case typeUInt32:
					bson_append_int64(&doc, key, -1, (int64_t) * (uint32_t*)dataPtr);
					break;
				case typeUInt16:
					bson_append_int32(&doc, key, -1, (int32_t) * (uint16_t*)dataPtr);
					break;
				case typeUInt8:
					bson_append_int32(&doc, key, -1, (int32_t) * (uint8_t*)dataPtr);
					break;
				case typeString:
					bson_append_utf8(&doc, key, -1, (char*)dataPtr, -1);
					break;
				case typeNull:
					bson_append_null(&doc, key, -1);
					break;
				default:
					break;
			}
		}
	}

	array* object::callMethod(const char* name, array* args) {
		method method = this->getMethod(name, 0);
		if (method != nullptr)
			return (*method)(this, args);
		return nullptr;
	}

	void object::setString(const char* name, char* value) {
		uint64_t size = strlen(value);
		void* data = new char[size + 1];
		memset((void*)(((uint64_t)data) + size), 0, 1);
		memcpy(data, value, size);
		uint64_t index = 0;
		objectItem item = {(char*)name, data, typeString};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setInt64(const char* name, int64_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeInt64};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setInt32(const char* name, int32_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(int64_t)value, typeInt32};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setInt16(const char* name, int16_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(int64_t)value, typeInt16};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setInt8(const char* name, int8_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(int64_t)value, typeInt8};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setUInt64(const char* name, uint64_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeUInt64};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setUInt32(const char* name, uint32_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(uint64_t)value, typeUInt32};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setUInt16(const char* name, uint16_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(uint64_t)value, typeUInt16};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setUInt8(const char* name, uint8_t value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)(uint64_t)value, typeUInt8};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setDouble(const char* name, double value) {
		uint64_t index = 0;
		void* data = nullptr;
		if (isCopyable(typeDouble)) {
			data = new double;
			memcpy(data, &value, sizeof(double));
		} else
			memcpy(&data, &value, sizeof(double));
		objectItem item = {(char*)name, data, typeDouble};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setFloat(const char* name, float value) {
		uint64_t index = 0;
		void* data = nullptr;
		memcpy(&data, &value, sizeof(float));
		objectItem item = {(char*)name, data, typeFloat};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setBool(const char* name, bool value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeBool};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setArray(const char* name, array* value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeArray};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setObject(const char* name, object* value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeObject};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	void object::setMethod(const char* name, method value) {
		uint64_t index = 0;
		objectItem item = {(char*)name, (void*)value, typeMethod};
		if (this->getKeyIndex(name, index))
			this->items[index] = item;
		else
			this->items.push_back(item);
	}

	char* object::getString(const char* name, char* def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (char*)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getString(name, def);
		return def;
	}

	int64_t object::getInt64(const char* name, int64_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (int64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getInt64(name, def);
		return def;
	}

	int32_t object::getInt32(const char* name, int32_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (int32_t)(int64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getInt32(name, def);
		return def;
	}

	int16_t object::getInt16(const char* name, int16_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (int16_t)(int64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getInt16(name, def);
		return def;
	}

	int8_t object::getInt8(const char* name, int8_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (int8_t)(int64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getInt8(name, def);
		return def;
	}

	uint64_t object::getUInt64(const char* name, uint64_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (uint64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getUInt64(name, def);
		return def;
	}

	uint32_t object::getUInt32(const char* name, uint32_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (uint32_t)(uint64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getUInt32(name, def);
		return def;
	}

	uint16_t object::getUInt16(const char* name, uint16_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (uint16_t)(uint64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getUInt16(name, def);
		return def;
	}

	uint8_t object::getUInt8(const char* name, uint8_t def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (uint8_t)(uint64_t)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getUInt8(name, def);
		return def;
	}

	double object::getDouble(const char* name, double def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			double data = 0;
			if (isCopyable(typeDouble))
				memcpy(&data, item.data, sizeof(double));
			else
				memcpy(&data, &item.data, sizeof(double));

			return data;
		}
		if (this->parent != nullptr)
			return this->parent->getDouble(name, def);
		return def;
	}

	float object::getFloat(const char* name, float def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			float data = 0;
			memcpy(&data, &item.data, sizeof(float));
			return data;
		}
		if (this->parent != nullptr)
			return this->parent->getFloat(name, def);
		return def;
	}

	bool object::getBool(const char* name, bool def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (bool)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getBool(name, def);
		return def;
	}

	array* object::getArray(const char* name, array* def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (array*)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getArray(name, def);
		return def;
	}

	object* object::getObject(const char* name, object* def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (object*)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getObject(name, def);
		return def;
	}

	method object::getMethod(const char* name, method def) {
		uint64_t index = 0;
		if (this->getKeyIndex(name, index)) {
			auto item = this->items[index];
			return (method)item.data;
		}
		if (this->parent != nullptr)
			return this->parent->getMethod(name, def);
		return def;
	}

}  // namespace red