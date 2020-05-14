#pragma once

#include "shape.hpp"

namespace gold {
	struct meshShape : public shape {
	 protected:
		static object& getPrototype();
		friend struct physicsBody;

	 public:
		meshShape();
		meshShape(object config);

		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold