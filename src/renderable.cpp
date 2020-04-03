#include "renderable.hpp"

#include "component.hpp"

namespace red {

	object renderable::proto =
		object({{"draw", renderable::draw}}, &component::proto);

	var renderable::draw(object& self, var args) {
		return logic_error("Needs to overriden");
	}

	renderable::renderable() : object(proto) {}

	renderable::renderable(object config)
		: object(config, &proto) {}
}  // namespace red
