#include "component.hpp"

#include <functional>

#include "object.hpp"
#include "types.hpp"

namespace gold {
	object component::proto = object({
		{"draw", method(&component::draw)},
		{"update", method(&component::update)},
	});

	var component::draw(varList) { return var(); }

	var component::update(varList) { return var(); }

	component::component() : object(proto) {}

	component::component(object config)
		: object(config, &proto) {}
}  // namespace gold