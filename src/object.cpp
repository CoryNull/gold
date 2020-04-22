
#include "object.hpp"

#include <stdarg.h>
#include <string.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>

#include "array.hpp"

namespace gold {
	using namespace std;

	shared_ptr<object::objData> object::newObjData(
		object* p, omap m, uint64_t id) {
		auto d =
			shared_ptr<objData>(new objData{m, p, id, mutex()});
		d->id = id != 0 ? id : (uint64_t)d.get();
		return d;
	}

	object::object(object* p) : data(newObjData(p)) {}

	object::object(const object& c)
		: data(newObjData(
				c.data->parent, c.data->items, c.data->id)) {}

	object::object(object c, object* p)
		: data(newObjData(p, c.data->items)) {}

	object::object(json value) : data(newObjData()) {
		if (value.is_object()) {
			for (auto it = value.begin(); it != value.end(); ++it) {
				auto name = it.key();
				auto value = it.value();
				var toSet = var();
				switch (value.type()) {
					case detail::value_t::object: {
						auto obj = object(value);
						toSet = var(obj);
						break;
					}
					case detail::value_t::array: {
						auto arr = array(value);
						toSet = var(arr);
						break;
					}
					case detail::value_t::string:
						toSet = var((char*)((string)value).c_str());
						break;
					case detail::value_t::boolean:
						toSet = var((bool)value);
						break;
					case detail::value_t::number_integer:
						toSet = var((int64_t)value);
						break;
					case detail::value_t::number_unsigned:
						toSet = var((uint64_t)value);
						break;
					case detail::value_t::number_float:
						toSet = var((double)value);
						break;
					default:
					case detail::value_t::null:
						break;
				}
				setVar(name, toSet);
			}
		}
	}

	object::object(var value) {
		auto obj = value.getObject();
		const auto o = (obj != nullptr);
		data =
			o ? newObjData(
						obj->data->parent, obj->data->items, obj->data->id)
				: newObjData();
	}

	object::object(object_list list, object* p)
		: data(newObjData(p, omap(list))) {}

	object::~object() {
		auto uses = data.use_count();
		if (uses == 0) this->callMethod("destroy");
	}

	object::omap::iterator object::begin() {
		unique_lock<mutex> gaurd(data->omutex);
		return std::begin(data->items);
	}

	object::omap::iterator object::end() {
		unique_lock<mutex> gaurd(data->omutex);
		return std::end(data->items);
	}

	uint64_t object::getSize() {
		unique_lock<mutex> gaurd(data->omutex);
		return data->items.size();
	}

