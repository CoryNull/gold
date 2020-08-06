cmake_minimum_required(VERSION 3.10)

# set the project name
project(ScriptingExample)

include_directories(include)
add_subdirectory(3rdParty)

get_filename_component(
	BUILD_ROOT
	"${CMAKE_CURRENT_BINARY_DIR}/"
	ABSOLUTE
)

get_filename_component(
	ASSETS_ABS
	"./assets/"
	ABSOLUTE
)

get_filename_component(
	APP_ABS
	"./app/"
	ABSOLUTE
)

get_filename_component(
	JS_ABS
	"./js/"
	ABSOLUTE
)

get_filename_component(
	CSS_ABS
	"./css/"
	ABSOLUTE
)

if(NOT (IS_SYMLINK ${BUILD_ROOT}/assets))
	file(
		CREATE_LINK 
		${ASSETS_ABS} 
		${BUILD_ROOT}/assets
		SYMBOLIC
	)
endif()

if(NOT (IS_SYMLINK ${BUILD_ROOT}/app))
	file(
		CREATE_LINK 
		${APP_ABS} 
		${BUILD_ROOT}/app
		SYMBOLIC
	)
endif()

if(NOT (IS_SYMLINK ${BUILD_ROOT}/js/main))
	file(
		CREATE_LINK 
		${JS_ABS} 
		${BUILD_ROOT}/js/main
		SYMBOLIC
	)
endif()

if(NOT (IS_SYMLINK ${BUILD_ROOT}/css/main))
	file(
		CREATE_LINK 
		${CSS_ABS} 
		${BUILD_ROOT}/css/main
		SYMBOLIC
	)
endif()

FIND_LIBRARY(cryptopp crypto++ /usr/lib)

# add the executable
add_executable(
	ScriptingExample
	src/main.cpp
)

if(MSVC)
  target_compile_options(ScriptingExample PRIVATE /W4 /WX)
else()
  target_compile_options(ScriptingExample PRIVATE -Wall -pedantic)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

target_include_directories(
	ScriptingExample
	PUBLIC
		"include"
)

target_link_libraries(
	ScriptingExample 
	PRIVATE
		gold::web
		cryptopp
)

target_compile_features(
	ScriptingExample
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