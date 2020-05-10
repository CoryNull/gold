#pragma once

#include "types.hpp"

namespace gold {
	struct component : public object {
	 protected:
	 public:
		static object& getPrototype();
		component();
		component(object config);
		var update(list args);
		var draw(list args);

	};
}  // namespace gold