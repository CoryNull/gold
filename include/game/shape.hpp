#pragma once

#include "component.hpp"

namespace gold {
	struct shape : public component {
	 protected:
		static object& getPrototype();
		friend struct physicsBody;

	 public:
		shape();
		shape(object config);

		var initialize(list args = {});
		var destroy(list args = {});
	};
}