#pragma once

/* <Includes> */
#include <inttypes.h>
#include <stdbool.h>
#include <libbson-1.0/bson.h>
/* </Includes> */

/* <Types> */
typedef struct redArray_t redArray;
typedef struct redObject_t redObject;
typedef struct redVar_t redVar;
typedef redArray* (*redMethod)(redObject*, redArray*);

typedef enum redTypes_t {
	redTypeNull = 0,
	redTypeArray,
	redTypeObject,
	redTypeMethod,
	redTypeString,
	redTypeInt64,
	redTypeInt32,
	redTypeInt16,
	redTypeInt8,
	redTypeUInt64,
	redTypeUInt32,
	redTypeUInt16,
	redTypeUInt8,
	redTypeDouble,
	redTypeFloat,
	redTypeBool
} redTypes;
/* </Types> */

/* <Memory> */

typedef void* (*AllocFunction)(uint64_t size);
typedef void* (*ReallocFunction)(void* src, uint64_t size);
typedef void (*FreeFunction)(void* ptr);

void redSetMemhook(AllocFunction, ReallocFunction, FreeFunction);
void redInit();
void redCleanUp();

void* redAlloc(uint64_t size, redTypes type);
void* redRealloc(void* ptr, uint64_t size, redTypes type);
void redFree(void* ptr);

/* </Memory> */

/* <Array> */
redArray* redNewArray();
void redDestroyArray(redArray*);

uint64_t redGetArraySize(redArray* ptr);
void redPopArray(redArray* ptr);
redTypes redGetArrayType(redArray* ptr, uint64_t index);
char* redGetJSONArray(redArray* ptr, uint64_t* size);
void redGetBSONArray(redArray* ptr, bson_t* doc);

void redPushArrayString(redArray* ptr, char* value);
void redPushArrayInt64(redArray* ptr, int64_t value);
void redPushArrayInt32(redArray* ptr, int32_t value);
void redPushArrayInt16(redArray* ptr, int16_t value);
void redPushArrayInt8(redArray* ptr, int8_t value);
void redPushArrayUInt64(redArray* ptr, uint64_t value);
void redPushArrayUInt32(redArray* ptr, uint32_t value);
void redPushArrayUInt16(redArray* ptr, uint16_t value);
void redPushArrayUInt8(redArray* ptr, uint8_t value);
void redPushArrayDouble(redArray* ptr, double value);
void redPushArrayFloat(redArray* ptr, float value);
void redPushArrayBool(redArray* ptr, bool value);
void redPushArrayArray(redArray* ptr, redArray* value);
void redPushArrayObject(redArray* ptr, redObject* value);

void redSetArrayString(redArray* ptr, uint64_t index, char* value);
void redSetArrayInt64(redArray* ptr, uint64_t index, int64_t value);
void redSetArrayInt32(redArray* ptr, uint64_t index, int32_t value);
void redSetArrayInt16(redArray* ptr, uint64_t index, int16_t value);
void redSetArrayInt8(redArray* ptr, uint64_t index, int8_t value);
void redSetArrayUInt64(redArray* ptr, uint64_t index, uint64_t value);
void redSetArrayUInt32(redArray* ptr, uint64_t index, uint32_t value);
void redSetArrayUInt16(redArray* ptr, uint64_t index, uint16_t value);
void redSetArrayUInt8(redArray* ptr, uint64_t index, uint8_t value);
void redSetArrayDouble(redArray* ptr, uint64_t index, double value);
void redSetArrayFloat(redArray* ptr, uint64_t index, float value);
void redSetArrayBool(redArray* ptr, uint64_t index, bool value);
void redSetArrayArray(redArray* ptr, uint64_t index, redArray* value);
void redSetArrayObject(redArray* ptr, uint64_t index, redObject* value);

