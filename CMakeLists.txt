cmake_minimum_required(VERSION 3.10)

# set the project name
project(RED2D)

find_package (libbson-1.0 1.7 REQUIRED)
find_package (SDL2 REQUIRED)

include_directories(include)

# add the executable
add_executable(RED2D 
	src/engine.cpp
	src/main.cpp
	src/object.cpp
)

set_property(TARGET RED2D PROPERTY CPP_STANDARD 11)

target_include_directories(
	RED2D PUBLIC
		"include"
	)

target_link_libraries (RED2D PRIVATE 
	${BSON_LIBRARY} 
	${SDL2_LIBRARIES}
)