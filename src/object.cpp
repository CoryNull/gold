
#include "object.hpp"
#include <stdlib.h>
#include <string.h>

struct redObject_t {
	uint64_t size;
	char** keys;
	void** data;
	redTypes* type;
	redObject* parent;
};

struct redArray_t {
	uint64_t size;
	void** data;
	redTypes* type;
};

typedef struct redChainLink_t redChainLink;
struct redChainLink_t {
	redChainLink* last;
	void* data;
	redTypes type;
	redChainLink* next;
};

struct {
	AllocFunction alloc;
	ReallocFunction realloc;
	FreeFunction free;
	uint64_t dataSize;
	redChainLink* dataStart;
	redChainLink* dataEnd;
} redInstance;

bool isCopy(redTypes type) {
	if (type == redTypeString)
		return true;
	return false;
}

void redFreeRedChainLink(redChainLink* link) {
	if (link != NULL) {
		if (link->last != NULL)
			link->last->next = link->next;
		if (link->next != NULL)
			link->next->last = link->last;
		if (link->data != NULL) {
			switch (link->type) {
				case redTypeObject:
					redDestroyObject((redObject*)link->data);
					break;
				case redTypeArray:
					redDestroyArray((redArray*)link->data);
					break;
				default:
					redInstance.free(link->data);
					break;
			}
		}
		redInstance.free(link);
	}
}

void redSetMemhook(AllocFunction a, ReallocFunction r, FreeFunction f) {
	redInstance.alloc = a;
	redInstance.realloc = r;
	redInstance.free = f;
}

void redInit() {
	if (redInstance.alloc == NULL)
		redInstance.alloc = malloc;
	if (redInstance.realloc == NULL)
		redInstance.realloc = realloc;
	if (redInstance.free == NULL)
		redInstance.free = free;
	redInstance.dataSize = 0;
	redInstance.dataEnd = NULL;
	redInstance.dataStart = NULL;
}

void redCleanUp() {
	// destroy from the end of the chain
	if (redInstance.dataStart != NULL) {
		uint64_t count = 0;
		redChainLink* current = redInstance.dataStart;
		while (current != NULL) {
			count += 1;
			redChainLink* next = current->next;
			redFreeRedChainLink(current);
			current = next;
		}
		if (count != redInstance.dataSize)
			printf("[Backend] Didn't clean up correctly!\n");
	}
}

void* redAlloc(uint64_t size, redTypes type) {
	if (size == 0)
		return NULL;
	redChainLink* link = NULL;
	redChainLink* linkEnd = redInstance.dataEnd;
	link = (redChainLink*)redInstance.alloc(sizeof(redChainLink));
	memset(link, 0, sizeof(redChainLink));
	link->data = redInstance.alloc(size);
	memset(link->data, 0, size);
	link->type = type;
	if (linkEnd) {
		link->last = linkEnd;
		linkEnd->next = link;
	}
	if (!redInstance.dataStart)
		redInstance.dataStart = link;
	redInstance.dataEnd = link;
	redInstance.dataSize += 1;
	return link->data;
}

void* redRealloc(void* ptr, uint64_t size, redTypes type) {
	if (size == 0)
		return NULL;
	if (ptr != NULL) {
		redChainLink* current = redInstance.dataEnd;
		while (current != NULL) {
			if (current->data == ptr) {
				current->data = redInstance.realloc(ptr, size);
				current->type = type;
				return current->data;
			}
			current = current->last;
		}
	}
	return redAlloc(size, type);
}

void redFree(void* ptr) {
	redChainLink* current = redInstance.dataEnd;
	while (current != NULL) {
		if (current->data == ptr) {
			if (current->last != NULL)
				current->last->next = current->next;
			if (current->next != NULL)
				current->next->last = current->last;
			redInstance.free(current->data);
			redInstance.free(current);
			redInstance.dataSize -= 1;
			return;
		}
		current = current->last;
	}
}

redArray* redNewArray() {
	redArray* array = (redArray*)redAlloc(sizeof(redArray), redTypeArray);
	memset(array, 0, sizeof(redArray));
	return array;
}

redObject* redNewObject(redObject* parent) {
	redObject* obj = (redObject*)redAlloc(sizeof(redObject), redTypeObject);
	memset(obj, 0, sizeof(redObject));
	obj->parent = parent;
	return obj;
}

void redDestroyArray(redArray* ptr) {
	for (uint64_t i = 0; i < ptr->size; ++i) {
		void* data = ptr->data[i];
		switch (ptr->type[i]) {
			case redTypeArray:
				redDestroyArray((redArray*)data);
				break;
			case redTypeObject:
				redDestroyObject((redObject*)data);
				break;
			default:
				redFree(data);
				break;
		}
	}
	redFree(ptr);
}

void redDestroyObject(redObject* ptr) {
	if (ptr != NULL && ptr->data != NULL) {
		for (uint64_t i = 0; i < ptr->size; ++i) {
			void* data = ptr->data[i];
			switch (ptr->type[i]) {
				case redTypeArray:
					redDestroyArray((redArray*)data);
					break;
				case redTypeObject:
					redDestroyObject((redObject*)data);
					break;
				case redTypeString:
					redFree(data);
					break;
			}
			char* name = ptr->keys[i];
			redFree(name);
		}
		redFree(ptr);
	}
}

redTypes redGetObjectType(redObject* ptr, char* name) {
	if (ptr == NULL)
		return redTypeNull;
	if (name == NULL)
		return redTypeNull;
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		return ptr->type[index];
	}
	return redTypeNull;
}

bool redGetObjectKeyIndex(redObject* ptr, const char* name, uint64_t* index) {
	if (ptr == NULL)
		return false;
	if (index == NULL)
		return false;
	for (uint64_t i = 0; i < ptr->size; ++i) {
		char* key = ptr->keys[i];
		if (strcmp(key, name) == 0) {
			*index = i;
			return true;
		}
	}
	return false;
}

void* redObjectNewItem(redObject* ptr,
											 const char* name,
											 uint64_t size,
											 redTypes type) {
	uint64_t newSize = ptr->size + 1;
	ptr->keys =
			(char**)redRealloc(ptr->keys, sizeof(char*) * newSize, redTypeString);
	ptr->type =
			(redTypes*)redRealloc(ptr->type, sizeof(void*) * newSize, redTypeString);
	ptr->data =
			(void**)redRealloc(ptr->data, sizeof(void*) * newSize, redTypeString);
	uint64_t index = ptr->size;
	void* item = NULL;
	if (size > sizeof(void*))
		item = redAlloc(size, type);
	ptr->size = newSize;
	return item;
}

void redObjectFreeItem(redObject* ptr, uint64_t index) {
	if (ptr->size == 0)
		return;
	uint64_t newSize = ptr->size - 1;
	if (newSize == 0) {
		redFree(ptr->data);
		redFree(ptr->keys);
		redFree(ptr->type);
		ptr->size = 0;
	} else {
		void** newData = (void**)redAlloc(sizeof(void*) * newSize, redTypeString);
		if (sizeof(void*) * index >= 0)
			memcpy(newData, ptr->data, sizeof(void*) * index);
		if (sizeof(void*) * (newSize - index) >= 0)
			memcpy(newData + (index), ptr->data + (index + 1),
						 sizeof(void*) * (newSize - index));
		redFree(ptr->data);
		ptr->data = newData;

		redTypes* newType =
				(redTypes*)redAlloc(sizeof(redTypes) * newSize, redTypeString);
		if (sizeof(redTypes) * index >= 0)
			memcpy(newType, ptr->type, sizeof(redTypes) * index);
		if (sizeof(redTypes) * (newSize - index) >= 0)
			memcpy(newType + (index), ptr->type + (index + 1),
						 sizeof(redTypes) * (newSize - index));
		redFree(ptr->type);
		ptr->type = newType;

		char** newKey = (char**)redAlloc(sizeof(void*) * newSize, redTypeString);
		if (sizeof(void*) * index >= 0)
			memcpy(newKey, ptr->keys, sizeof(void*) * index);
		if (sizeof(void*) * (newSize - index) >= 0)
			memcpy(newKey + (index), ptr->keys + (index + 1),
						 sizeof(void*) * (newSize - index));
		redFree(ptr->keys);
		ptr->keys = newKey;
	}
}

