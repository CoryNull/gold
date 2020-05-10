#include "entity.hpp"

#include "component.hpp"
#include "transform.hpp"

namespace gold {
	object& entity::getPrototype() {
		static auto proto = object({
			{"name", "New Object"},
			{"enabled", true},
			{"initialize", method(&entity::initialize)},
			{"add", method(&entity::add)},
			{"remove", method(&entity::remove)},
			{"enable", method(&entity::enable)},
			{"disable", method(&entity::disable)},
		});
		return proto;
	}

	transform entity::getTransform() {
		return getComponent({transform::getPrototype()})
			.getObject<transform>();
	}

	var entity::add(list args) {
		auto comps = getList("components");
		auto children = getList("children");
		if (!comps) return var();
		if (!children) return var();
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto obj = object();
			if (it->isObject(getPrototype())) {
				it->returnObject(obj);
				obj.setObject("parent", *this);
				children.pushObject(obj);
			} else if (it->isObject(component::getPrototype())) {
				it->returnObject(obj);
				obj.setObject("object", *this);
				comps.pushObject(obj);
			}
		}
		return var();
	}

	var entity::remove(list args) {
		auto comps = getList("components");
		auto children = getList("children");
		if (!comps) return var();
		if (!children) return var();
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto obj = object();
			if (it->isObject(getPrototype())) {
				it->returnObject(obj);
				auto it = children.find(obj);
				if (it != children.end()) children.erase(it);
			} else if (it->isObject(component::getPrototype())) {
				it->returnObject(obj);
				auto it = comps.find(obj);
				if (it != comps.end()) comps.erase(it);
			}
		}

		return var();
	}

	var entity::initialize(list) {
		if (getType("components") != typeList) {
			auto comps = list({transform(obj{
				{"object", *this},
			})});
			setList("components", comps);
		}
		if (getType("children") != typeList) {
			auto children = list({});
			setList("children", children);
		}
		if (getType("enabled") != typeBool)
			setBool("enabled", true);
		return var();
	}

	var entity::enable(list) {
		if (!getBool("enabled")) {
			setBool("enabled", true);
			// Do something
		}
		return var();
	}

	var entity::disable(list) {
		if (getBool("enabled")) {
			setBool("enabled", false);
			// Do something
		}
		return var();
	}

	var entity::getComponent(list args) {
		auto proto = args[0].getObject();
		auto comps = getList("components");
		for (auto it = comps.begin(); it != comps.end(); ++it)
			if (it->isObject(proto)) return *it;
		return var();
	}

	var entity::getComponents(list args) {
		auto proto = args[0].getObject();
		auto comps = getList("components");
		auto ret = list({});
		for (auto it = comps.begin(); it != comps.end(); ++it)
			if (it->isObject(proto)) ret.pushVar(*it);
		return ret;
	}

	var entity::getComponentsRecursive(list args) {
		auto proto = args[0].getObject();
		auto comps = getList("components");
		auto children = getList("children");
		auto ret = list({});
		for (auto it = comps.begin(); it != comps.end(); ++it)
			if (it->isObject(proto)) ret.pushVar(*it);
		for (auto it = children.begin(); it != children.end();
				 ++it) {
			auto child = it->getObject<entity>();
			if (child) {
				auto childComps = child.getComponentsRecursive(args);
				ret += childComps.getList();
			}
		}
		return ret;
	}

	entity::entity() : object() {}

	entity::entity(object config) : object(config) {
		setParent(getPrototype());
		initialize();
	}

	entity& entity::operator+=(list args) {
		add(args);
		return *this;
	}

	entity& entity::operator-=(list args) {
		remove(args);
		return *this;
	}
}  // namespace gold