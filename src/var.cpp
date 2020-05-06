#include <string.h>

#include "types.hpp"

namespace gold {
	using namespace std;

	struct objData {
		object::omap items;
		gold::object parent;
		uint64_t id;
		mutex omutex;
	};

	var::varContainer::~varContainer() {
		if (!this) return;
		switch (type) {
			case typeException: {
				if (data) delete (genericError*)data;
				break;
			}
			case typeList: {
				if (data) delete (list*)data;
				break;
			}
			case typeObject: {
				if (data) delete (object*)data;
				break;
			}
			case typeString: {
				if (data) delete (string*)data;
				break;
			}
			case typeBinary: {
				if (data) delete (binary*)data;
				break;
			}
			case typeUInt64: {
				if (data) delete (uint64_t*)data;
				break;
			}
			case typeUInt32: {
				if (data) delete (uint32_t*)data;
				break;
			}
			case typeUInt16: {
				if (data) delete (uint16_t*)data;
				break;
			}
			case typeUInt8: {
				if (data) delete (uint8_t*)data;
				break;
			}
			case typeInt64: {
				if (data) delete (int64_t*)data;
				break;
			}
			case typeInt32: {
				if (data) delete (int32_t*)data;
				break;
			}
			case typeInt16: {
				if (data) delete (int16_t*)data;
				break;
			}
			case typeInt8: {
				if (data) delete (int8_t*)data;
				break;
			}
			case typeBool: {
				if (data) delete (bool*)data;
				break;
			}
			case typeDouble: {
				if (data) delete (double*)data;
				break;
			}
			case typeFloat: {
				if (data) delete (float*)data;
				break;
			}
			case typeMethod: {
				if (data) delete (method*)data;
				break;
			}
			case typeFunction: {
				if (data) delete (func*)data;
				break;
			}
			default:
				break;
		}
		data = nullptr;
		type = typeNull;
	}

	var::varContainer::varContainer() {
		data = nullptr;
		type = typeNull;
	}

	var::varContainer::varContainer(const varContainer& other) {
		type = other.type;
		switch (type) {
			case typeException: {
				auto o = (genericError*)other.data;
				data = new genericError(*o);
				break;
			}
			case typeList: {
				auto o = (list*)other.data;
				data = new list(*o);
				break;
			}
			case typeObject: {
				auto o = (object*)other.data;
				data = new object(*o);
				break;
			}
			case typeString: {
				auto o = (string*)other.data;
				data = new string(*o);
				break;
			}
			case typeBinary: {
				auto o = (binary*)other.data;
				data = new binary(*o);
				break;
			}
			case typeInt64: {
				auto o = (int64_t*)other.data;
				data = new int64_t(*o);
				break;
			}
			case typeInt32: {
				auto o = (int32_t*)other.data;
				data = new int32_t(*o);
				break;
			}
			case typeInt16: {
				auto o = (int16_t*)other.data;
				data = new int16_t(*o);
				break;
			}
			case typeInt8: {
				auto o = (int8_t*)other.data;
				data = new int8_t(*o);
				break;
			}
			case typeUInt64: {
				auto o = (uint64_t*)other.data;
				data = new uint64_t(*o);
				break;
			}
			case typeUInt32: {
				auto o = (uint32_t*)other.data;
				data = new uint32_t(*o);
				break;
			}
			case typeUInt16: {
				auto o = (uint16_t*)other.data;
				data = new uint16_t(*o);
				break;
			}
			case typeUInt8: {
				auto o = (uint8_t*)other.data;
				data = new uint8_t(*o);
				break;
			}
			case typeBool: {
				auto o = (bool*)other.data;
				data = new bool(*o);
				break;
			}
			case typeFunction: {
				auto o = (func*)other.data;
				data = new func(*o);
				break;
			}
			case typeMethod: {
				auto o = (method*)other.data;
				data = new method(*o);
				break;
			}
			case typeFloat: {
				auto o = (float*)other.data;
				data = new float(*o);
				break;
			}
			case typeDouble: {
				auto o = (double*)other.data;
				data = new double(*o);
				break;
			}
			case typePtr: {
				auto o = (void*)other.data;
				data = o;
				break;
			}
			default: {
				break;
			}
		}
	}

	var::varPtr var::autoNull =
		var::varPtr(nullptr, [](void*) {});

	var::var() : sPtr(autoNull) {}

	var::var(const var& copy) : sPtr(copy.sPtr) {}

	var::var(void* v, types t) {
		auto ptr = new varContainer();
		ptr->data = v;
		ptr->type = t;
		sPtr = varPtr(ptr);
	}

	var::var(const char* value) {
		auto ptr = new varContainer();
		ptr->data = new string(value);
		ptr->type = typeString;
		sPtr = varPtr(ptr);
	}

	var::var(string v) {
		auto ptr = new varContainer();
		ptr->data = new string(v);
		ptr->type = typeString;
		sPtr = varPtr(ptr);
	}

	var::var(const binary& bin) {
		auto ptr = new varContainer();
		ptr->data = new binary(bin);
		ptr->type = typeBinary;
		sPtr = varPtr(ptr);
	}

	var::var(int64_t v) {
		auto ptr = new varContainer();
		ptr->data = new int64_t(v);
		ptr->type = typeInt64;
		sPtr = varPtr(ptr);
	}

	var::var(int32_t v) {
		auto ptr = new varContainer();
		ptr->data = new int32_t(v);
		ptr->type = typeInt32;
		sPtr = varPtr(ptr);
	}

	var::var(int16_t v) {
		auto ptr = new varContainer();
		ptr->data = new int16_t(v);
		ptr->type = typeInt16;
		sPtr = varPtr(ptr);
	}

	var::var(int8_t v) {
		auto ptr = new varContainer();
		ptr->data = new int8_t(v);
		ptr->type = typeInt8;
		sPtr = varPtr(ptr);
	}

	var::var(uint64_t v) {
		auto ptr = new varContainer();
		ptr->data = new uint64_t(v);
		ptr->type = typeUInt64;
		sPtr = varPtr(ptr);
	}

	var::var(uint32_t v) {
		auto ptr = new varContainer();
		ptr->data = new uint32_t(v);
		ptr->type = typeUInt32;
		sPtr = varPtr(ptr);
	}

	var::var(uint16_t v) {
		auto ptr = new varContainer();
		ptr->data = new uint16_t(v);
		ptr->type = typeUInt16;
		sPtr = varPtr(ptr);
	}

	var::var(uint8_t v) {
		auto ptr = new varContainer();
		ptr->data = new uint8_t(v);
		ptr->type = typeUInt8;
		sPtr = varPtr(ptr);
	}

	var::var(double v) {
		auto ptr = new varContainer();
		ptr->data = new double(v);
		ptr->type = typeDouble;
		sPtr = varPtr(ptr);
	}

	var::var(float v) {
		auto ptr = new varContainer();
		ptr->data = new float(v);
		ptr->type = typeFloat;
		sPtr = varPtr(ptr);
	}

	var::var(const list& v) {
		auto ptr = new varContainer();
		ptr->data = new list((list&)v);
		ptr->type = typeList;
		sPtr = varPtr(ptr);
	}

	var::var(const obj& v) {
		auto ptr = new varContainer();
		ptr->data = new obj(v);
		ptr->type = typeObject;
		sPtr = varPtr(ptr);
	}

	var::var(method v) {
		auto ptr = new varContainer();
		ptr->data = new method(v);
		ptr->type = typeMethod;
		sPtr = varPtr(ptr);
	}

	var::var(func v) {
		auto ptr = new varContainer();
		ptr->data = new func(v);
		ptr->type = typeFunction;
		sPtr = varPtr(ptr);
	}

	var::var(bool v) {
		auto ptr = new varContainer();
		ptr->data = new bool(v);
		ptr->type = typeBool;
		sPtr = varPtr(ptr);
	}

	var::var(const genericError& v) {
		auto ptr = new varContainer();
		ptr->data = new genericError(v);
		ptr->type = typeException;
		sPtr = varPtr(ptr);
	}

	var::~var() { sPtr = nullptr; }

	var& var::operator=(const var& rhs) {
		auto rPtr = rhs.sPtr.get();
		sPtr = rPtr ? make_shared<varContainer>(*rPtr) : autoNull;
		return *this;
	}