void redSetObjectData(redObject* ptr,
											const char* name,
											void* dataPtr,
											uint64_t size,
											redTypes type) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		if (isCopy(ptr->type[index])) {
			void* old = ptr->data[index];
			redFree(old);
		}
		if (size > sizeof(void*)) {
			void* data = redAlloc(size, type);
			memcpy(data, dataPtr, size);
			ptr->data[index] = data;
		} else {
			memcpy(ptr->data + index, dataPtr, size);
		}
	} else {
		index = ptr->size;
		void* data = redObjectNewItem(ptr, name, size, type);
		ptr->data[index] = data;
		ptr->keys[index] = (char*)name;
		if (isCopy(type)) {
			memcpy(ptr->data[index], dataPtr, size);
		} else {
			memcpy(ptr->data + index, dataPtr, size);
		}
	}
	ptr->type[index] = type;
}

void redSetObjectString(redObject* ptr, const char* name, char* value) {
	redSetObjectData(ptr, name, value, strlen(value) + 1, redTypeString);
}

void redSetObjectInt64(redObject* ptr, const char* name, int64_t value) {
	redSetObjectData(ptr, name, &value, sizeof(int64_t), redTypeInt64);
}

void redSetObjectInt32(redObject* ptr, const char* name, int32_t value) {
	redSetObjectData(ptr, name, &value, sizeof(int32_t), redTypeInt32);
}

void redSetObjectInt16(redObject* ptr, const char* name, int16_t value) {
	redSetObjectData(ptr, name, &value, sizeof(int16_t), redTypeInt16);
}

void redSetObjectInt8(redObject* ptr, const char* name, int8_t value) {
	redSetObjectData(ptr, name, &value, sizeof(int8_t), redTypeInt8);
}

void redSetObjectUInt64(redObject* ptr, const char* name, uint64_t value) {
	redSetObjectData(ptr, name, &value, sizeof(int64_t), redTypeUInt64);
}

void redSetObjectUInt32(redObject* ptr, const char* name, uint32_t value) {
	redSetObjectData(ptr, name, &value, sizeof(uint32_t), redTypeUInt32);
}

void redSetObjectUInt16(redObject* ptr, const char* name, uint16_t value) {
	redSetObjectData(ptr, name, &value, sizeof(uint16_t), redTypeUInt16);
}

void redSetObjectUInt8(redObject* ptr, const char* name, uint8_t value) {
	redSetObjectData(ptr, name, &value, sizeof(uint8_t), redTypeUInt8);
}

void redSetObjectDouble(redObject* ptr, const char* name, double value) {
	redSetObjectData(ptr, name, &value, sizeof(double), redTypeDouble);
}

void redSetObjectFloat(redObject* ptr, const char* name, float value) {
	redSetObjectData(ptr, name, &value, sizeof(float), redTypeFloat);
}

void redSetObjectBool(redObject* ptr, const char* name, bool value) {
	redSetObjectData(ptr, name, &value, sizeof(bool), redTypeBool);
}

bool redGetObjectData(redObject* ptr, const char* name, void* target, uint64_t size) {
	uint64_t index = 0;
	if (size < sizeof(void*)) {
		if (redGetObjectKeyIndex(ptr, name, &index)) {
			memcpy(target, ptr->data + index, size);
			return true;
		}
		redObject* parent = ptr->parent;
		while (parent != NULL) {
			if (redGetObjectKeyIndex(parent, name, &index)) {
				memcpy(target, parent->data + index, size);
				return true;
			}
			parent = parent->parent;
		}
	} else {
		if (redGetObjectKeyIndex(ptr, name, &index)) {
			target = ptr->data[index];
			return true;
		}
		redObject* parent = ptr->parent;
		while (parent != NULL) {
			if (redGetObjectKeyIndex(parent, name, &index)) {
				target = parent->data[index];
				return true;
			}
			parent = parent->parent;
		}
	}
	return false;
}

