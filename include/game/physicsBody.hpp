#pragma once

#include "types.hpp"
#include "component.hpp"

namespace gold {
	struct physicsBody : public component {
	 protected:
		static object& getPrototype();
		friend struct world;

	 public:
		physicsBody();
		physicsBody(object config);

		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold