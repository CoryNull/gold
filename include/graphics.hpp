#pragma once

#include "object.hpp"
#include "window.hpp"

namespace gold {
	class backend : public object {
	 protected:
		static object proto;

	 public:
		backend();
		backend(object config);

		var destroy(varList args = {});
		var initialize(varList args = {});
		var preFrame(varList args = {});
		var renderFrame(varList args = {});
		var getConfig(varList args = {});
	};
}  // namespace gold
