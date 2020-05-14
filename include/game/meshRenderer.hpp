#pragma once

#include "renderable.hpp"

namespace gold {
	struct meshRenderer : public renderable {
	 protected:
		static object& getPrototype();
		
		void setMaterial(object matData);

	 public:
		meshRenderer();
		meshRenderer(object config);

		var draw(list args = {});
		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold