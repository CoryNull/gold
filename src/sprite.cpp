#include "sprite.hpp"

namespace red {

	object sprite::proto =
		object({{"draw", sprite::draw}}, &renderable::proto);

	var sprite::draw(object& self, var args) {
		return logic_error("Needs to overriden");
	}

	sprite::sprite() : renderable(proto) {}

	sprite::sprite(object config) : renderable(config) {
		setParent(&proto);
	}
}  // namespace red