	bool var::operator==(const var& rhs) const {
		auto rCon = rhs.sPtr.get();
		if (rCon) {
			switch (rCon->type) {
				case typeNull:
					return isEmpty();
				case typeObject: {
					auto o = *(object*)rCon->data;
					auto mob = getObject();
					return o.data == mob.data;
				}
				case typeList: {
					auto arr = *(list*)rCon->data;
					auto mar = getList();
					return arr.data == mar.data;
				}
				case typeString: {
					auto o = *(string*)rCon->data;
					return o == (string) * this;
				}
				case typeDouble: {
					auto o = *(double*)rCon->data;
					return o == (double)*this;
				}
				case typeFloat: {
					auto o = *(float*)rCon->data;
					return o == (float)*this;
				}
				case typeInt64: {
					auto o = *(int64_t*)rCon->data;
					return o == (int64_t) * this;
				}
				case typeInt32: {
					auto o = *(int32_t*)rCon->data;
					return o == (int32_t) * this;
				}
				case typeInt16: {
					auto o = *(int16_t*)rCon->data;
					return 0 == (int16_t) * this;
				}
				case typeInt8: {
					auto o = *(int8_t*)rCon->data;
					return o == (int8_t) * this;
				}
				case typeUInt64: {
					auto o = *(uint64_t*)rCon->data;
					return o == (uint64_t) * this;
				}
				case typeUInt32: {
					auto o = *(uint32_t*)rCon->data;
					return o == (uint32_t) * this;
				}
				case typeUInt16: {
					auto o = *(uint16_t*)rCon->data;
					return o == (uint16_t) * this;
				}
				case typeUInt8: {
					auto o = *(uint8_t*)rCon->data;
					return o == (uint8_t) * this;
				}
				case typeBool: {
					auto o = *(bool*)rCon->data;
					return o == (bool)*this;
				}
				default: {
					auto o = (void*)rCon->data;
					return o == (void*)*this;
				}
			}
		}
		return false;
	}

	bool var::operator!=(const var& rhs) const {
		return !operator==(rhs);
	}

	types var::getType() const {
		auto container = sPtr.get();
		if (container) return container->type;
		return typeNull;
	}

	const char* var::getTypeString() const {
		types type = typeNull;
		auto container = sPtr.get();
		if (container) type = container->type;
		return gold::getTypeString(type);
	}

	bool var::isString() const {
		auto container = sPtr.get();
		if (container && container->type == typeString) return true;
		return false;
	}

	bool var::isNumber() const {
		auto container = sPtr.get();
		if (
			container && (container->type == typeDouble ||
										container->type == typeFloat ||
										container->type == typeInt64 ||
										container->type == typeInt32 ||
										container->type == typeInt16 ||
										container->type == typeInt8 ||
										container->type == typeUInt64 ||
										container->type == typeUInt32 ||
										container->type == typeUInt16 ||
										container->type == typeUInt8 ||
										container->type == typePtr))
			return true;
		return false;
	}

	bool var::isFloating() const {
		auto container = sPtr.get();
		if (
			container && (container->type == typeDouble ||
										container->type == typeFloat))
			return true;
		return false;
	}

	bool var::isSigned() const {
		auto container = sPtr.get();
		if (
			container && (container->type == typeDouble ||
										container->type == typeFloat ||
										container->type == typeInt64 ||
										container->type == typeInt32 ||
										container->type == typeInt16 ||
										container->type == typeInt8))
			return true;
		return false;
	}

	bool var::isBool() const {
		auto container = sPtr.get();
		if (container && container->type == typeBool) return true;
		return false;
	}

	bool var::isObject() const {
		auto container = sPtr.get();
		if (container && container->type == typeObject) return true;
		return false;
	}

	bool var::isObject(obj& proto) const {
		auto container = sPtr.get();
		if (container && container->type == typeObject) {
			auto p = *(obj*)container->data;
			while (p) {
				if (p == proto) return true;
				p = p.data->parent;
			}
		}
		return false;
	}

	bool var::isList() const {
		auto container = sPtr.get();
		if (container && container->type == typeList) return true;
		return false;
	}

