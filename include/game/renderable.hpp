#pragma once

#include "types.hpp"
#include "component.hpp"

namespace gold {

	enum shaderType {
		NullShaderType = 0,
		VertexShaderType,
		FragmentShaderType,
		ComputeShaderType,
	};

	struct renderable : public component {
	 public:
		static object& getPrototype();

		var draw(list args = {});
	};
}  // namespace gold