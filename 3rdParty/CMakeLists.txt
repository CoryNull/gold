cmake_minimum_required( VERSION 3.0 )

project(gold)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")

add_subdirectory(json)
add_subdirectory(bullet3)

include(cmake/brtshaderc.cmake)
include(cmake/uSockets.cmake)
include(cmake/uWebSockets.cmake)
include(cmake/bgfxConfig.cmake)