	types object::getType(string name) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getType();
		if (data->parent) return data->parent->getType(name);
		return typeNull;
	}

	json object::getJSON() {
		unique_lock<mutex> gaurd(data->omutex);
		json j = json::object();
		auto end = data->items.end();
		for (auto i = data->items.begin(); i != end; ++i) {
			auto name = i->first;
			auto value = i->second;
			switch (value.getType()) {
				case typeNull:
					j[name] = (nullptr);
					break;
				case typeArray: {
					auto array = value.getArray();
					j[name] = (array->getJSON());
					break;
				}
				case typeObject: {
					auto object = value.getObject();
					j[name] = (object->getJSON());
					break;
				}
				case typeString:
					j[name] = (value.getString());
					break;
				case typeInt64:
					j[name] = (value.getInt64());
					break;
				case typeInt32:
					j[name] = (value.getInt32());
					break;
				case typeInt16:
					j[name] = (value.getInt16());
					break;
				case typeInt8:
					j[name] = (value.getInt8());
					break;
				case typeUInt64:
					j[name] = (value.getUInt64());
					break;
				case typeUInt32:
					j[name] = (value.getUInt32());
					break;
				case typeUInt16:
					j[name] = (value.getUInt16());
					break;
				case typeUInt8:
					j[name] = (value.getUInt8());
					break;
				case typeDouble:
					j[name] = (value.getDouble());
					break;
				case typeFloat:
					j[name] = (value.getFloat());
					break;
				case typeBool:
					j[name] = (value.getBool());
					break;
				default:
					break;
			}
		}
		return j;
	}

	binary object::getBSON() { return json::to_bson(getJSON()); }

	binary object::getCBOR() { return json::to_cbor(getJSON()); }

	binary object::getMsgPack() {
		return json::to_msgpack(getJSON());
	}

	binary object::getUBJSON() {
		return json::to_ubjson(getJSON());
	}

	var object::callMethod(string name) {
		auto method = this->getMethod(name);
		var resp;
		if (method != nullptr) resp = (this->*method)(varList());
		return resp;
	}

	var object::callMethod(string name, varList args) {
		auto method = this->getMethod(name);
		var resp;
		if (method != nullptr) resp = (this->*method)(args);
		return resp;
	}

	void object::copy(object& other) {
		auto end = other.end();
		for (auto it = other.begin(); it != end; ++it)
			this->setVar(it->first, it->second);
	}

	void object::setParent(object* other) {
		unique_lock<mutex> gaurd(data->omutex);
		data->parent = other;
	}

	object* object::getParent() { return data->parent; }

	void object::setString(string name, char* value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setString(string name, string value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt64(string name, int64_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt32(string name, int32_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt16(string name, int16_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setInt8(string name, int8_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt64(string name, uint64_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt32(string name, uint32_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt16(string name, uint16_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setUInt8(string name, uint8_t value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setDouble(string name, double value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setFloat(string name, float value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setBool(string name, bool value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setArray(string name, array value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setObject(string name, object value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setMethod(string name, method& value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value);
	}

	void object::setPtr(string name, void* value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var(value, typePtr);
	}

	void object::setVar(string name, var value) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = value;
	}

	void object::setNull(string name) {
		unique_lock<mutex> gaurd(data->omutex);
		data->items[name] = var();
	}

	string object::getString(string name, char* def) {
		try {
			if (data == nullptr) return def;
			unique_lock<mutex> gaurd(data->omutex);
			auto it = data->items.find(name);
			if (it != data->items.end())
				return it->second.getString();
			if (data->parent)
				return data->parent->getString(name, def);
			return string(def);
		} catch (exception& e) {
			return string();
		}
	}

	int64_t object::getInt64(string name, int64_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt64();
		if (data->parent) return data->parent->getInt64(name, def);
		return def;
	}

	int32_t object::getInt32(string name, int32_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt32();
		if (data->parent) return data->parent->getInt32(name, def);
		return def;
	}

	int16_t object::getInt16(string name, int16_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt16();
		if (data->parent) return data->parent->getInt16(name, def);
		return def;
	}

	int8_t object::getInt8(string name, int8_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getInt8();
		if (data->parent) return data->parent->getInt8(name, def);
		return def;
	}

	uint64_t object::getUInt64(string name, uint64_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt64();
		if (data->parent) return data->parent->getUInt64(name, def);
		return def;
	}

	uint32_t object::getUInt32(string name, uint32_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt32();
		if (data->parent) return data->parent->getUInt32(name, def);
		return def;
	}

	uint16_t object::getUInt16(string name, uint16_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt16();
		if (data->parent) return data->parent->getUInt16(name, def);
		return def;
	}

	uint8_t object::getUInt8(string name, uint8_t def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getUInt8();
		if (data->parent) return data->parent->getUInt8(name, def);
		return def;
	}

	double object::getDouble(string name, double def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getDouble();
		if (data->parent) return data->parent->getDouble(name, def);
		return def;
	}

	float object::getFloat(string name, float def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getFloat();
		if (data->parent) return data->parent->getFloat(name, def);
		return def;
	}

	bool object::getBool(string name, bool def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getBool();
		if (data->parent) return data->parent->getBool(name, def);
		return def;
	}

	array* object::getArray(string name, array* def) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getArray();
		if (data->parent) return data->parent->getArray(name, def);
		return def;
	}

	object* object::getObject(string name, object* def) {
		try {
			unique_lock<mutex> gaurd(data->omutex);
			auto it = data->items.find(name);
			if (it != data->items.end())
				return it->second.getObject();
			if (data->parent)
				return data->parent->getObject(name, def);
			return def;
		} catch (exception& e) {
			return def;
		}
	}

	method object::getMethod(string name) {
		unique_lock<mutex> gaurd(data->omutex);
		auto end = data->items.end();
		auto it = data->items.begin();
		while (it != end) {
			auto key = it->first;
			if (key == name) break;
			it++;
		}
		if (it != end) return it->second.getMethod();
		if (data->parent) return data->parent->getMethod(name);
		return nullptr;
	}

	void* object::getPtr(string name, void* def) {
		if (!data) return def;
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second.getPtr();
		if (data->parent) return data->parent->getPtr(name, def);
		return def;
	}

	var object::getVar(string name) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second;
		if (data->parent) return data->parent->getVar(name);
		return var();
	}

	var object::operator[](string name) {
		unique_lock<mutex> gaurd(data->omutex);
		auto it = data->items.find(name);
		if (it != data->items.end()) return it->second;
		if (data->parent) return data->parent->operator[](name);
		return var();
	}

	var object::operator()(string name, varList args) {
		return this->callMethod(name, args);
	}

	var object::operator()(string name) {
		return this->callMethod(name);
	}

	bool object::operator==(const object& other) {
		if (data->id == other.data->id) return true;
		if (data->items == other.data->items) return true;
		if (data.get() == other.data.get()) return true;
		if (data == other.data) return true;
		return false;
	}

	object& object::operator=(const object rhs) {
		data = rhs.data;
		return *this;
	}

	object object::parseURLEncoded(string value) {
		auto ret = object();
		auto it = value.begin();
		string buffer = "";
		string key = "";
		while (it != value.end()) {
			if (isalnum(*it))
				buffer += *it;
			else if (*it == '=') {
				key = buffer;
				buffer = "";
			} else if (*it == '&') {
				auto vType = ret.getType(key);
				if (vType == typeString) {
					auto copy = ret.getString(key);
					ret.setArray(key, array({copy, buffer}));
				} else if (vType == typeArray) {
					auto arr = ret.getArray(key);
					arr->pushString(buffer);
				} else {
					ret.setString(key, buffer);
				}
				buffer = "";
				key = "";
			} else if (*it == '.') {
				buffer += ".";
			} else if (*it == '+') {
				buffer += " ";
			} else if (*it == '%') {
				string h = "0x";
				h += *(++it);
				h += *(++it);
				auto c = (char)stoul(h, nullptr, 16);
				buffer += string(&c, 1);
			}
			++it;
		}
		if (key.size() > 0 && buffer.size() > 0)
			ret.setString(key, buffer);
		return ret;
	}

	var object::loadJSON(string path) {
		try {
			ifstream ss(path);
			if (!ss.good())
				return var(genericError("file doesn't exist"));
			json j = json();
			ss >> j;
			ss.close();
			if (j.is_object()) {
				object obj(j);
				return var(obj);
			} else
				return var(genericError("not object"));
		} catch (exception& err) {
			return var(genericError(err));
		}
	}

	void object::saveJSON(string path, object value) {
		ofstream ss(path);
		json j = value.getJSON();
		ss << j.dump(1, '\t') << endl;
		ss.close();
	}

	var object::loadBSON(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_bson(data);
			if (j.is_object())
				return var(object(j));
			else
				return var(genericError("failed to parse"));
		} catch (exception& err) {
			return var(genericError(err));
		}
	}

	void object::saveBSON(string path, object value) {
		ofstream ss(path);
		auto data = value.getBSON();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadCBOR(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_cbor(data);
			if (j.is_object())
				return object(j);
			else
				return var();
		} catch (exception& err) {
			return var();
		}
	}

	void object::saveCBOR(string path, object value) {
		ofstream ss(path);
		auto data = value.getCBOR();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadMsgPack(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_msgpack(data);
			if (j.is_object())
				return object(j);
			else
				return var();
		} catch (exception& err) {
			return var();
		}
	}

	void object::saveMsgPack(string path, object value) {
		ofstream ss(path);
		auto data = value.getMsgPack();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

	var object::loadUBJSON(string path) {
		try {
			ifstream ss(path);
			vector<uint8_t> data;
			ss.seekg(0, ss.end);
			data.resize(ss.tellg());
			ss.seekg(0, ss.beg);
			ss.read((char*)data.data(), data.size());
			ss.close();
			json j = json::from_ubjson(data);
			if (j.is_object())
				return object(j);
			else
				return var();
		} catch (exception& err) {
			return var();
		}
	}

	void object::saveUBJSON(string path, object value) {
		ofstream ss(path);
		auto data = value.getUBJSON();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

}  // namespace gold