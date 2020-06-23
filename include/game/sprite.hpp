#pragma once

#include "renderable.hpp"

namespace gold {
	struct sprite : public renderable {
	 protected:
		void updateVertexBuffer();

	 public:
		static object& getPrototype();
		sprite();
		sprite(object config);

		var setSize(list args);
		var setArea(list args);
		var setOffset(list args);
		var draw(list args = {});
		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold