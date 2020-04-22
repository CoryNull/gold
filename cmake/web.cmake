
cmake_minimum_required(VERSION 3.10)

find_package (libmongoc-1.0 1.9.2 REQUIRED)

add_library(
	web
	STATIC
		src/database.cpp
		src/html.cpp
		src/server.cpp
)
add_library(
	gold::web ALIAS web
)

if(MSVC)
  target_compile_options(web PRIVATE /W4 /WX)
else()
  target_compile_options(web PRIVATE -Wall -pedantic)
endif()

target_include_directories(
	web
	PUBLIC
		"include"
		${MONGOC_INCLUDE_DIRS}
		3rdParty/uWebSockets/src
)

target_link_libraries (
	web
	PUBLIC 
		shared
		${MONGOC_LIBRARIES}
		uWebSockets
)

target_compile_features(
	web
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