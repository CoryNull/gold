#pragma once

#include "renderable.hpp"
#include "camera.hpp"

namespace gold {
	struct meshRenderer : public renderable {
	 protected:
		static object& getPrototype();

		void setMaterial(camera cam, object primitive, object mesh);
		object configureVertex(object primitive, object mesh);
		object configureFragment(object primitive, object mesh);

	 public:
		meshRenderer();
		meshRenderer(object config);

		var draw(list args = {});
		var initialize(list args = {});
		var destroy(list args = {});
	};
}  // namespace gold