
cmake_minimum_required(VERSION 3.10)

project(gold)

find_package (SDL2 REQUIRED)
find_package (glm 0.9.9 REQUIRED)

add_library(
	goldGame
	STATIC
		shaderSprite.hpp
		src/component.cpp
		src/engine.cpp
		src/entity.cpp
		src/graphics.cpp
		src/mesh.cpp
		src/renderable.cpp
		src/sprite.cpp
		src/texture.cpp
		src/transform.cpp
		src/window.cpp
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
		${GLM_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries (
	goldGame 
	PUBLIC 
		gold::shared
		bgfx
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