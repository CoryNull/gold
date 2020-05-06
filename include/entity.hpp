#pragma once

#include "types.hpp"

namespace gold {
	struct entity : public object {
	 protected:
		friend struct engine;
		static object& getPrototype();

	 public:
		entity();
		entity(initList config);

		var initialize(list args = {});
		var add(list args);
		var remove(list args);
		var enable(list args);
		var disable(list args);

		entity& operator+=(list args);
		entity& operator-=(list args);
	};
}  // namespace gold