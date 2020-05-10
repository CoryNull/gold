
#include "types.hpp"

namespace gold {
	using value_t = nlohmann::detail::value_t;
	void list::initMemory() {
		if (!data)
			data = shared_ptr<arrData>(new arrData{avec(), mutex()});
	}

	list::list() : data(nullptr) {}

	list::list(const list& copy) : data(copy.data) {}

	list::list(json value) : data(new arrData{avec(), mutex()}) {
		if (value.is_array()) {
			for (auto it = value.begin(); it != value.end(); ++it) {
				auto name = std::stoul(it.key());
				auto value = it.value();
				switch (value.type()) {
					case value_t::null:
						this->setNull(name);
						break;
					case value_t::object: {
						auto obj = object(value);
						this->setObject(name, obj);
						break;
					}
					case value_t::array: {
						auto arr = list(value);
						this->setList(name, arr);
						break;
					}
					case value_t::string:
						this->setString(
							name, (char*)((string)value).c_str());
						break;
					case value_t::boolean:
						this->setBool(name, (bool)value);
						break;
					case value_t::number_integer:
						this->setInt64(name, (int64_t)value);
						break;
					case value_t::number_unsigned:
						this->setUInt64(name, (uint64_t)value);
						break;
					case value_t::number_float:
						this->setDouble(name, (double)value);
						break;
					default:
						break;
				}
			}
		}
	}

	list::list(initializer_list<var> list)
		: data(new arrData{avec(list), mutex()}) {}

	list::list(var value) {
		auto arr = value.getList();
		if (arr.data)
			data = shared_ptr<arrData>(
				new arrData{avec(arr.data->items), mutex()});
	}

	list::~list() {
		if (data && data.use_count() == 1) data->items.clear();
		data = nullptr;
	}

	uint64_t list::size() {
		if (!data) return 0;
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.size();
	}

	void list::pop() {
		if (!data) return;
		unique_lock<mutex> gaurd(data->amutex);
		data->items.pop_back();
	}

	types list::getType(uint64_t index) {
		if (!data) return typeNull;
		unique_lock<mutex> gaurd(data->amutex);
		return data->items[index].getType();
	}

	json list::getJSON() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		json j = json::array();
		for (auto i = data->items.begin(); i != data->items.end();
				 ++i) {
			switch (i->getType()) {
				case typeException:
					j.push_back(string(*i->getError()));
					break;
				case typeNull:
					j.push_back(nullptr);
					break;
				case typeList: {
					auto arr = i->getList();
					auto a = arr.getJSON();
					j.push_back(a);
					break;
				}
				case typeObject: {
					auto obj = i->getObject();
					auto o = obj.getJSON();
					j.push_back(o);
					break;
				}
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

	vector<uint8_t> list::getBSON() {
		return json::to_bson(getJSON());
	}

	vector<uint8_t> list::getCBOR() {
		return json::to_cbor(getJSON());
	}

	vector<uint8_t> list::getMsgPack() {
		return json::to_msgpack(getJSON());
	}

	vector<uint8_t> list::getUBJSON() {
		return json::to_ubjson(getJSON());
	}

	bool list::isAllFloating() const {
		if (!data) return false;
		bool isAllFloating = true;
		for (size_t i = 0; i < data->items.size(); ++i) {
			if (!data->items[i].isFloating()) {
				isAllFloating = false;
				break;
			}
		}
		return isAllFloating;
	}

	bool list::isAllNumber() const {
		if (!data) return false;
		bool isAllNumber = true;
		for (size_t i = 0; i < data->items.size(); ++i) {
			if (!data->items[i].isNumber()) {
				isAllNumber = false;
				break;
			}
		}
		return isAllNumber;
	}

	void list::assign(types t, void* target, size_t count) const {
		if (data) {
			auto size = min(data->items.size(), count);
			for (size_t i = 0; i < size; ++i) {
				switch (t) {
					case typeBool:
						((bool*)target)[i] = data->items[i].getBool();
						break;
					case typeUInt64:
						((uint64_t*)target)[i] = data->items[i].getUInt64();
						break;
					case typeUInt32:
						((uint32_t*)target)[i] = data->items[i].getUInt32();
						break;
					case typeUInt16:
						((uint16_t*)target)[i] = data->items[i].getUInt16();
						break;
					case typeUInt8:
						((uint8_t*)target)[i] = data->items[i].getUInt8();
						break;
					case typeInt64:
						((int64_t*)target)[i] = data->items[i].getInt64();
						break;
					case typeInt32:
						((int32_t*)target)[i] = data->items[i].getInt32();
						break;
					case typeInt16:
						((int16_t*)target)[i] = data->items[i].getInt16();
						break;
					case typeInt8:
						((int8_t*)target)[i] = data->items[i].getInt8();
						break;
					case typeFloat:
						((float*)target)[i] = data->items[i].getFloat();
						break;
					case typeDouble:
						((double*)target)[i] = data->items[i].getDouble();
						break;
					case typePtr:
						((void**)target)[i] = data->items[i].getPtr();
						break;
					case typeString:
						((string*)target)[i] = data->items[i].getString();
						break;
					case typeObject:
						((object*)target)[i] = data->items[i].getObject();
						break;
					case typeList:
						((list*)target)[i] = data->items[i].getList();
						break;
					default:
						break;
				}
			}
		}
	}

	list::avec::iterator list::begin() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.begin();
	}

	list::avec::iterator list::end() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.end();
	}

