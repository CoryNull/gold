#include "renderable.hpp"

#include "component.hpp"

namespace gold {

	object renderable::proto = object(
		{
			{"draw", method(&renderable::draw)},
		},
		&component::proto);

	var renderable::draw(varList args) {
		return genericError("Needs to overriden");
	}

	renderable::renderable(object* parent) : object(parent) {}

	renderable::renderable(object data, object* parent)
		: object(data, parent) {}
}  // namespace gold
