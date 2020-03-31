#include "renderable.hpp"

namespace red {

	object renderable::proto = object({{"draw", renderable::draw}});

	var renderable::draw(object& self, var args) { var(logic_error("Needs to overriden")); }

	renderable::renderable() : object(proto) {}

	renderable::renderable(object& config) : object(config, &proto) {}
}  // namespace red
