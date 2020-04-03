#pragma once

#include "component.hpp"

namespace red {
	class transform : public component {
	 protected:
		static object proto;

		static var initialize(object& self, var args);
		static var setPosition(object& self, var args);
		static var setRotation(object& self, var args);
		static var setScale(object& self, var args);
		static var getPosition(object& self, var args);
		static var getRotation(object& self, var args);
		static var getScale(object& self, var args);
		static var getMatrix(object& self, var args);
		static var reset(object& self, var args);

	 public:
		transform();
		transform(object config);
	};
}  // namespace red