	list::avec::reverse_iterator list::rbegin() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.rbegin();
	}

	list::avec::reverse_iterator list::rend() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.rend();
	}

	list::avec::iterator list::erase(list::avec::iterator i) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		return data->items.erase(i);
	}

	list::avec::iterator list::find(object& proto) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = data->items.begin();
		for (; it != e; ++it)
			if ((*it).isObject(proto)) return it;
		return e;
	}

	list::avec::iterator list::find(
		object& proto, avec::iterator start) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = start;
		for (; it != e; ++it)
			if ((*it).isObject(proto)) return it;
		return e;
	}

	list::avec::iterator list::find(var item) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = data->items.begin();
		for (; it != e; ++it)
			if ((*it) == item) return it;
		return e;
	}

	list::avec::iterator list::find(
		var item, avec::iterator start) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = start;
		for (; it != e; ++it)
			if ((*it) == item) return it;
		return e;
	}

	list::avec::iterator list::find(
		types t, avec::iterator start) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = start;
		for (; it != e; ++it)
			if (it->getType() == t) return it;
		return e;
	}

	list::avec::iterator list::find(types t) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto e = data->items.end();
		auto it = data->items.begin();
		for (; it != e; ++it)
			if (it->getType() == t) return it;
		return e;
	}

	void list::resize(size_t newSize) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.resize(newSize);
	}

	list list::operator+=(list item) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto end = item.end();
		for (auto it = item.begin(); it != end; ++it)
			data->items.push_back(*it);
		return *this;
	}

	list list::operator+=(var item) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(item);
		return *this;
	}

	list list::operator-=(var item) {
		if (!data) return *this;
		unique_lock<mutex> gaurd(data->amutex);
		for (auto it = begin(); it != end(); ++it) {
			if (*it == item) {
				erase(it);
				break;
			}
		}
		return *this;
	}

	list::operator bool() const {
		return (this->data.operator bool());
	}

	var list::operator[](uint64_t index) const {
		if (!data) return var();
		auto it = data->items.begin();
		std::advance(it, index);
		if (it != data->items.end()) return *it;
		return var();
	}

	void list::pushString(char* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushString(const char* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushString(string value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushInt64(int64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushInt32(int32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushInt16(int16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushInt8(int8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushUInt64(uint64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushUInt32(uint32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushUInt16(uint16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushUInt8(uint8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushDouble(double value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushFloat(float value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushBool(bool value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushList(list value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushObject(object value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(var(value));
	}

	void list::pushMethod(method& value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(var(value));
	}

	void list::pushFunc(func value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(var(value));
	}

	void list::pushPtr(void* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(var(value, typePtr));
	}

	void list::pushVar(var value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(value);
	}

	void list::pushNull() {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		data->items.push_back(var());
	}

	void list::setString(uint64_t index, char* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setString(uint64_t index, string value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setInt64(uint64_t index, int64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setInt32(uint64_t index, int32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setInt16(uint64_t index, int16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setInt8(uint64_t index, int8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setUInt64(uint64_t index, uint64_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setUInt32(uint64_t index, uint32_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setUInt16(uint64_t index, uint16_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setUInt8(uint64_t index, uint8_t value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setDouble(uint64_t index, double value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setFloat(uint64_t index, float value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setBool(uint64_t index, bool value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setList(uint64_t index, list value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setObject(uint64_t index, object value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setFunc(uint64_t index, func& value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setMethod(uint64_t index, method& value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setPtr(uint64_t index, void* value) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var(value);
	}

	void list::setNull(uint64_t index) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (data->items.size() <= index) data->items.resize(index);
		data->items[index] = var();
	}

	const char* list::getString(uint64_t index, char* def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeString) return (const char*)item;
		return def;
	}

	string list::getString(uint64_t index, string def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeString) return (string)item;
		return def;
	}

	int64_t list::getInt64(uint64_t index, int64_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeInt64) return (int64_t)item;
		return def;
	}

	int32_t list::getInt32(uint64_t index, int32_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeInt32) return (int32_t)item;
		return def;
	}

	int16_t list::getInt16(uint64_t index, int16_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeInt16) return (int16_t)item;
		return def;
	}

	int8_t list::getInt8(uint64_t index, int8_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeInt8) return (int8_t)item;
		return def;
	}

	uint64_t list::getUInt64(uint64_t index, uint64_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeUInt64) return (uint64_t)item;
		return def;
	}

	uint32_t list::getUInt32(uint64_t index, uint32_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeUInt32) return (uint32_t)item;
		return def;
	}

	uint16_t list::getUInt16(uint64_t index, uint16_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeUInt16) return (uint16_t)item;
		return def;
	}

	uint8_t list::getUInt8(uint64_t index, uint8_t def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeUInt8) return (uint8_t)item;
		return def;
	}

	double list::getDouble(uint64_t index, double def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeDouble) return (double)item;
		return def;
	}

	float list::getFloat(uint64_t index, float def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeFloat) return (float)item;
		return def;
	}

	bool list::getBool(uint64_t index, bool def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeBool) return (bool)item;
		return def;
	}

	list list::getList(uint64_t index, list def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeList) return (list)item;
		return def;
	}

	void list::returnList(uint64_t index, list& result) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto item = data->items[index];
		if (item.getType() == typeList) result = list(item);
	}

	object list::getObject(uint64_t index, object def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeObject) return (object)item;
		return def;
	}

	method list::getMethod(uint64_t index, method def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeMethod) return (method)item;
		return def;
	}

	func list::getFunction(uint64_t index, func def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typeFunction) return (func)item;
		return def;
	}

	void list::returnObject(uint64_t index, object& result) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		auto it = data->items.at(index);
		if (it.getType() == typeObject) result = (object)it;
	}

	void* list::getPtr(uint64_t index, void* def) {
		initMemory();
		unique_lock<mutex> gaurd(data->amutex);
		if (index >= data->items.size()) return def;
		auto item = data->items[index];
		if (item.getType() == typePtr) return (void*)item;
		return def;
	}
}  // namespace gold