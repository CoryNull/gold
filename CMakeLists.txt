cmake_minimum_required(VERSION 3.10)

# set the project name
project(RED2D)

find_package (libbson-1.0 1.7 REQUIRED)
find_package (SDL2 REQUIRED)

include_directories(include)

# add the executable
add_executable(RED2D
	src/array.cpp
	src/engine.cpp
	src/main.cpp
	src/memory.cpp
	src/object.cpp
	src/var.cpp
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

target_include_directories(
	RED2D PUBLIC
		"include"
	)

target_link_libraries (RED2D PRIVATE 
	${BSON_LIBRARY} 
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