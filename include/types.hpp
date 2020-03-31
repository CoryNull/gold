#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <nlohmann/json.hpp>
#include <vector>

namespace red {
	using namespace std;
	/* <Types> */
	class object;
	class array;
	class var;
	typedef var (*method)(object&, var&);
	typedef vector<uint8_t> binary;

	typedef enum types_t {
		typeNull = 0,
		typeArray,
		typeObject,
		typeMethod,
		typePtr,
		typeString,
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

	bool isCopyable(types type);
	const char* getTypeString(types type);
	/* </Types> */
}  // namespace red