char* redGetObjectString(redObject* ptr, const char* name, char* def) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		return (char*)ptr->data[index];
	}
	redObject* parent = ptr->parent;
	while (parent != NULL) {
		if (redGetObjectKeyIndex(parent, name, &index))
			return (char*)parent->data[index];
		parent = parent->parent;
	}
	return def;
}

int64_t redGetObjectInt64(redObject* ptr, const char* name, int64_t def) {
	int64_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(int64_t))) {
		return response;
	}
	return def;
}

int32_t redGetObjectInt32(redObject* ptr, const char* name, int32_t def) {
	int32_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(int32_t))) {
		return response;
	}
	return def;
}

int16_t redGetObjectInt16(redObject* ptr, const char* name, int16_t def) {
	int16_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(int16_t))) {
		return response;
	}
	return def;
}

int8_t redGetObjectInt8(redObject* ptr, const char* name, int8_t def) {
	int8_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(int8_t))) {
		return response;
	}
	return def;
}

uint64_t redGetObjectUInt64(redObject* ptr, const char* name, uint64_t def) {
	uint64_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(uint64_t))) {
		return response;
	}
	return def;
}

uint32_t redGetObjectUInt32(redObject* ptr, const char* name, uint32_t def) {
	uint32_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(uint32_t))) {
		return response;
	}
	return def;
}

uint16_t redGetObjectUInt16(redObject* ptr, const char* name, uint16_t def) {
	uint16_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(uint16_t))) {
		return response;
	}
	return def;
}

uint8_t redGetObjectUInt8(redObject* ptr, const char* name, uint8_t def) {
	uint8_t response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(uint8_t))) {
		return response;
	}
	return def;
}

double redGetObjectDouble(redObject* ptr, const char* name, double def) {
	double response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(double))) {
		return response;
	}
	return def;
}

float redGetObjectFloat(redObject* ptr, const char* name, float def) {
	float response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(float))) {
		return response;
	}
	return def;
}

bool redGetObjectBool(redObject* ptr, const char* name, bool def) {
	bool response = 0;
	if (redGetObjectData(ptr, name, &response, sizeof(bool))) {
		return response;
	}
	return def;
}

uint64_t redGetArraySize(redArray* ptr) {
	return ptr->size;
}

void redPopArray(redArray* ptr) {
	uint64_t newSize = ptr->size - 1;
	if (newSize > 0) {
		ptr->data = (void**)redInstance.realloc(ptr->data, sizeof(void*) * newSize);
		ptr->type =
				(redTypes*)redInstance.realloc(ptr->type, sizeof(redTypes) * newSize);
		ptr->size = newSize;
	}
}

redTypes redGetArrayType(redArray* ptr, uint64_t index) {
	return ptr->type[index];
}

char* redGetJSONArray(redArray* ptr, uint64_t* size) {
	bson_t doc;
	bson_init(&doc);
	redGetBSONArray(ptr, &doc);
	size_t jsonSize = 0;
	char* json = bson_array_as_json(&doc, &jsonSize);
	void* dataPtr = redAlloc(jsonSize + 1, redTypeString);
	memset(dataPtr, 0, jsonSize + 1);
	memcpy(dataPtr, json, jsonSize);
	bson_free(json);
	bson_destroy(&doc);
	return (char*)dataPtr;
}

void redGetBSONArray(redArray* ptr, bson_t* doc) {
	for (uint32_t i = 0; i < ptr->size; ++i) {
		char iStr[16];
		const char* iStrPtr;
		size_t iSize = bson_uint32_to_string(i, &iStrPtr, iStr, 16);
		void* dataPtr = ptr->data[i];
		bson_t child;
		switch (ptr->type[i]) {
			case redTypeArray:
				bson_append_array_begin(doc, iStrPtr, iSize, &child);
				redGetBSONArray((redArray*)dataPtr, &child);
				bson_append_array_end(doc, &child);
				break;
			case redTypeObject:
				bson_append_document_begin(doc, iStrPtr, iSize, &child);
				redGetBSONObject((redObject*)dataPtr, &child);
				bson_append_document_end(doc, &child);
				break;
			case redTypeBool:
				bson_append_bool(doc, iStrPtr, iSize, *(bool*)dataPtr);
				break;
			case redTypeDouble:
				bson_append_double(doc, iStrPtr, iSize, *(double*)dataPtr);
				break;
			case redTypeFloat:
				bson_append_double(doc, iStrPtr, iSize, (double)*(float*)dataPtr);
				break;
			case redTypeInt64:
				bson_append_int64(doc, iStrPtr, iSize, *(int64_t*)dataPtr);
				break;
			case redTypeInt32:
				bson_append_int32(doc, iStrPtr, iSize, *(int32_t*)dataPtr);
				break;
			case redTypeInt16:
				bson_append_int32(doc, iStrPtr, iSize, (int32_t) * (int16_t*)dataPtr);
				break;
			case redTypeInt8:
				bson_append_int32(doc, iStrPtr, iSize, (int32_t) * (int8_t*)dataPtr);
				break;
			case redTypeUInt64:
				bson_append_double(doc, iStrPtr, iSize, (double)*(uint64_t*)dataPtr);
				break;
			case redTypeUInt32:
				bson_append_int64(doc, iStrPtr, iSize, (int64_t) * (uint32_t*)dataPtr);
				break;
			case redTypeUInt16:
				bson_append_int32(doc, iStrPtr, iSize, (int32_t) * (uint16_t*)dataPtr);
				break;
			case redTypeUInt8:
				bson_append_int32(doc, iStrPtr, iSize, (int32_t) * (uint8_t*)dataPtr);
				break;
			case redTypeString:
				bson_append_utf8(doc, iStrPtr, iSize, (char*)dataPtr, -1);
				break;
			case redTypeNull:
				bson_append_null(doc, iStrPtr, iSize);
				break;
			default:
				break;
		}
	}
}

char* redGetJSONObject(redObject* ptr, uint64_t* size) {
	bson_t doc;
	bson_init(&doc);
	redGetBSONObject(ptr, &doc);
	size_t jsonSize = 0;
	char* json = bson_as_relaxed_extended_json(&doc, &jsonSize);
	void* dataPtr = redAlloc(jsonSize + 1, redTypeString);
	memset(dataPtr, 0, jsonSize + 1);
	memcpy(dataPtr, json, jsonSize);
	bson_free(json);
	bson_destroy(&doc);
	return (char*)dataPtr;
}

void redGetBSONObject(redObject* ptr, bson_t* doc) {
	for (uint32_t i = 0; i < ptr->size; ++i) {
		char* key = ptr->keys[i];
		void* dataPtr = ptr->data[i];
		bson_t child;
		switch (ptr->type[i]) {
			case redTypeArray:
				bson_append_array_begin(doc, key, -1, &child);
				redGetBSONArray((redArray*)dataPtr, &child);
				bson_append_array_end(doc, &child);
				break;
			case redTypeObject:
				bson_append_document_begin(doc, key, -1, &child);
				redGetBSONObject((redObject*)dataPtr, &child);
				bson_append_document_end(doc, &child);
				break;
			case redTypeBool:
				bson_append_bool(doc, key, -1, *(bool*)dataPtr);
				break;
			case redTypeDouble:
				bson_append_double(doc, key, -1, *(double*)dataPtr);
				break;
			case redTypeFloat:
				bson_append_double(doc, key, -1, (double)*(float*)dataPtr);
				break;
			case redTypeInt64:
				bson_append_int64(doc, key, -1, *(int64_t*)dataPtr);
				break;
			case redTypeInt32:
				bson_append_int32(doc, key, -1, *(int32_t*)dataPtr);
				break;
			case redTypeInt16:
				bson_append_int32(doc, key, -1, (int32_t) * (int16_t*)dataPtr);
				break;
			case redTypeInt8:
				bson_append_int32(doc, key, -1, (int32_t) * (int8_t*)dataPtr);
				break;
			case redTypeUInt64:
				bson_append_double(doc, key, -1, (double)*(uint64_t*)dataPtr);
				break;
			case redTypeUInt32:
				bson_append_int64(doc, key, -1, (int64_t) * (uint32_t*)dataPtr);
				break;
			case redTypeUInt16:
				bson_append_int32(doc, key, -1, (int32_t) * (uint16_t*)dataPtr);
				break;
			case redTypeUInt8:
				bson_append_int32(doc, key, -1, (int32_t) * (uint8_t*)dataPtr);
				break;
			case redTypeString:
				bson_append_utf8(doc, key, -1, (char*)dataPtr, -1);
				break;
			case redTypeNull:
				bson_append_null(doc, key, -1);
				break;
			default:
				break;
		}
	}
}

void redPushArrayData(redArray* ptr,
											void* dataPtr,
											uint64_t size,
											redTypes type) {
	uint64_t i = ptr->size;
	ptr->type =
			(redTypes*)redRealloc(ptr->type, sizeof(void*) * (i + 1), redTypeString);
	ptr->data =
			(void**)redRealloc(ptr->data, sizeof(void*) * (i + 1), redTypeString);
	void* data = redAlloc(size, type);
	memcpy(data, dataPtr, size);
	ptr->data[i] = data;
	ptr->type[i] = type;
	ptr->size += 1;
}

void redPushArrayString(redArray* ptr, char* value) {
	redPushArrayData(ptr, value, strlen(value) + 1, redTypeString);
}

void redPushArrayInt64(redArray* ptr, int64_t value) {
	redPushArrayData(ptr, &value, sizeof(int64_t), redTypeInt64);
}

void redPushArrayInt32(redArray* ptr, int32_t value) {
	redPushArrayData(ptr, &value, sizeof(int32_t), redTypeInt32);
}

void redPushArrayInt16(redArray* ptr, int16_t value) {
	redPushArrayData(ptr, &value, sizeof(int16_t), redTypeInt16);
}

void redPushArrayInt8(redArray* ptr, int8_t value) {
	redPushArrayData(ptr, &value, sizeof(int8_t), redTypeInt8);
}

void redPushArrayUInt64(redArray* ptr, uint64_t value) {
	redPushArrayData(ptr, &value, sizeof(uint64_t), redTypeUInt64);
}

void redPushArrayUInt32(redArray* ptr, uint32_t value) {
	redPushArrayData(ptr, &value, sizeof(uint32_t), redTypeUInt32);
}

void redPushArrayUInt16(redArray* ptr, uint16_t value) {
	redPushArrayData(ptr, &value, sizeof(uint16_t), redTypeUInt16);
}

void redPushArrayUInt8(redArray* ptr, uint8_t value) {
	redPushArrayData(ptr, &value, sizeof(uint8_t), redTypeUInt8);
}

void redPushArrayDouble(redArray* ptr, double value) {
	redPushArrayData(ptr, &value, sizeof(double), redTypeDouble);
}

void redPushArrayFloat(redArray* ptr, float value) {
	redPushArrayData(ptr, &value, sizeof(float), redTypeFloat);
}

void redPushArrayBool(redArray* ptr, bool value) {
	redPushArrayData(ptr, &value, sizeof(bool), redTypeBool);
}

void redPushArrayArray(redArray* ptr, redArray* value) {
	uint64_t i = ptr->size;
	ptr->type =
			(redTypes*)redRealloc(ptr->type, sizeof(void*) * (i + 1), redTypeString);
	ptr->data =
			(void**)redRealloc(ptr->data, sizeof(void*) * (i + 1), redTypeString);
	ptr->data[i] = value;
	ptr->type[i] = redTypeArray;
	ptr->size += 1;
}

void redPushArrayObject(redArray* ptr, redObject* value) {
	uint64_t i = ptr->size;
	ptr->type =
			(redTypes*)redRealloc(ptr->type, sizeof(void*) * (i + 1), redTypeString);
	ptr->data =
			(void**)redRealloc(ptr->data, sizeof(void*) * (i + 1), redTypeString);
	ptr->data[i] = value;
	ptr->type[i] = redTypeObject;
	ptr->size += 1;
}

