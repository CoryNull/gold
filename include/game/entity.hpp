#pragma once

#include "transform.hpp"
#include "types.hpp"

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
		// args: [entity child, component comp], ...
		var add(list args);
		// args: [entity child, component comp], ...
		var remove(list args);
		var enable(list args);
		var disable(list args);
		// args: object prototype
		var getComponent(list args);
		// args: object prototype
		var getComponents(list args);
		// args: object prototype
		var getComponentsRecursive(list args);

		// args: [entity child, component comp], ...
		entity& operator+=(list args);
		// args: [entity child, component comp], ...
		entity& operator-=(list args);
	};
}  // namespace gold