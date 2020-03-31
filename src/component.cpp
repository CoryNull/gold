#include "component.hpp"

namespace red {
	object component::proto =
			object({{"draw", component::draw}, {"update", component::update}});

	var component::draw(object& self, var args) {}

	var component::update(object& self, var args) {}

	component::component() : object(proto) {}

	component::component(object& config) : object(config, &proto) {}
}  // namespace red