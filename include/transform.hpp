#pragma once

#include "component.hpp"

namespace gold {
	class transform : public component {
	 protected:
		static object proto;

	 public:
		transform();
		transform(object config);

		var initialize(varList args = {});
		var setPosition(varList args);
		var setRotation(varList args);
		var setScale(varList args);
		var getPosition(varList args = {});
		var getRotation(varList args = {});
		var getScale(varList args = {});
		var getMatrix(varList args = {});
		var reset(varList args = {});
	};
}  // namespace gold