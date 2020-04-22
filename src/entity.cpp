#include "entity.hpp"

#include "array.hpp"
#include "component.hpp"

namespace gold {
	object entity::proto = object({
		{"name", "New Object"},
		{"enabled", true},
		{"initialize", method(&entity::initialize)},
		{"add", method(&entity::add)},
		{"remove", method(&entity::remove)},
		{"enable", method(&entity::enable)},
		{"disable", method(&entity::disable)},
	});

	var entity::add(varList args) {
		auto comps = getArray("components");
		auto children = getArray("children");
		if (!comps) return var();
		if (!children) return var();
		for (auto it = args.begin(); it != args.end(); ++it) {
			if (it->isObject(proto))
				children->pushObject(*it->getObject());
			else if (it->isObject(component::proto))
				comps->pushObject(*it->getObject());
		}
		return var();
	}

	var entity::remove(varList args) {
		auto comps = getArray("components");
		auto children = getArray("children");
		if (!comps) return var();
		if (!children) return var();
		for (auto it = args.begin(); it != args.end(); ++it) {
			if (it->isObject(proto)) {
				auto obj = it->getObject();
				auto it = children->find(*obj);
				if (it != children->end()) children->erase(it);
			} else if (it->isObject(component::proto)) {
				auto obj = it->getObject();
				auto it = comps->find(*obj);
				if (it != comps->end()) comps->erase(it);
			}
		}

		return var();
	}

	var entity::initialize(varList) {
		if (getArray("components") == nullptr)
			setArray("components", array());
		if (getArray("children") == nullptr)
			setArray("children", array());
		if (getType("enabled") != typeBool)
			setBool("enabled", true);
		return var();
	}

	var entity::enable(varList) {
		if (!getBool("enabled")) {
			setBool("enabled", true);
			// Do something
		}
		return var();
	}

	var entity::disable(varList) {
		if (getBool("enabled")) {
			setBool("enabled", false);
			// Do something
		}
		return var();
	}

	entity::entity() : object(proto) { initialize(); }

	entity::entity(object config) : object(config, &proto) {
		initialize();
	}

	entity& entity::operator+=(varList args) {
		add(args);
		return *this;
	}

	entity& entity::operator-=(varList args) {
		remove(args);
		return *this;
	}
}  // namespace gold