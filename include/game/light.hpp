#pragma once

#include "component.hpp"

namespace gold {
	struct light : public component {
	 public:
		static object& getPrototype();
		light();
		light(object config);

		var setColor(list args);
		var getColor(list args = {});
		var setIntensity(list args);
		var getIntensity(list args = {});
		var setCone(list args);
		var getCone(list args = {});
		var setType(list args);
		var getType(list args = {});

		var getArea(list args = {});

		var drawScene(list args);
	};
}  // namespace gold