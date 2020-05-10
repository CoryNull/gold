cmake_minimum_required( VERSION 3.0 )

project(gold)

add_subdirectory(json)
add_subdirectory(bullet3)

include(cmake/uSockets.cmake)
include(cmake/uWebSockets.cmake)
include(cmake/bgfxConfig.cmake)
