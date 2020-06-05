#pragma once

#include "entity.hpp"

namespace gold {
	struct camera : public entity {
	 protected:
		friend struct engine;
		static object& getPrototype();

	 public:
		// args: uint16? flags, uint32? rgba, uint8? stencil,
		// [float|double]? depth
		var setViewClear(list args);
		// args: list, vec2
		var setViewSize(list args);
		// args: list, vec2
		var setViewOffset(list args);

		// args: uint16 viewId
		var setViewTransform(list args = {});
		// args: uint16 viewId
		var setView(list args = {});

		camera();
		camera(object config);
	};
}  // namespace gold