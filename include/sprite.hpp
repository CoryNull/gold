#pragma once

#include "types.hpp"
#include "renderable.hpp"

namespace gold {
	struct sprite : public renderable {
	 protected:
		static object& getPrototype();

	 public:
		sprite();
		sprite(object config);

		var draw(list args = {});
		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold