#pragma once

#include "types.hpp"
#include "component.hpp"

namespace gold {
	struct transform : public component {
	 protected:
		static object& getPrototype();

	 public:
		transform();
		transform(object config);

		//Low level
		void getMatrix(float* results);
		void getWorldMatrix(float* results);

		var setPosition(list args);
		var setRotation(list args);
		var setScale(list args);
		var getPosition(list args = {});
		var getRotation(list args = {});
		var getScale(list args = {});
		var reset(list args = {});
	};
}  // namespace gold