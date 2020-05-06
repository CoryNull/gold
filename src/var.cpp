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
		switch (type) {
			case typeException:
				if (e) delete e;
				break;
			case typeList:
				if (a) delete a;
				break;
			case typeObject:
				if (o) delete o;
				break;
			case typeString:
				if (str) delete str;
				break;
			case typeBinary:
				if (bv) delete bv;
				break;
			default:
				break;
		}
	}

	var::varContainer::varContainer() {
		ptr = nullptr;
		type = typeNull;
	}

	var::varContainer::varContainer(const varContainer& other) {
		type = other.type;
		switch (type) {
			case typeException:
				e = new genericError(*other.e);
				break;
			case typeList:
				a = new list(*other.a);
				break;
			case typeObject:
				o = new obj(*other.o);
				break;
			case typeString:
				str = new string(*other.str);
				break;
			case typeBinary:
				bv = new binary(*other.bv);
				break;
			case typeFunction:
				fu = other.fu;
				break;
			case typeMethod:
				m = other.m;
				break;
			default:
				ptr = other.ptr;
				break;
		}
	}

	var::varPtr var::autoNull =
		var::varPtr(nullptr, [](void*) {});

	var::var() : sPtr(autoNull) {}

	var::var(const var& copy) : sPtr(copy.sPtr) {}

	var::var(void* v, types t) {
		auto ptr = new varContainer();
		ptr->ptr = v;
		ptr->type = t;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(const char* value) {
		auto ptr = new varContainer();
		ptr->str = new string(value);
		ptr->type = typeString;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(string str) {
		auto ptr = new varContainer();
		ptr->str = new string(str);
		ptr->type = typeString;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(const binary& bin) {
		auto ptr = new varContainer();
		ptr->bv = new binary(bin);
		ptr->type = typeUInt8;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(int64_t v) {
		auto ptr = new varContainer();
		ptr->i64 = v;
		ptr->type = typeInt16;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(int32_t v) {
		auto ptr = new varContainer();
		ptr->i32 = v;
		ptr->type = typeInt32;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(int16_t v) {
		auto ptr = new varContainer();
		ptr->i16 = v;
		ptr->type = typeInt16;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(int8_t v) {
		auto ptr = new varContainer();
		ptr->i8 = v;
		ptr->type = typeInt8;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(uint64_t v) {
		auto ptr = new varContainer();
		ptr->u64 = v;
		ptr->type = typeUInt64;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(uint32_t v) {
		auto ptr = new varContainer();
		ptr->u32 = v;
		ptr->type = typeUInt32;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(uint16_t v) {
		auto ptr = new varContainer();
		ptr->u16 = v;
		ptr->type = typeUInt16;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(uint8_t v) {
		auto ptr = new varContainer();
		ptr->u8 = v;
		ptr->type = typeUInt8;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(double v) {
		auto ptr = new varContainer();
		ptr->d = v;
		ptr->type = typeDouble;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(float v) {
		auto ptr = new varContainer();
		ptr->f = v;
		ptr->type = typeFloat;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(const list& v) {
		auto ptr = new varContainer();
		ptr->a = new list(*(list*)&v);
		ptr->type = typeList;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(const obj& v) {
		auto ptr = new varContainer();
		ptr->o = new object(v);
		ptr->type = typeObject;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(method v) {
		auto ptr = new varContainer();
		ptr->m = v;
		ptr->type = typeMethod;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(func v) {
		auto ptr = new varContainer();
		ptr->fu = v;
		ptr->type = typeFunction;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(bool v) {
		auto ptr = new varContainer();
		ptr->b = v;
		ptr->type = typeBool;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::var(const genericError& v) {
		auto ptr = new varContainer();
		ptr->e = (genericError*)&v;
		ptr->type = typeException;
		sPtr = shared_ptr<varContainer>(ptr);
	}

	var::~var() {}

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
					auto o = *rCon->o;
					auto mob = getObject();
					return &o.data == &mob.data;
				}
				case typeList: {
					auto arr = *rCon->a;
					auto mar = getList();
					return arr.data == mar.data;
				}
				case typeString:
					return *rCon->str == (string) * this;
				case typeDouble:
					return rCon->d == (double)*this;
				case typeFloat:
					return rCon->f == (float)*this;
				case typeInt64:
					return rCon->i64 == (int64_t) * this;
				case typeInt32:
					return rCon->i32 == (int32_t) * this;
				case typeInt16:
					return rCon->i16 == (int16_t) * this;
				case typeInt8:
					return rCon->i8 == (int8_t) * this;
				case typeUInt64:
					return rCon->u64 == (uint64_t) * this;
				case typeUInt32:
					return rCon->u32 == (uint32_t) * this;
				case typeUInt16:
					return rCon->u16 == (uint16_t) * this;
				case typeUInt8:
					return rCon->u8 == (uint8_t) * this;
				case typeBool:
					return rCon->b == (bool)*this;
				default:
					return rCon->ptr == (void*)*this;
					break;
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
			auto p = *container->o;
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
			if (container->type == typeList)
				return container->a->size() == 0;
			else if (container->type == typeObject)
				return container->o->size() == 0;
			else if (container->type == typeString)
				return container->str->length() == 0;
			else if (container->type == typeFunction)
				return container->ptr == 0;
			else if (container->type == typeMethod)
				return container->ptr == 0;
			else if (container->type == typeDouble)
				return container->d == 0.0;
			else if (container->type == typeFloat)
				return container->f == 0.0;
			else if (container->type == typeUInt64)
				return container->u64 == 0;
			else if (container->type == typeUInt32)
				return container->u32 == 0;
			else if (container->type == typeUInt16)
				return container->u16 == 0;
			else if (container->type == typeUInt8)
				return container->u8 == 0;
			else if (container->type == typeInt64)
				return container->i64 == 0;
			else if (container->type == typeInt32)
				return container->i32 == 0;
			else if (container->type == typeInt16)
				return container->i16 == 0;
			else if (container->type == typeInt8)
				return container->i8 == 0;
			else if (container->type == typeBool)
				return container->b == false;
			else if (container->type == typePtr)
				return container->ptr == nullptr;
			else if (container->type == typeException)
				return container->ptr == nullptr;
			else
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
			return *container->a;
		return list();
	}

	object var::getObject() const {
		auto container = sPtr.get();
		if (container && container->type == typeObject)
			return *container->o;
		return object();
	}

	void var::returnList(list& result) const {
		auto container = sPtr.get();
		if (container && container->type == typeList)
			result = *container->a;
	}

	void var::returnObject(obj& result) const {
		auto container = sPtr.get();
		if (container && container->type == typeObject)
			result = *container->o;
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
				return *container->bv;
			else if (container->type == typeString) {
				size_t size = container->str->size();
				auto result = binary(size);
				memcpy(result.data(), container->str->data(), size);
				return result;
			}
		}
		return binary();
	}

	void var::returnBinary(binary& result) const {
		auto container = sPtr.get();
		if (container) {
			if (container->type == typeBinary)
				result = *container->bv;
			else if (container->type == typeString) {
				size_t size = container->str->size();
				result = binary(size);
				memcpy(result.data(), container->str->data(), size);
			}
		}
	}

	var::operator string() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt64:
					return to_string(container->i64);
				case typePtr:
					return "0x" + to_string((uint64_t)container->ptr);
				case typeInt32:
					return to_string(container->i32);
				case typeInt16:
					return to_string(container->i16);
				case typeInt8:
					return to_string(container->i8);
				case typeUInt64:
					return to_string(container->u64);
				case typeUInt32:
					return to_string(container->u32);
				case typeUInt16:
					return to_string(container->u16);
				case typeUInt8:
					return to_string(container->u8);
				case typeDouble:
					return to_string(container->d);
				case typeFloat:
					return to_string(container->f);
				case typeBool:
					return container->b ? "true" : "false";
				case typeString:
					return *container->str;
				case typeNull:
					return "null";
				case typeList:
					return container->a->getJSON();
				case typeObject:
					return container->o->getJSON();
				case typeFunction:
					return "0x" + to_string((uint64_t)&container->fu);
				case typeMethod:
					return "0x" + to_string((uint64_t)&container->m);
				case typeException:
					return string(*container->e);
				case typeBinary:
					return string(
						(char*)container->bv->data(),
						container->bv->size());
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
				return container->str->data();
			else if (container->type == typeBinary)
				return (char*)container->bv->data();
		}
		return nullptr;
	}

	var::operator int64_t() const {
		auto container = sPtr.get();
		if (container) {
			switch (container->type) {
				case typeInt64:
					return container->i64;
				case typePtr:
					return (int64_t)container->ptr;
				case typeInt32:
					return (int64_t)container->i32;
				case typeInt16:
					return (int64_t)container->i16;
				case typeInt8:
					return (int64_t)container->i8;
				case typeUInt64:
					return (int64_t)container->u64;
				case typeUInt32:
					return (int64_t)container->u32;
				case typeUInt16:
					return (int64_t)container->u16;
				case typeUInt8:
					return (int64_t)container->u8;
				case typeDouble:
					return (int64_t)container->d;
				case typeFloat:
					return (int64_t)container->f;
				case typeBool:
					return (int64_t)container->b;
				case typeString:
					return atol(container->str->c_str());
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
					return container->i32;
				case typeInt64:
					return (int32_t)container->i64;
				case typeInt16:
					return (int32_t)container->i16;
				case typeInt8:
					return (int32_t)container->i8;
				case typeUInt64:
					return (int32_t)container->u64;
				case typeUInt32:
					return (int32_t)container->u32;
				case typeUInt16:
					return (int32_t)container->u16;
				case typeUInt8:
					return (int32_t)container->u8;
				case typeDouble:
					return (int32_t)container->d;
				case typeFloat:
					return (int32_t)container->f;
				case typeBool:
					return (int32_t)container->b;
				case typeString:
					return atoi(container->str->c_str());
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
					return container->i16;
				case typeInt64:
					return (int16_t)container->i64;
				case typeInt32:
					return (int16_t)container->i32;
				case typeInt8:
					return (int16_t)container->i8;
				case typeUInt64:
					return (int16_t)container->u64;
				case typeUInt32:
					return (int16_t)container->u32;
				case typeUInt16:
					return (int16_t)container->u16;
				case typeUInt8:
					return (int16_t)container->u8;
				case typeDouble:
					return (int16_t)container->d;
				case typeFloat:
					return (int16_t)container->f;
				case typeBool:
					return (int16_t)container->b;
				case typeString:
					return (int16_t)atoi(container->str->c_str());
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
					return container->i8;
				case typeInt64:
					return (int8_t)container->i64;
				case typeInt32:
					return (int8_t)container->i32;
				case typeInt16:
					return (int8_t)container->i16;
				case typeUInt64:
					return (int8_t)container->u64;
				case typeUInt32:
					return (int8_t)container->u32;
				case typeUInt16:
					return (int8_t)container->u16;
				case typeUInt8:
					return (int8_t)container->u8;
				case typeDouble:
					return (int8_t)container->d;
				case typeFloat:
					return (int8_t)container->f;
				case typeBool:
					return (int8_t)container->b;
				case typeString:
					return (int8_t)atoi(container->str->c_str());
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
					return container->u64;
				case typeInt64:
					return (uint64_t)container->i64;
				case typePtr:
					return (uint64_t)container->ptr;
				case typeInt32:
					return (uint64_t)container->i32;
				case typeInt16:
					return (uint64_t)container->i16;
				case typeInt8:
					return (uint64_t)container->i8;
				case typeUInt32:
					return (uint64_t)container->u32;
				case typeUInt16:
					return (uint64_t)container->u16;
				case typeUInt8:
					return (uint64_t)container->u8;
				case typeDouble:
					return (uint64_t)container->d;
				case typeFloat:
					return (uint64_t)container->f;
				case typeBool:
					return (uint64_t)container->b;
				case typeString:
					return (uint64_t)strtoul(
						container->str->c_str(), NULL, 0);
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
					return container->u32;
				case typeInt64:
					return (uint32_t)container->i64;
				case typeInt32:
					return (uint32_t)container->i32;
				case typeInt16:
					return (uint32_t)container->i16;
				case typeInt8:
					return (uint32_t)container->i8;
				case typeUInt64:
					return (uint32_t)container->u64;
				case typeUInt16:
					return (uint32_t)container->u16;
				case typeUInt8:
					return (uint32_t)container->u8;
				case typeDouble:
					return (uint32_t)container->d;
				case typeFloat:
					return (uint32_t)container->f;
				case typeBool:
					return (uint32_t)container->b;
				case typeString:
					return (uint32_t)strtoul(
						container->str->c_str(), NULL, 0);
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
					return container->u16;
				case typeInt64:
					return (uint16_t)container->i64;
				case typeInt32:
					return (uint16_t)container->i32;
				case typeInt16:
					return (uint16_t)container->i16;
				case typeInt8:
					return (uint16_t)container->i8;
				case typeUInt64:
					return (uint16_t)container->u64;
				case typeUInt32:
					return (uint16_t)container->u32;
				case typeUInt8:
					return (uint16_t)container->u8;
				case typeDouble:
					return (uint16_t)container->d;
				case typeFloat:
					return (uint16_t)container->f;
				case typeBool:
					return (uint16_t)container->b;
				case typeString:
					return (uint16_t)strtoul(
						container->str->c_str(), NULL, 0);
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
					return container->u8;
				case typeInt64:
					return (uint8_t)container->i64;
				case typeInt32:
					return (uint8_t)container->i32;
				case typeInt16:
					return (uint8_t)container->i16;
				case typeInt8:
					return (uint8_t)container->i8;
				case typeUInt64:
					return (uint8_t)container->u64;
				case typeUInt32:
					return (uint8_t)container->u32;
				case typeUInt16:
					return (uint8_t)container->u16;
				case typeDouble:
					return (uint8_t)container->d;
				case typeFloat:
					return (uint8_t)container->f;
				case typeBool:
					return (uint8_t)container->b;
				case typeString:
					return (uint8_t)strtoul(
						container->str->c_str(), NULL, 0);
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
					return container->d;
				case typeInt64:
					return (double)container->i64;
				case typePtr:
					return (double)(uint64_t)container->ptr;
				case typeInt32:
					return (double)container->i32;
				case typeInt16:
					return (double)container->i16;
				case typeInt8:
					return (double)container->i8;
				case typeUInt64:
					return (double)container->u64;
				case typeUInt32:
					return (double)container->u32;
				case typeUInt16:
					return (double)container->u16;
				case typeUInt8:
					return (double)container->u8;
				case typeFloat:
					return (double)container->f;
				case typeBool:
					return (double)container->b;
				case typeString:
					return stod(container->str->c_str());
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
					return container->f;
				case typeDouble:
					return (float)container->d;
				case typeInt64:
					return (float)container->i64;
				case typePtr:
					return (float)(uint64_t)container->ptr;
				case typeInt32:
					return (float)container->i32;
				case typeInt16:
					return (float)container->i16;
				case typeInt8:
					return (float)container->i8;
				case typeUInt64:
					return (float)container->u64;
				case typeUInt32:
					return (float)container->u32;
				case typeUInt16:
					return (float)container->u16;
				case typeUInt8:
					return (float)container->u8;
				case typeBool:
					return (float)container->b;
				case typeString:
					return stof(container->str->c_str());
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
					return (bool)container->b;
				case typeInt64:
					return (bool)container->i64;
				case typePtr:
					return (bool)container->ptr;
				case typeInt32:
					return (bool)container->i32;
				case typeInt16:
					return (bool)container->i16;
				case typeInt8:
					return (bool)container->i8;
				case typeUInt64:
					return (bool)container->u64;
				case typeUInt32:
					return (bool)container->u32;
				case typeUInt16:
					return (bool)container->u16;
				case typeUInt8:
					return (bool)container->u8;
				case typeFloat:
					return (bool)container->f;
				case typeDouble:
					return (bool)container->d;
				case typeBinary:
					return container->bv->size() > 0;
				case typeString:
					if (
						*container->str == "true" || *container->str == "1")
						return true;
					if (
						*container->str == "false" ||
						*container->str == "0")
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
			return container->ptr;
		return nullptr;
	}

	var::operator method() const {
		auto container = sPtr.get();
		if (container && container->type == typeMethod)
			return container->m;
		return nullptr;
	}

	var::operator func() const {
		auto container = sPtr.get();
		if (container && container->type == typeFunction)
			return container->fu;
		return nullptr;
	}

	var::operator genericError*() const {
		auto container = sPtr.get();
		if (container && container->type == typeException)
			return container->e;
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
			return (container->fu)(args);
		return var();
	}
}  // namespace gold