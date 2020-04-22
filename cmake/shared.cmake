
cmake_minimum_required(VERSION 3.10)

add_library(
	shared
	STATIC
		src/array.cpp
		src/object.cpp
		src/types.cpp
		src/var.cpp
)
add_library(
	gold::shared ALIAS shared
)

if(MSVC)
  target_compile_options(shared PRIVATE /W4 /WX)
else()
  target_compile_options(shared PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(
	shared
	PUBLIC
		"include"
		${GLM_INCLUDE_DIRS}
		${MONGOC_INCLUDE_DIRS}
		3rdParty/uWebSockets/src
)

target_link_libraries (
	shared
	PUBLIC 
		nlohmann_json::nlohmann_json
)

target_compile_features(
	shared
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