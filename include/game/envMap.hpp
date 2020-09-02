#pragma once

#include "component.hpp"

namespace gold {
	struct envMap : public component {
	 public:
		static object& getPrototype();
		envMap();
		envMap(object config);

		var getSize(list args = {});
	};
}  // namespace gold