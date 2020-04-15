#pragma once

#include "object.hpp"

namespace gold {
	class window : public object {
	 protected:
		static object proto;

	 public:
		window();
		window(object config);

		var setSize(varList args);
		var setPos(varList args);
		var setTitle(varList args);
		var setFullscreen(varList args);
		var setBorderless(varList args);
		var create(varList args = {});
		var destroy(varList args = {});
		var handleEvent(varList args);
		var getConfig(varList args = {});
	};
}  // namespace gold
