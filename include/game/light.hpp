#pragma once

#include "component.hpp"

namespace gold {
	struct light : public component {
	 public:
		static object& getPrototype();
		light();
		light(object config);

		var setColor(list args);
		var getColor();
		var setIntensity(list args);
		var getIntensity();
		var setCone(list args);
		var getCone();
		var setType(list args);
		var getType();

		var getArea();

		var drawScene(list comps, uint16_t scene);
	};
}  // namespace gold