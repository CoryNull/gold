
cmake_minimum_required(VERSION 3.10)

project(gold)

add_library(
	goldShared
	STATIC
		src/list.cpp
		src/file.cpp
		src/object.cpp
		src/types.cpp
		src/var.cpp
		src/promise.cpp
)
add_library(
	gold::shared ALIAS goldShared
)

if(MSVC)
  target_compile_options(goldShared PRIVATE /W4 /WX)
else()
  target_compile_options(goldShared PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(
	goldShared
	PUBLIC
		"include"
		3rdParty/uWebSockets/src
)

target_link_libraries (
	goldShared
	PUBLIC 
		nlohmann_json::nlohmann_json
		stdc++fs
)

target_compile_features(
	goldShared
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