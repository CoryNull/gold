#pragma once

#include <inttypes.h>
#include <stdbool.h>

namespace red {
	/* <Types> */
	class object;
	class array;
	class var;
	typedef array* (*method)(object*, array*);

	typedef enum types_t {
		typeNull = 0,
		typeArray,
		typeObject,
		typeMethod,
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
		typeBool
	} types;

	bool isCopyable(types type);
	/* </Types> */
}  // namespace red
