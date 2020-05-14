#pragma once

#include "entity.hpp"

namespace gold {
	struct camera : public entity {
	 protected:
	 	friend struct engine;
		static object& getPrototype();

	 public:

		var setViewId(list args);
		var setViewClear(list args);
		var setViewSize(list args);
		var setViewOffset(list args);
		var setViewTransform(list args = {});
		var setView(list args = {});

		camera();
		camera(object config);
	};
}  // namespace gold