void redSetArrayString(redArray* ptr, uint64_t index, char* value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = strlen(value);
	void* data = redAlloc(size + 1, redTypeString);
	memset(data, 0, size + 1);
	memcpy(data, value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeString;
}

void redSetArrayInt64(redArray* ptr, uint64_t index, int64_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(int64_t);
	void* data = redAlloc(size, redTypeInt64);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeInt64;
}

void redSetArrayInt32(redArray* ptr, uint64_t index, int32_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(int32_t);
	void* data = redAlloc(size, redTypeInt32);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeInt32;
}

void redSetArrayInt16(redArray* ptr, uint64_t index, int16_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(int16_t);
	void* data = redAlloc(size, redTypeInt16);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeInt16;
}

void redSetArrayInt8(redArray* ptr, uint64_t index, int8_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(int8_t);
	void* data = redAlloc(size, redTypeInt8);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeInt8;
}

void redSetArrayUInt64(redArray* ptr, uint64_t index, uint64_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(uint64_t);
	void* data = redAlloc(size, redTypeUInt64);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeUInt64;
}

void redSetArrayUInt32(redArray* ptr, uint64_t index, uint32_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(uint32_t);
	void* data = redAlloc(size, redTypeUInt32);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeUInt32;
}

void redSetArrayUInt16(redArray* ptr, uint64_t index, uint16_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(uint16_t);
	void* data = redAlloc(size, redTypeUInt16);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeUInt16;
}

void redSetArrayUInt8(redArray* ptr, uint64_t index, uint8_t value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(uint8_t);
	void* data = redAlloc(size, redTypeUInt8);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeUInt8;
}

void redSetArrayDouble(redArray* ptr, uint64_t index, double value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(double);
	void* data = redAlloc(size, redTypeDouble);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeDouble;
}

void redSetArrayFloat(redArray* ptr, uint64_t index, float value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(float);
	void* data = redAlloc(size, redTypeFloat);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeFloat;
}

void redSetArrayBool(redArray* ptr, uint64_t index, bool value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	uint64_t size = sizeof(bool);
	void* data = redAlloc(size, redTypeBool);
	memcpy(data, &value, size);
	ptr->data[index] = data;
	ptr->type[index] = redTypeBool;
}

void redSetArrayArray(redArray* ptr, uint64_t index, redArray* value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	ptr->data[index] = value;
	ptr->type[index] = redTypeArray;
}

void redSetArrayObject(redArray* ptr, uint64_t index, redObject* value) {
	if (index >= ptr->size) {
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * (index + 1),
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * (index + 1),
																	 redTypeString);
		ptr->size = index + 1;
	}
	ptr->data[index] = value;
	ptr->type[index] = redTypeObject;
}

char* redGetArrayString(redArray* ptr, uint64_t index, char* def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeString)
		return (char*)ptr->data[index];
	return def;
}

int64_t redGetArrayInt64(redArray* ptr, uint64_t index, int64_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeInt64)
		return *(int64_t*)ptr->data[index];
	return def;
}

int32_t redGetArrayInt32(redArray* ptr, uint64_t index, int32_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeInt32)
		return *(int32_t*)ptr->data[index];
	return def;
}

int16_t redGetArrayInt16(redArray* ptr, uint64_t index, int16_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeInt16)
		return *(int16_t*)ptr->data[index];
	return def;
}

int8_t redGetArrayInt8(redArray* ptr, uint64_t index, int8_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeInt8)
		return *(int8_t*)ptr->data[index];
	return def;
}

uint64_t redGetArrayUInt64(redArray* ptr, uint64_t index, uint64_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeUInt64)
		return *(uint64_t*)ptr->data[index];
	return def;
}

uint32_t redGetArrayUInt32(redArray* ptr, uint64_t index, uint32_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeUInt32)
		return *(uint32_t*)ptr->data[index];
	return def;
}

uint16_t redGetArrayUInt16(redArray* ptr, uint64_t index, uint16_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeUInt16)
		return *(uint16_t*)ptr->data[index];
	return def;
}

uint8_t redGetArrayUInt8(redArray* ptr, uint64_t index, uint8_t def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeUInt8)
		return *(uint8_t*)ptr->data[index];
	return def;
}

