
#include "object.hpp"

#include <string.h>

#include <fstream>
#include <iostream>

#include "array.hpp"

namespace red {

	object::object(object* p) : items(new omap()) { setParent(p); }

	object::object(object& copy) : items(copy.items), parent(copy.parent) {}

	object::object(object& c, object p) : items(new omap(*c.items)), parent(p) {}

	object::object(json value) : items(new omap()) {
		setParent(nullptr);
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

	object::object(object_list list, object* p) : items(new omap(list)), parent(p) {}

	object::omap::iterator object::begin() { return std::begin(*items); }

	object::omap::iterator object::end() { return std::end(*items); }

	uint64_t object::getSize() { return items->size(); }

	types object::getType(string name) {
		uint64_t index;
		auto it = items->find(name);
		if (it != items->end()) return it->second.getType();
		return typeNull;
	}

	json object::getJSON() {
		json j = json::object();
		auto end = items->end();
		for (auto i = items->begin(); i != end; ++i) {
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

	binary object::getMsgPack() { return json::to_msgpack(getJSON()); }

	binary object::getUBJSON() { return json::to_ubjson(getJSON()); }

	var object::callMethod(string name) {
		method method = this->getMethod(name, 0);
		static var nullVar;
		var resp;
		if (method != nullptr) resp = (*method)(*this, nullVar);
		return resp;
	}

	var object::callMethod(string name, var args) {
		method method = this->getMethod(name, 0);
		var resp;
		if (method != nullptr) resp = (*method)(*this, args);
		return resp;
	}

	void object::copy(object& other) {
		auto end = other.end();
		for (auto it = other.begin(); it != end; ++it) this->setVar(it->first, it->second);
	}

	void object::setParent(object* other) {
		if (other)
			this->parent = var(*other);
		else
			this->parent = var();
	}

	object* object::getParent() { return this->parent.getObject(); }

	void object::setString(string name, char* value) { (*items)[name] = var(value); }

	void object::setInt64(string name, int64_t value) { (*items)[name] = var(value); }

	void object::setInt32(string name, int32_t value) { (*items)[name] = var(value); }

	void object::setInt16(string name, int16_t value) { (*items)[name] = var(value); }

	void object::setInt8(string name, int8_t value) { (*items)[name] = var(value); }

	void object::setUInt64(string name, uint64_t value) { (*items)[name] = var(value); }

	void object::setUInt32(string name, uint32_t value) { (*items)[name] = var(value); }

	void object::setUInt16(string name, uint16_t value) { (*items)[name] = var(value); }

	void object::setUInt8(string name, uint8_t value) { (*items)[name] = var(value); }

	void object::setDouble(string name, double value) { (*items)[name] = var(value); }

	void object::setFloat(string name, float value) { (*items)[name] = var(value); }

	void object::setBool(string name, bool value) { (*items)[name] = var(value); }

	void object::setArray(string name, array value) { (*items)[name] = var(value); }

	void object::setObject(string name, object value) { (*items)[name] = var(value); }

	void object::setMethod(string name, method value) { (*items)[name] = var(value); }

	void object::setPtr(string name, void* value) { (*items)[name] = var(value); }

	void object::setVar(string name, var value) { (*items)[name] = value; }

	void object::setNull(string name) { (*items)[name] = var(); }

	const char* object::getString(string name, char* def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getString();
		if (this->parent.isObject()) return this->parent.getObject()->getString(name, def);
		return def;
	}

	int64_t object::getInt64(string name, int64_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getInt64();
		if (this->parent.isObject()) return this->parent.getObject()->getInt64(name, def);
		return def;
	}

	int32_t object::getInt32(string name, int32_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getInt32();
		if (this->parent.isObject()) return this->parent.getObject()->getInt32(name, def);
		return def;
	}

	int16_t object::getInt16(string name, int16_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getInt16();
		if (this->parent.isObject()) return this->parent.getObject()->getInt16(name, def);
		return def;
	}

	int8_t object::getInt8(string name, int8_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getInt8();
		if (this->parent.isObject()) return this->parent.getObject()->getInt8(name, def);
		return def;
	}

	uint64_t object::getUInt64(string name, uint64_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getUInt64();
		if (this->parent.isObject()) return this->parent.getObject()->getUInt64(name, def);
		return def;
	}

	uint32_t object::getUInt32(string name, uint32_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getUInt32();
		if (this->parent.isObject()) return this->parent.getObject()->getUInt32(name, def);
		return def;
	}

	uint16_t object::getUInt16(string name, uint16_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getUInt16();
		if (this->parent.isObject()) return this->parent.getObject()->getUInt16(name, def);
		return def;
	}

	uint8_t object::getUInt8(string name, uint8_t def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getUInt8();
		if (this->parent.isObject()) return this->parent.getObject()->getUInt8(name, def);
		return def;
	}

	double object::getDouble(string name, double def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getDouble();
		if (this->parent.isObject()) return this->parent.getObject()->getDouble(name, def);
		return def;
	}

	float object::getFloat(string name, float def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getFloat();
		if (this->parent.isObject()) return this->parent.getObject()->getFloat(name, def);
		return def;
	}

	bool object::getBool(string name, bool def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getBool();
		if (this->parent.isObject()) return this->parent.getObject()->getBool(name, def);
		return def;
	}

	array* object::getArray(string name, array* def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getArray();
		if (this->parent.isObject()) return this->parent.getObject()->getArray(name, def);
		return def;
	}

	object* object::getObject(string name, object* def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getObject();
		if (this->parent.isObject()) return this->parent.getObject()->getObject(name, def);
		return def;
	}

	method object::getMethod(string name, method def) {
		auto end = items->end();
		auto it = items->begin();
		while (it != end) {
			auto key = it->first;
			if (key == name) break;
			it++;
		}
		if (it != items->end()) return it->second.getMethod();
		if (this->parent.isObject()) return this->parent.getObject()->getMethod(name, def);
		return def;
	}

	void* object::getPtr(string name, void* def) {
		auto it = items->find(name);
		if (it != items->end()) return it->second.getPtr();
		if (this->parent.isObject()) return this->parent.getObject()->getPtr(name, def);
		return def;
	}

	var object::operator[](string name) {
		auto it = items->find(name);
		if (it != items->end()) return it->second;
		if (this->parent.isObject()) return this->parent.getObject()->operator[](name);
		return var();
	}

	var object::operator()(string name, var& args) { return this->callMethod(name, args); }

	var object::operator()(string name) { return this->callMethod(name); }

	var object::loadJSON(string path) {
		try {
			ifstream ss(path);
			if (!ss.good()) return var(runtime_error("file doesn't exist"));
			json j = json();
			ss >> j;
			ss.close();
			if (j.is_object()) {
				object obj(j);
				return var(obj);
			} else
				return var(runtime_error("not object"));
		} catch (exception err) {
			return var(err);
		}
	}

	void object::saveJSON(string path, object& value) {
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
				return var(new object(j));
			else
				return var(logic_error("failed to parse"));
		} catch (exception err) {
			return var(err);
		}
	}

	void object::saveBSON(string path, object& value) {
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
				return var(new object(j));
			else
				return var();
		} catch (exception err) {
			return var();
		}
	}

	void object::saveCBOR(string path, object& value) {
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
				return var(new object(j));
			else
				return var();
		} catch (exception err) {
			return var();
		}
	}

	void object::saveMsgPack(string path, object& value) {
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
				return var(new object(j));
			else
				return var();
		} catch (exception err) {
			return var();
		}
	}

	void object::saveUBJSON(string path, object& value) {
		ofstream ss(path);
		auto data = value.getUBJSON();
		ss.write((char*)data.data(), data.size());
		ss.close();
	}

}  // namespace red