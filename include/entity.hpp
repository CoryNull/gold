#pragma once

#include "types.hpp"
#include "transform.hpp"

namespace gold {
	struct entity : public object {
	 protected:
		friend struct engine;
		friend struct camera;
		static object& getPrototype();

	 public:
		entity();
		entity(object config);

		transform getTransform();

		var initialize(list args = {});
		var add(list args);
		var remove(list args);
		var enable(list args);
		var disable(list args);
		var getComponent(list args);
		var getComponents(list args);
		var getComponentsRecursive(list args);

		entity& operator+=(list args);
		entity& operator-=(list args);
	};
}  // namespace gold