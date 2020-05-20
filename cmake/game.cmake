
cmake_minimum_required(VERSION 3.10)

project(gold)

find_package (SDL2 REQUIRED)
find_package (glm 0.9.9 REQUIRED)

add_library(
	goldGame
	STATIC
		shaderSprite.hpp
		src/game/boxShape.cpp
		src/game/camera.cpp
		src/game/component.cpp
		src/game/engine.cpp
		src/game/entity.cpp
		src/game/graphics.cpp
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
  target_compile_options(goldGame PRIVATE /W4 /WX)
else()
  target_compile_options(goldGame PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(
	goldGame
	PUBLIC
		"include"
		"include/game"
		${GLM_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
		3rdParty/bullet3/src
)

target_link_libraries (
	goldGame 
	PUBLIC 
		gold::shared
		bgfx
		Bullet3Common
		BulletSoftBody 
		BulletDynamics 
		BulletCollision 
		BulletInverseDynamicsUtils 
		BulletInverseDynamics 
		LinearMath
		${SDL2_LIBRARIES}
		${OPENGL_LIBRARIES}
)

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