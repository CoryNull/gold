cmake_minimum_required(VERSION 3.10)

# set the project name
project(RED2D)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")

find_package (SDL2 REQUIRED)
find_package (glm 0.9.9 REQUIRED)
add_subdirectory(3rdParty/json)

include_directories(include)
unset(_MSC_VER CACHE)

# add the executable
add_executable(RED2D
	src/array.cpp
	src/component.cpp
	src/engine.cpp
	src/entity.cpp
	src/graphics.cpp
	src/main.cpp
	src/mesh.cpp
	src/object.cpp
	src/renderable.cpp
	src/sprite.cpp
	src/types.cpp
	src/var.cpp
	src/window.cpp
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

target_include_directories(
	RED2D 
	PUBLIC
		"include"
	PRIVATE
		${GLM_INCLUDE_DIRS}
	)

target_link_libraries (RED2D PRIVATE 
	nlohmann_json::nlohmann_json
	${SDL2_LIBRARIES}
)

target_compile_features(RED2D
		PUBLIC
				cxx_variadic_templates
				cxx_nullptr
				cxx_generic_lambdas
				cxx_lambdas
				cxx_auto_type
				cxx_variable_templates
				cxx_variadic_macros
				cxx_template_template_parameters
		PRIVATE
				cxx_std_17
)