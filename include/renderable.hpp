#pragma once

#include "types.hpp"

namespace gold {

	struct renderable : public object {
	 public:
		static object& getPrototype();

		var draw(list args = {});
	};
}  // namespace gold