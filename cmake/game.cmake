
cmake_minimum_required(VERSION 3.10)

find_package (SDL2 REQUIRED)
find_package (glm 0.9.9 REQUIRED)

add_library(
	game
	STATIC
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
		src/worker.cpp
)
add_library(
	gold::game ALIAS game
)

if(MSVC)
  target_compile_options(game PRIVATE /W4 /WX)
else()
  target_compile_options(game PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(
	game
	PUBLIC
		"include"
		${GLM_INCLUDE_DIRS}
)

target_link_libraries (
	game 
	PUBLIC 
		shared
		bgfx
		${SDL2_LIBRARIES}
)

target_compile_features(
	game
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