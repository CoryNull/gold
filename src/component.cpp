#include "component.hpp"

#include <functional>

#include "types.hpp"

namespace gold {
	obj& component::getPrototype() {
		static auto proto = obj({
			{"draw", method(&component::draw)},
			{"update", method(&component::update)},
		});
		return proto;
	}

	var component::draw(list) { return var(); }

	var component::update(list) { return var(); }

	component::component() : obj() {
		setParent(getPrototype());
	}

	component::component(obj config) : obj() {
		copy(config);
		setParent(getPrototype());
	}
}  // namespace gold