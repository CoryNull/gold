#include "renderable.hpp"

#include "component.hpp"

namespace gold {

	obj& renderable::getPrototype() {
		static auto proto = obj({
			{"draw", method(&renderable::draw)},
			{"proto", component::getPrototype()},
		});
		return proto;
	}

	var renderable::draw(list) {
		return genericError("Needs to overriden");
	}

}  // namespace gold
