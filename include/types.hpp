#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <experimental/source_location>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>

namespace gold {
	using namespace std;
	using sourceLocation = std::experimental::source_location;
	/* <Types> */
	class object;
	class array;
	class var;
	using varList = vector<var>;
	using method = var (object::*)(varList);
	using func = function<var(varList)>;
	using binary = vector<uint8_t>;

	class genericError : public exception {
	 public:
		const string msg;
		const sourceLocation loc;

		genericError(
			const exception& copy,
			const sourceLocation& l = sourceLocation::current());
		genericError(const genericError& copy);
		genericError(
			string_view message,
			const sourceLocation& l = sourceLocation::current());
		operator string() const;
		friend ostream& operator<<(ostream& os, const genericError& dt);
	};

	typedef enum types_t {
		typeNull = 0,
		typeArray,
		typeObject,
		typeMethod,
		typeFunction,
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
}  // namespace gold
