#pragma once

#include "component.hpp"

class btTransform;
class btVector3;
class btQuaternion;

namespace gold {
	struct transform : public component {
	 public:
		static object& getPrototype();
		transform();
		transform(object config);

		// Low level
		btVector3 getBtPosition();
		btQuaternion getBtRotation();
		btTransform getBtTransform();

		var getMatrix(list args = {});
		var getWorldMatrix(list args = {});
		var relative(list args);
		var setPosition(list args);
		var setRotation(list args);
		var setAxisRotation(list args);
		var setScale(list args);
		var getPosition();
		var getRotation();
		var getEuler();
		var getScale();
		var reset(list args = {});
	};
}  // namespace gold