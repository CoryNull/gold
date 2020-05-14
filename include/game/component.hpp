#pragma once

#include "types.hpp"

namespace gold {

	enum priorityEnum {
		// Needs to be set
		highPriority = 0,
		// Things that need loading
		dataPriority = 10,
		// Physics bodies and shaped loading
		physicsPriority = 100,
		// User space 1000->1000000
		genericPriority = 1000,
		// Reserved for renderables and are called last
		drawPriority = 1000000
	};

	struct component : public object {
	 protected:
	 public:
		static object& getPrototype();
		component();
		component(object config);
		var initialize();
		var update();
		var draw(list args);
	};
}  // namespace gold