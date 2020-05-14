#pragma once

#include "types.hpp"

namespace gold {
	struct window : public object {
	 protected:
		static object& getPrototype();

	 public:
		window();
		window(object config);

		var setSize(list args);
		var setPos(list args);
		var setTitle(list args);
		var setFullscreen(list args);
		var setBorderless(list args);
		var create(list args = {});
		var destroy(list args = {});
		var handleEvent(list args);
		var getConfig(list args = {});
	};
}  // namespace gold
