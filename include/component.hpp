#pragma once

#include "types.hpp"

namespace gold {
	struct component : public object {
	 protected:
		var update(list args);
		var draw(list args);

	 public:
		static object& getPrototype();
		component();
		component(object config);
	};
}  // namespace gold