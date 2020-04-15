cmake_minimum_required(VERSION 3.10)

project(gold)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BGFX_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BGFX_CONFIG_DEBUG OFF CACHE INTERNAL "")
set(ENTRY_CONFIG_USE_SDL ON CACHE INTERNAL "")
set(BGFX_OPENGL_VERSION "46" CACHE INTERNAL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package (SDL2 REQUIRED)
find_package (glm 0.9.9 REQUIRED)
find_package (libmongoc-1.0 1.9.2 REQUIRED)
add_subdirectory(3rdParty)

unset(_MSC_VER CACHE)

add_library(gold
	STATIC
		src/array.cpp
		src/component.cpp
		src/database.cpp
		src/engine.cpp
		src/entity.cpp
		src/graphics.cpp
		src/html.cpp
		src/mesh.cpp
		src/object.cpp
		src/renderable.cpp
		src/server.cpp
		src/sprite.cpp
		src/texture.cpp
		src/transform.cpp
		src/types.cpp
		src/var.cpp
		src/window.cpp
		src/worker.cpp
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

target_include_directories(
	gold
	PUBLIC
		"include"
		${GLM_INCLUDE_DIRS}
		${MONGOC_INCLUDE_DIRS}
		3rdParty/cpp-httplib
)

target_link_libraries (gold 
	PUBLIC 
		${MONGOC_LIBRARIES}
		nlohmann_json::nlohmann_json
		bgfx
		${SDL2_LIBRARIES}
)

target_compile_features(gold
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