char* redGetArrayString(redArray* ptr, uint64_t index, char* def);
int64_t redGetArrayInt64(redArray* ptr, uint64_t index, int64_t def);
int32_t redGetArrayInt32(redArray* ptr, uint64_t index, int32_t def);
int16_t redGetArrayInt16(redArray* ptr, uint64_t index, int16_t def);
int8_t redGetArrayInt8(redArray* ptr, uint64_t index, int8_t def);
uint64_t redGetArrayUInt64(redArray* ptr, uint64_t index, uint64_t def);
uint32_t redGetArrayUInt32(redArray* ptr, uint64_t index, uint32_t def);
uint16_t redGetArrayUInt16(redArray* ptr, uint64_t index, uint16_t def);
uint8_t redGetArrayUInt8(redArray* ptr, uint64_t index, uint8_t def);
double redGetArrayDouble(redArray* ptr, uint64_t index, double def);
float redGetArrayFloat(redArray* ptr, uint64_t index, float def);
bool redGetArrayBool(redArray* ptr, uint64_t index, bool def);
redArray* redGetArrayArray(redArray* ptr, uint64_t index, redArray* def);
redObject* redGetArrayObject(redArray* ptr, uint64_t index, redObject* def);

/* </Array> */

/* <Object> */

redObject* redNewObject(redObject* parent);
void redDestroyObject(redObject*);

redTypes redGetObjectType(redObject* ptr, const char* name);
bool redGetObjectKeyIndex(redObject* ptr, const char* name, uint64_t* index);
char* redGetJSONObject(redObject* ptr, uint64_t* size);
void redGetBSONObject(redObject* ptr, bson_t* doc);
redArray* redCallMethod(redObject* ptr, const char* name, redArray* args);

void redSetObjectString(redObject* ptr, const char* name, char* value);
void redSetObjectInt64(redObject* ptr, const char* name, int64_t value);
void redSetObjectInt32(redObject* ptr, const char* name, int32_t value);
void redSetObjectInt16(redObject* ptr, const char* name, int16_t value);
void redSetObjectInt8(redObject* ptr, const char* name, int8_t value);
void redSetObjectUInt64(redObject* ptr, const char* name, uint64_t value);
void redSetObjectUInt32(redObject* ptr, const char* name, uint32_t value);
void redSetObjectUInt16(redObject* ptr, const char* name, uint16_t value);
void redSetObjectUInt8(redObject* ptr, const char* name, uint8_t value);
void redSetObjectDouble(redObject* ptr, const char* name, double value);
void redSetObjectFloat(redObject* ptr, const char* name, float value);
void redSetObjectBool(redObject* ptr, const char* name, bool value);
void redSetObjectArray(redObject* ptr, const char* name, redArray* value);
void redSetObjectObject(redObject* ptr, const char* name, redObject* value);
void redSetObjectMethod(redObject* ptr, const char* name, redMethod value);

char* redGetObjectString(redObject* ptr, const char* name, char* def);
int64_t redGetObjectInt64(redObject* ptr, const char* name, int64_t def);
int32_t redGetObjectInt32(redObject* ptr, const char* name, int32_t def);
int16_t redGetObjectInt16(redObject* ptr, const char* name, int16_t def);
int8_t redGetObjectInt8(redObject* ptr, const char* name, int8_t def);
uint64_t redGetObjectUInt64(redObject* ptr, const char* name, uint64_t def);
uint32_t redGetObjectUInt32(redObject* ptr, const char* name, uint32_t def);
uint16_t redGetObjectUInt16(redObject* ptr, const char* name, uint16_t def);
uint8_t redGetObjectUInt8(redObject* ptr, const char* name, uint8_t def);
double redGetObjectDouble(redObject* ptr, const char* name, double def);
float redGetObjectFloat(redObject* ptr, const char* name, float def);
bool redGetObjectBool(redObject* ptr, const char* name, bool def);
redArray* redGetObjectArray(redObject* ptr, const char* name, redArray* def);
redObject* redGetObjectObject(redObject* ptr, const char* name, redObject* def);
redMethod redGetObjectMethod(redObject* ptr, const char* name, redMethod def);

/* </Object> */