
#include "array.hpp"

#include "object.hpp"
#include "types.hpp"

namespace red {

	array::array() : items(new avec()), amutex() {}

	array::array(array& copy) : items(copy.items), amutex() {}

	array::array(json value) : items(new avec()), amutex() {
		if (value.is_array()) {
			for (auto it = value.begin(); it != value.end(); ++it) {
				auto name = std::stoul(it.key());
				auto value = it.value();
				switch (value.type()) {
					case detail::value_t::null:
						this->setNull(name);
						break;
					case detail::value_t::object:
						this->setObject(name, value);
						break;
					case detail::value_t::array:
						this->setArray(name, value);
						break;
					case detail::value_t::string:
						this->setString(
							name, (char*)((string)value).c_str());
						break;
					case detail::value_t::boolean:
						this->setBool(name, (bool)value);
						break;
					case detail::value_t::number_integer:
						this->setInt64(name, (int64_t)value);
						break;
					case detail::value_t::number_unsigned:
						this->setUInt64(name, (uint64_t)value);
						break;
					case detail::value_t::number_float:
						this->setDouble(name, (double)value);
						break;
					default:
						break;
				}
			}
		}
	}

	array::~array() {}

	uint64_t array::getSize() {
		unique_lock<mutex> gaurd(amutex);
		return items->size();
	}

	void array::pop() {
		unique_lock<mutex> gaurd(amutex);
		items->pop_back();
	}

	types array::getType(uint64_t index) {
		unique_lock<mutex> gaurd(amutex);
		return (*items)[index].getType();
	}

	json array::getJSON() {
		unique_lock<mutex> gaurd(amutex);
		json j = json::array();
		for (auto i = items->begin(); i != items->end(); ++i) {
			switch (i->getType()) {
				case typeNull:
					j.push_back(nullptr);
					break;
				case typeArray:
					j.push_back(i->getArray()->getJSON());
					break;
				case typeObject:
					j.push_back(i->getObject()->getJSON());
					break;
				case typeString:
					j.push_back(i->getString());
					break;
				case typeInt64:
					j.push_back(i->getInt64());
					break;
				case typeInt32:
					j.push_back(i->getInt32());
					break;
				case typeInt16:
					j.push_back(i->getInt16());
					break;
				case typeInt8:
					j.push_back(i->getInt8());
					break;
				case typeUInt64:
					j.push_back(i->getUInt64());
					break;
				case typeUInt32:
					j.push_back(i->getUInt32());
					break;
				case typeUInt16:
					j.push_back(i->getUInt16());
					break;
				case typeUInt8:
					j.push_back(i->getUInt8());
					break;
				case typeDouble:
					j.push_back(i->getDouble());
					break;
				case typeFloat:
					j.push_back(i->getFloat());
					break;
				case typeBool:
					j.push_back(i->getBool());
					break;
				default:
					break;
			}
		}
		return j;
	}

	vector<uint8_t> array::getBSON() {
		return json::to_bson(getJSON());
	}

	vector<uint8_t> array::getCBOR() {
		return json::to_cbor(getJSON());
	}

	vector<uint8_t> array::getMsgPack() {
		return json::to_msgpack(getJSON());
	}

	vector<uint8_t> array::getUBJSON() {
		return json::to_ubjson(getJSON());
	}

	array::avec::iterator array::begin() {
		unique_lock<mutex> gaurd(amutex);
		return items->begin();
	}

	array::avec::iterator array::end() {
		unique_lock<mutex> gaurd(amutex);
		return items->end();
	}

	void array::erase(array::avec::iterator i) {
		unique_lock<mutex> gaurd(amutex);
		items->erase(i);
	}

	array::avec::iterator array::find(var item) {
		unique_lock<mutex> gaurd(amutex);
		for (auto it = begin(); it != end(); ++it)
			if ((*it) == item) return it;
		return end();
	}

	array::avec::iterator array::find(
		var item, avec::iterator start) {
		unique_lock<mutex> gaurd(amutex);
		for (auto it = start; it != end(); ++it)
			if ((*it) == item) return it;
		return end();
	}

	array& array::operator+=(var item) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(item);
		return *this;
	}

	array& array::operator-=(var item) {
		unique_lock<mutex> gaurd(amutex);
		for (auto it = begin(); it != end(); ++it) {
			if (it->getPtr() == item.getPtr()) {
				erase(it);
				break;
			}
		}
		return *this;
	}

	void array::pushString(char* value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushInt64(int64_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushInt32(int32_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushInt16(int16_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushInt8(int8_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushUInt64(uint64_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushUInt32(uint32_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushUInt16(uint16_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushUInt8(uint8_t value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushDouble(double value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushFloat(float value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushBool(bool value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushArray(array value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushObject(object value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushPtr(void* value) {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(value);
	}

	void array::pushNull() {
		unique_lock<mutex> gaurd(amutex);
		items->push_back(var());
	}

	void array::setString(uint64_t index, char* value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setInt64(uint64_t index, int64_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setInt32(uint64_t index, int32_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setInt16(uint64_t index, int16_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setInt8(uint64_t index, int8_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setUInt64(uint64_t index, uint64_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setUInt32(uint64_t index, uint32_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setUInt16(uint64_t index, uint16_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setUInt8(uint64_t index, uint8_t value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setDouble(uint64_t index, double value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setFloat(uint64_t index, float value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setBool(uint64_t index, bool value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setArray(uint64_t index, array value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setObject(uint64_t index, object value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setPtr(uint64_t index, void* value) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var(value);
	}

	void array::setNull(uint64_t index) {
		unique_lock<mutex> gaurd(amutex);
		if (items->size() <= index) items->resize(index);
		(*items)[index] = var();
	}

	const char* array::getString(uint64_t index, char* def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeString) return (const char*)item;
		return def;
	}

	int64_t array::getInt64(uint64_t index, int64_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeInt64) return (int64_t)item;
		return def;
	}

	int32_t array::getInt32(uint64_t index, int32_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeInt32) return (int32_t)item;
		return def;
	}

	int16_t array::getInt16(uint64_t index, int16_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeInt16) return (int16_t)item;
		return def;
	}

	int8_t array::getInt8(uint64_t index, int8_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeInt8) return (int8_t)item;
		return def;
	}

	uint64_t array::getUInt64(uint64_t index, uint64_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeUInt64) return (uint64_t)item;
		return def;
	}

	uint32_t array::getUInt32(uint64_t index, uint32_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeUInt32) return (uint32_t)item;
		return def;
	}

	uint16_t array::getUInt16(uint64_t index, uint16_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeUInt16) return (uint16_t)item;
		return def;
	}

	uint8_t array::getUInt8(uint64_t index, uint8_t def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeUInt8) return (uint8_t)item;
		return def;
	}

	double array::getDouble(uint64_t index, double def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeDouble) return (double)item;
		return def;
	}

	float array::getFloat(uint64_t index, float def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeFloat) return (float)item;
		return def;
	}

	bool array::getBool(uint64_t index, bool def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeBool) return (bool)item;
		return def;
	}

	array* array::getArray(uint64_t index, array* def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeArray) return (array*)item;
		return def;
	}

	object* array::getObject(uint64_t index, object* def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typeObject) return (object*)item;
		return def;
	}

	void* array::getPtr(uint64_t index, void* def) {
		unique_lock<mutex> gaurd(amutex);
		if (index >= items->size()) return def;
		auto item = (*items)[index];
		if (item.getType() == typePtr) return (void*)item;
		return def;
	}
}  // namespace red