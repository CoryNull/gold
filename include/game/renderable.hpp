#pragma once

#include "component.hpp"
#include "types.hpp"

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

		// args: uint16_t scene, camera, list lights, envMap,
		// occlusionQuery
		var draw(list args);
		// args: uint16_t scene, light, shaderProgram
		var drawFromLight(list args);
		// args: uint16_t scene, envMap, list lights
		var drawFromEnvMap(list args);
	};
}  // namespace gold