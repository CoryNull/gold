#pragma once

#include "texture.hpp"
#include "types.hpp"
#include "window.hpp"

namespace gold {
	struct backend : public object {
	 protected:
		static object& getPrototype();

	 public:
		backend();
		backend(object config);

		var destroy(list args = {});
		var initialize(list args = {});
		var preFrame(list args = {});
		var renderFrame(list args = {});
		var getConfig(list args = {});
	};
}  // namespace gold