double redGetArrayDouble(redArray* ptr, uint64_t index, double def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeDouble)
		return *(double*)ptr->data[index];
	return def;
}

float redGetArrayFloat(redArray* ptr, uint64_t index, float def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeFloat)
		return *(float*)ptr->data[index];
	return def;
}

bool redGetArrayBool(redArray* ptr, uint64_t index, bool def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeBool)
		return *(bool*)ptr->data[index];
	return def;
}

redArray* redGetArrayArray(redArray* ptr, uint64_t index, redArray* def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeArray)
		return (redArray*)ptr->data[index];
	return def;
}

redObject* redGetArrayObject(redArray* ptr, uint64_t index, redObject* def) {
	if (index >= ptr->size)
		return def;
	if (ptr->type[index] == redTypeObject)
		return (redObject*)ptr->data[index];
	return def;
}

redMethod redGetObjectMethod(redObject* ptr, const char* name, redMethod def) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		return (redMethod)ptr->data[index];
	}
	redObject* parent = ptr->parent;
	while (parent != NULL) {
		if (redGetObjectKeyIndex(parent, name, &index)) {
			return (redMethod)parent->data[index];
		}
		parent = parent->parent;
	}
	return def;
}

void redSetObjectArray(redObject* ptr, const char* name, redArray* value) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		redTypes type = ptr->type[index];
		if (isCopy(type)) {
			void* old = ptr->data[index];
			redFree(old);
		}
		ptr->data[index] = value;
	} else {
		index = ptr->size;
		ptr->keys = (char**)redRealloc(ptr->keys, sizeof(char*) * ptr->size + 1,
																	 redTypeString);
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * ptr->size + 1,
																			redTypeString);
		ptr->data = (void**)redRealloc(ptr->data, sizeof(void*) * ptr->size + 1,
																	 redTypeString);
		ptr->data[index] = value;
		ptr->keys[index] = (char*)name;
		ptr->size += 1;
	}
	ptr->type[index] = redTypeArray;
}

void redSetObjectObject(redObject* ptr, const char* name, redObject* value) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		redTypes type = ptr->type[index];
		if (isCopy(type)) {
			void* old = ptr->data[index];
			redFree(old);
		}
		ptr->data[index] = value;
	} else {
		index = ptr->size;
		ptr->keys =
				(char**)redRealloc(ptr->keys, sizeof(char*) * index + 1, redTypeString);
		ptr->type = (redTypes*)redRealloc(ptr->type, sizeof(void*) * index + 1,
																			redTypeString);
		ptr->data =
				(void**)redRealloc(ptr->data, sizeof(void*) * index + 1, redTypeString);
		ptr->data[index] = value;
		ptr->keys[index] = (char*)name;
		ptr->size += 1;
	}
	ptr->type[index] = redTypeObject;
}

void redSetObjectMethod(redObject* ptr, const char* name, redMethod value) {
	uint64_t index = 0;
	if (redGetObjectKeyIndex(ptr, name, &index)) {
		redTypes type = ptr->type[index];
		if (isCopy(type)) {
			void* old = ptr->data[index];
			redFree(old);
		}
		ptr->data[index] = (void*)value;
	} else {
		index = ptr->size;
		uint64_t nSize = index + 1;
		ptr->keys =
				(char**)redRealloc(ptr->keys, sizeof(char*) * nSize, redTypeString);
		ptr->type =
				(redTypes*)redRealloc(ptr->type, sizeof(void*) * nSize, redTypeString);
		ptr->data =
				(void**)redRealloc(ptr->data, sizeof(void*) * nSize, redTypeString);
		ptr->data[index] = (void*)value;
		ptr->keys[index] = (char*)name;
		ptr->size += 1;
	}
	ptr->type[index] = redTypeMethod;
}

redArray* redCallMethod(redObject* ptr, const char* name, redArray* args) {
	redMethod method = redGetObjectMethod(ptr, name, 0);
	if (method != NULL) {
		redArray* response = (*method)(ptr, args);
		return response;
	}
	return NULL;
}
