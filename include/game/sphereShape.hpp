#pragma once

#include "shape.hpp"

namespace gold {
	struct sphereShape : public shape {
	 protected:
		static object& getPrototype();
		friend struct physicsBody;

	 public:
		sphereShape();
		sphereShape(object config);

		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold