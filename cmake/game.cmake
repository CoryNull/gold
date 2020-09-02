
cmake_minimum_required(VERSION 3.10)

project(gold CXX)

add_library(
	goldGame
	STATIC
		shaderSprite.hpp
		src/game/boxShape.cpp
		src/game/camera.cpp
		src/game/component.cpp
		src/game/engine.cpp
		src/game/entity.cpp
		src/game/envMap.cpp
		src/game/graphics.cpp
		src/game/light.cpp
		src/game/mesh.cpp
		src/game/meshRenderer.cpp
		src/game/meshShape.cpp
		src/game/physicsBody.cpp
		src/game/renderable.cpp
		src/game/shape.cpp
		src/game/sphereShape.cpp
		src/game/sprite.cpp
		src/game/transform.cpp
		src/game/window.cpp
		src/game/world.cpp
)
add_dependencies(goldGame Shaders)
add_library(
	gold::game ALIAS goldGame
)

if(MSVC)
  target_compile_options(goldGame PRIVATE /W4)
else()
  target_compile_options(goldGame PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(
	goldGame
	PUBLIC
		"include"
		"include/game"
		${CMAKE_CURRENT_BINARY_DIR}
		3rdParty/bullet3/src
)
target_link_libraries (
	goldGame 
	PUBLIC 
		gold::shared
		brtshaderc
		bgfx
		Bullet3Common
		BulletSoftBody 
		BulletDynamics 
		BulletCollision 
		BulletInverseDynamicsUtils 
		BulletInverseDynamics 
		LinearMath
		SDL2-static
		${OPENGL_LIBRARIES}
)
target_link_directories(goldGame PUBLIC ${LIBRARY_OUTPUT_DIRECTORY})

target_compile_features(
	goldGame
	PUBLIC
		cxx_variadic_templates
		cxx_nullptr
		cxx_generic_lambdas
		cxx_lambdas
		cxx_auto_type
		cxx_variable_templates
		cxx_variadic_macros
		cxx_template_template_parameters
		cxx_std_20
)