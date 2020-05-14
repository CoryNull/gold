#pragma once

#include "types.hpp"

namespace gold {

	enum shaderType {
		NullShaderType = 0,
		VertexShaderType,
		FragmentShaderType,
		ComputeShaderType,
	};

	struct renderable : public object {
	 public:
		static object& getPrototype();

		var draw(list args = {});
	};
}  // namespace gold