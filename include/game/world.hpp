#pragma once 

#include "types.hpp"

namespace gold{
	struct world : public object {
	protected:
		static object& getPrototype();
		friend struct engine;
		var registerComponent(list args);
	public:
		world();
		world(object config);

		var step(list args = {});
		var setGravity(list args);
		var raytrace(list args);
		var initialize(list args = {});
		var destroy(list args = {});
	};
}