	bool var::isEmpty() const {
		auto container = sPtr.get();
		if (container) {
			if (container->type == typeList) {
				auto v = *(list*)container->data;
				return v.size() == 0;
			} else if (container->type == typeObject) {
				auto v = *(obj*)container->data;
				return v.size() == 0;
			} else if (container->type == typeString) {
				auto v = *(string*)container->data;
				return v.length() == 0;
			} else if (container->type == typeFunction) {
				auto v = *(func*)container->data;
				return v == 0;
			} else if (container->type == typeMethod) {
				auto v = *(method*)container->data;
				return v == 0;
			} else if (container->type == typeDouble) {
				auto v = *(double*)container->data;
				return v == 0.0;
			} else if (container->type == typeFloat) {
				auto v = *(float*)container->data;
				return v == 0.0;
			} else if (container->type == typeUInt64) {
				auto v = *(uint64_t*)container->data;
				return v == 0;
			} else if (container->type == typeUInt32) {
				auto v = *(uint32_t*)container->data;
				return v == 0;
			} else if (container->type == typeUInt16) {
				auto v = *(uint16_t*)container->data;
				return v == 0;
			} else if (container->type == typeUInt8) {
				auto v = *(uint8_t*)container->data;
				return v == 0;
			} else if (container->type == typeInt64) {
				auto v = *(int64_t*)container->data;
				return v == 0;
			} else if (container->type == typeInt32) {
				auto v = *(int32_t*)container->data;
				return v == 0;
			} else if (container->type == typeInt16) {
				auto v = *(int16_t*)container->data;
				return v == 0;
			} else if (container->type == typeInt8) {
				auto v = *(int8_t*)container->data;
				return v == 0;
			} else if (container->type == typeBool) {
				auto v = *(bool*)container->data;
				return v == false;
			} else if (container->type == typePtr) {
				auto v = (void*)container->data;
				return v == nullptr;
			} else
				return false;
		}
		return true;
	}

	bool var::isError() const {
		auto container = sPtr.get();
		return container && container->type == typeException;
	}

	bool var::isFunction() const {
		auto container = sPtr.get();
		return container && container->type == typeFunction;
	}

	bool var::isMethod() const {
		auto container = sPtr.get();
		return container && container->type == typeMethod;
	}

	bool var::isBinary() const {
		auto container = sPtr.get();
		return container && container->type == typeBinary;
	}

	string var::getString() const { return (string) * this; }

	int64_t var::getInt64() const { return (int64_t) * this; }

	int32_t var::getInt32() const { return (int32_t) * this; }

	int16_t var::getInt16() const { return (int16_t) * this; }

	int8_t var::getInt8() const { return (int8_t) * this; }

	uint64_t var::getUInt64() const { return (uint64_t) * this; }

	uint32_t var::getUInt32() const { return (uint32_t) * this; }

	uint16_t var::getUInt16() const { return (uint16_t) * this; }

	uint8_t var::getUInt8() const { return (uint8_t) * this; }

	double var::getDouble() const { return (double)*this; }

	float var::getFloat() const { return (float)*this; }

	bool var::getBool() const { return (bool)*this; }

	list var::getList() const {
		auto container = sPtr.get();
		if (container && container->type == typeList)
			return *(list*)container->data;
		return list();
	}

	object var::getObject() const {
		auto container = sPtr.get();
		if (container && container->type == typeObject)
			return *(object*)container->data;
		return object();
	}

	void var::returnList(list& result) const {
		auto container = sPtr.get();
		if (container && container->type == typeList)
			result = *(list*)container->data;
	}

	void var::returnObject(obj& result) const {
		auto container = sPtr.get();
		if (container && container->type == typeObject)
			result = *(object*)container->data;
	}

	method var::getMethod() const { return (method)(*this); }

	func var::getFunction() const { return (func)(*this); }

	void* var::getPtr() const { return (void*)*this; }

	genericError* var::getError() const {
		return (genericError*)*this;
	}

	binary var::getBinary() const {
		auto container = sPtr.get();
		if (container) {
			if (container->type == typeBinary)
				return *(binary*)container->data;
			else if (container->type == typeString) {
				auto str = *(string*)container->data;
				size_t size = str.size();
				auto result = binary(size);
				memcpy(result.data(), str.data(), size);
				return result;
			}
		}
		return binary();
	}

