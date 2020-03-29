#pragma once

#include "object.hpp"

namespace red {
	class window : public object {
	 protected:
		static object proto;

		static var setSize(object& self, var& args);
		static var setPos(object& self, var& args);
		static var setTitle(object& self, var& args);
		static var setFullscreen(object& self, var& args);
		static var setBorderless(object& self, var& args);
		static var create(object& self, var& args);
		static var destroy(object& self, var& args);
		static var handleEvent(object& self, var& args);
		static var getConfig(object& self, var& args);

	 public:
		window();
		window(object& config);

	};
}  // namespace red
