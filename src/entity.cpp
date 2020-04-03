#include "entity.hpp"

#include "array.hpp"
#include "component.hpp"

namespace red {
	object entity::proto =
		object({{"id", var()},
						{"name", "New Object"},
						{"initialize", entity::initialize},
						{"add", entity::add},
						{"remove", entity::remove},
						{"enable", entity::enable},
						{"disable", entity::disable}});

	var entity::add(object& self, var args) {
		auto comps = self.getArray("components");
		auto children = self.getArray("children");
		if (!comps) return var();
		if (!children) return var();
		if (args.isObject(proto))
			children->pushObject(*args.getObject());
		if (args.isObject(component::proto))
			comps->pushObject(*args.getObject());
	}

	var entity::remove(object& self, var args) {
		auto comps = self.getArray("components");
		auto children = self.getArray("children");
		if (!comps) return var();
		if (!children) return var();
		if (args.isObject(proto)) {
			auto obj = args.getObject();
			auto it = children->find(obj);
			if (it != children->end()) children->erase(it);
		}
		if (args.isObject(component::proto)) {
			auto obj = args.getObject();
			auto it = comps->find(obj);
			if (it != comps->end()) comps->erase(it);
		}
	}

	var entity::initialize(object& self, var args) {
		if (self.getArray("components") == nullptr)
			self.setArray("components", array());
		if (self.getArray("children") == nullptr)
			self.setArray("children", array());
		if (self.getType("enabled") != typeBool)
			self.setBool("enabled", true);
	}

	var entity::enable(object& self, var args) {
		if (!self.getBool("enabled")) {
			self.setBool("enabled", true);
			// Do something
		}
	}

	var entity::disable(object& self, var args) {
		if (self.getBool("enabled")) {
			self.setBool("enabled", false);
			// Do something
		}
	}

	entity::entity() : object(proto) {
		this->callMethod("initialize");
	}

	entity::entity(object config) : object(config, &proto) {
		this->callMethod("initialize");
	}

	entity& entity::operator+=(var args) {
		this->callMethod("add", args);
		return *this;
	}

	entity& entity::operator-=(var args) {
		this->callMethod("remove", args);
		return *this;
	}
}  // namespace red