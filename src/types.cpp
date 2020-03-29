#include "types.hpp"

#include "var.hpp"

namespace red {

	const var nullVar = var();

	bool isCopyable(types type) {
		if (type == typeString)
			return true;
		if (sizeof(uint64_t) > sizeof(void*) && type == typeUInt64)
			return true;
		if (sizeof(int64_t) > sizeof(void*) && type == typeInt64)
			return true;
		if (sizeof(double) > sizeof(void*) && type == typeDouble)
			return true;
		return false;
	}

	const char* getTypeString(types type) {
		switch (type) {
			case typeArray:
				return "Array";
			case typeObject:
				return "Object";
			case typeMethod:
				return "Method";
			case typePtr:
				return "Pointer";
			case typeString:
				return "String";
			case typeInt64:
				return "Int64";
			case typeInt32:
				return "Int32";
			case typeInt16:
				return "Int16";
			case typeInt8:
				return "Int8";
			case typeUInt64:
				return "UInt64";
			case typeUInt32:
				return "UInt32";
			case typeUInt16:
				return "UInt16";
			case typeUInt8:
				return "UInt8";
			case typeDouble:
				return "Double";
			case typeFloat:
				return "Float";
			case typeBool:
				return "Bool";
			case typeNull:
				return "Null";
			case typeException:
				return "Exception";
		}
	}
}  // namespace red