	void var::returnBinary(binary& result) const {
		auto container = sPtr.get();
		if (container) {
			if (container->type == typeBinary)
				result = *(binary*)container->data;
			else if (container->type == typeString) {
				auto str = *(string*)container->data;
				size_t size = str.size();
				result = binary(size);
				memcpy(result.data(), str.data(), size);
			}
		}
	}

	var::operator string() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt64:
					return to_string(*(int64_t*)container->data);
				case typePtr:
				case typeFunction:
				case typeMethod:
					return "0x" + to_string((uint64_t)container->data);
				case typeInt32:
					return to_string(*(int32_t*)container->data);
				case typeInt16:
					return to_string(*(int16_t*)container->data);
				case typeInt8:
					return to_string(*(int8_t*)container->data);
				case typeUInt64:
					return to_string(*(uint64_t*)container->data);
				case typeUInt32:
					return to_string(*(uint32_t*)container->data);
				case typeUInt16:
					return to_string(*(uint16_t*)container->data);
				case typeUInt8:
					return to_string(*(uint8_t*)container->data);
				case typeDouble:
					return to_string(*(double*)container->data);
				case typeFloat:
					return to_string(*(float*)container->data);
				case typeBool:
					return *(bool*)container->data ? "true" : "false";
				case typeString:
					return *(string*)container->data;
				case typeNull:
					return "null";
				case typeList:
					return ((list*)container->data)->getJSON();
				case typeObject:
					return ((object*)container->data)->getJSON();
				case typeException:
					return string(*(genericError*)container->data);
				case typeBinary: {
					auto bin = (binary*)container->data;
					return string((char*)bin->data(), bin->size());
				}
				default:
					break;
			}
		}

		return "";
	}

	var::operator const char*() const {
		auto container = sPtr.get();
		if (container) {
			if (container->type == typeString)
				return ((string*)container->data)->data();
			else if (container->type == typeBinary)
				return (char*)container->data;
		}
		return nullptr;
	}

	var::operator int64_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt64:
					return (*(int64_t*)container->data);
				case typePtr:
					return (int64_t)(void*)container->data;
				case typeInt32:
					return (int64_t)(*(int32_t*)container->data);
				case typeInt16:
					return (int64_t)(*(int16_t*)container->data);
				case typeInt8:
					return (int64_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (int64_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (int64_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (int64_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (int64_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (int64_t)(*(double*)container->data);
				case typeFloat:
					return (int64_t)(*(float*)container->data);
				case typeBool:
					return (int64_t)(*(bool*)container->data);
				case typeString:
					return atol((*(string*)container->data).c_str());
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator int32_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt32:
					return (*(int32_t*)container->data);
				case typeInt64:
					return (int32_t)(*(int64_t*)container->data);
				case typeInt16:
					return (int32_t)(*(int16_t*)container->data);
				case typeInt8:
					return (int32_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (int32_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (int32_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (int32_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (int32_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (int32_t)(*(double*)container->data);
				case typeFloat:
					return (int32_t)(*(float*)container->data);
				case typeBool:
					return (int32_t)(*(bool*)container->data);
				case typeString:
					return atoi((*(string*)container->data).c_str());
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator int16_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt16:
					return (*(int16_t*)container->data);
				case typeInt64:
					return (int16_t)(*(int64_t*)container->data);
				case typeInt32:
					return (int16_t)(*(int32_t*)container->data);
				case typeInt8:
					return (int16_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (int16_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (int16_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (int16_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (int16_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (int16_t)(*(double*)container->data);
				case typeFloat:
					return (int16_t)(*(float*)container->data);
				case typeBool:
					return (int16_t)(*(bool*)container->data);
				case typeString:
					return (int16_t)atoi(
						(*(string*)container->data).c_str());
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator int8_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt8:
					return (*(int8_t*)container->data);
				case typeInt64:
					return (int8_t)(*(int64_t*)container->data);
				case typeInt32:
					return (int8_t)(*(int32_t*)container->data);
				case typeInt16:
					return (int8_t)(*(int16_t*)container->data);
				case typeUInt64:
					return (int8_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (int8_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (int8_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (int8_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (int8_t)(*(double*)container->data);
				case typeFloat:
					return (int8_t)(*(float*)container->data);
				case typeBool:
					return (int8_t)(*(bool*)container->data);
				case typeString:
					return (int8_t)atoi(
						(*(string*)container->data).c_str());
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator uint64_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeUInt64:
					return (*(uint64_t*)container->data);
				case typeInt64:
					return (uint64_t)(*(int64_t*)container->data);
				case typePtr:
					return (uint64_t)(void*)container->data;
				case typeInt32:
					return (uint64_t)(*(int32_t*)container->data);
				case typeInt16:
					return (uint64_t)(*(int16_t*)container->data);
				case typeInt8:
					return (uint64_t)(*(int8_t*)container->data);
				case typeUInt32:
					return (uint64_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (uint64_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (uint64_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (uint64_t)(*(double*)container->data);
				case typeFloat:
					return (uint64_t)(*(float*)container->data);
				case typeBool:
					return (uint64_t)(*(bool*)container->data);
				case typeString:
					return (uint64_t)strtoul(
						(*(string*)container->data).c_str(), NULL, 0);
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator uint32_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeUInt32:
					return (*(uint32_t*)container->data);
				case typeInt64:
					return (uint32_t)(*(int64_t*)container->data);
				case typeInt32:
					return (uint32_t)(*(int32_t*)container->data);
				case typeInt16:
					return (uint32_t)(*(int16_t*)container->data);
				case typeInt8:
					return (uint32_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (uint32_t)(*(uint64_t*)container->data);
				case typeUInt16:
					return (uint32_t)(*(uint16_t*)container->data);
				case typeUInt8:
					return (uint32_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (uint32_t)(*(double*)container->data);
				case typeFloat:
					return (uint32_t)(*(float*)container->data);
				case typeBool:
					return (uint32_t)(*(bool*)container->data);
				case typeString:
					return (uint32_t)strtoul(
						(*(string*)container->data).c_str(), NULL, 0);
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator uint16_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeUInt16:
					return (*(uint16_t*)container->data);
				case typeInt64:
					return (uint16_t)(*(int64_t*)container->data);
				case typeInt32:
					return (uint16_t)(*(int32_t*)container->data);
				case typeInt16:
					return (uint16_t)(*(int16_t*)container->data);
				case typeInt8:
					return (uint16_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (uint16_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (uint16_t)(*(uint32_t*)container->data);
				case typeUInt8:
					return (uint16_t)(*(uint8_t*)container->data);
				case typeDouble:
					return (uint16_t)(*(double*)container->data);
				case typeFloat:
					return (uint16_t)(*(float*)container->data);
				case typeBool:
					return (uint16_t)(*(bool*)container->data);
				case typeString:
					return (uint16_t)strtoul(
						(*(string*)container->data).c_str(), NULL, 0);
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator uint8_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeUInt8:
					return (*(uint8_t*)container->data);
				case typeInt64:
					return (uint8_t)(*(int64_t*)container->data);
				case typeInt32:
					return (uint8_t)(*(int32_t*)container->data);
				case typeInt16:
					return (uint8_t)(*(int16_t*)container->data);
				case typeInt8:
					return (uint8_t)(*(int8_t*)container->data);
				case typeUInt64:
					return (uint8_t)(*(uint64_t*)container->data);
				case typeUInt32:
					return (uint8_t)(*(uint32_t*)container->data);
				case typeUInt16:
					return (uint8_t)(*(uint16_t*)container->data);
				case typeDouble:
					return (uint8_t)(*(double*)container->data);
				case typeFloat:
					return (uint8_t)(*(float*)container->data);
				case typeBool:
					return (uint8_t)(*(bool*)container->data);
				case typeString:
					return (uint8_t)strtoul(
						(*(string*)container->data).c_str(), NULL, 0);
				case typePtr:
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator double() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeDouble:
					return (*(double*)container->data);
				case typeInt64:
					return (double)(*(int64_t*)container->data);
				case typePtr:
					return (double)(uint64_t)(void*)container->data;
				case typeInt32:
					return (double)(*(int32_t*)container->data);
				case typeInt16:
					return (double)(*(int16_t*)container->data);
				case typeInt8:
					return (double)(*(int8_t*)container->data);
				case typeUInt64:
					return (double)(*(uint64_t*)container->data);
				case typeUInt32:
					return (double)(*(uint32_t*)container->data);
				case typeUInt16:
					return (double)(*(uint16_t*)container->data);
				case typeUInt8:
					return (double)(*(uint8_t*)container->data);
				case typeFloat:
					return (double)(*(float*)container->data);
				case typeBool:
					return (double)(*(bool*)container->data);
				case typeString:
					return stod((*(string*)container->data).c_str());
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator float() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeFloat:
					return (*(float*)container->data);
				case typeDouble:
					return (float)(*(double*)container->data);
				case typeInt64:
					return (float)(*(int64_t*)container->data);
				case typePtr:
					return (float)(uint64_t)(void*)container->data;
				case typeInt32:
					return (float)(*(int32_t*)container->data);
				case typeInt16:
					return (float)(*(int16_t*)container->data);
				case typeInt8:
					return (float)(*(int8_t*)container->data);
				case typeUInt64:
					return (float)(*(uint64_t*)container->data);
				case typeUInt32:
					return (float)(*(uint32_t*)container->data);
				case typeUInt16:
					return (float)(*(uint16_t*)container->data);
				case typeUInt8:
					return (float)(*(uint8_t*)container->data);
				case typeBool:
					return (float)(*(bool*)container->data);
				case typeString:
					return stof((*(string*)container->data).c_str());
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return 0;
	}

	var::operator bool() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeBool:
					return (bool)(*(bool*)container->data);
				case typeInt64:
					return (bool)(*(int64_t*)container->data);
				case typePtr:
					return (bool)(void*)container->data;
				case typeInt32:
					return (bool)(*(int32_t*)container->data);
				case typeInt16:
					return (bool)(*(int16_t*)container->data);
				case typeInt8:
					return (bool)(*(int8_t*)container->data);
				case typeUInt64:
					return (bool)(*(uint64_t*)container->data);
				case typeUInt32:
					return (bool)(*(uint32_t*)container->data);
				case typeUInt16:
					return (bool)(*(uint16_t*)container->data);
				case typeUInt8:
					return (bool)(*(uint8_t*)container->data);
				case typeFloat:
					return (bool)(*(float*)container->data);
				case typeDouble:
					return (bool)(*(double*)container->data);
				case typeBinary:
					return (*(binary*)container->data).size() > 0;
				case typeString:
					if (
						(*(string*)container->data) == "true" ||
						(*(string*)container->data) == "1")
						return true;
					if (
						(*(string*)container->data) == "false" ||
						(*(string*)container->data) == "0")
						return false;
				case typeNull:
				case typeList:
				case typeObject:
				case typeMethod:
				default:
					break;
			}
		}
		return false;
	}

	var::operator void*() const {
		auto container = sPtr.get();
		if (container && container->type == typePtr)
			return (void*)container->data;
		return nullptr;
	}

	var::operator method() const {
		auto container = sPtr.get();
		if (container && container->type == typeMethod)
			return (*(method*)container->data);
		return nullptr;
	}

	var::operator func() const {
		auto container = sPtr.get();
		if (container && container->type == typeFunction)
			return (*(func*)container->data);
		return nullptr;
	}

	var::operator genericError*() const {
		auto container = sPtr.get();
		if (container && container->type == typeException)
			return ((genericError*)container->data);
		return nullptr;
	}

	ostream& operator<<(ostream& os, const var& v) {
		os << string(v);
		return os;
	}

	var var::operator()(obj self, list args) const {
		auto func = getMethod();
		if (func != nullptr) return (self.*func)(args);
		return var();
	}

	var var::operator()(obj self) const {
		auto func = getMethod();
		if (func != nullptr) {
			auto empty = list();
			return (self.*func)(empty);
		}
		return var();
	}

	var var::operator()(list args) const {
		auto container = sPtr.get();
		if (container && container->type == typeFunction)
			return (*(func*)container->data)(args);
		return var();
	}
}  // namespace gold