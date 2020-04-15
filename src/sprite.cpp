#include "sprite.hpp"

namespace gold {

	object sprite::proto = object(
		{
			{"draw", method(&sprite::draw)},
		},
		&renderable::proto);

	var sprite::draw(varList args) {
		return genericError("Needs to overriden");
	}

	sprite::sprite() : renderable(&proto) {}

	sprite::sprite(object config) : renderable(config, &proto) {}
}  // namespace gold