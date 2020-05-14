#pragma once

#include "shape.hpp"

namespace gold {
	struct boxShape : public shape {
	 protected:
		static object& getPrototype();
		friend struct physicsBody;

	 public:
		boxShape();
		boxShape(object config);

		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold