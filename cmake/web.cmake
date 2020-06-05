
cmake_minimum_required(VERSION 3.10)

project(gold CXX)

find_package (libmongoc-1.0 1.9.2 REQUIRED)

add_library(
	goldWeb
	STATIC
		src/web/database.cpp
		src/web/html.cpp
		src/web/server.cpp
)
add_library(
	gold::web ALIAS goldWeb
)

if(MSVC)
  target_compile_options(goldWeb PRIVATE /W4 /WX)
else()
  target_compile_options(goldWeb PRIVATE -pedantic)
endif()

target_include_directories(
	goldWeb
	PUBLIC
		"include"
		"include/web"
		${MONGOC_INCLUDE_DIRS}
		3rdParty/uWebSockets/src
)

target_link_libraries (
	goldWeb
	PUBLIC 
		gold::shared
		${MONGOC_LIBRARIES}
		uWebSockets
)

target_compile_features(
	goldWeb
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