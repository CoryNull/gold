#pragma once

#include "object.hpp"
#include "window.hpp"

namespace red {
	class backend : public object {
	 protected:
		static object proto;

		static var destroy(object& self, var args);
		static var initialize(object& self, var args);
		static var preFrame(object& self, var args);
		static var renderFrame(object& self, var args);
		static var getConfig(object& self, var args);

	 public:
		backend();
		backend(object config);
	};
}  // namespace red
