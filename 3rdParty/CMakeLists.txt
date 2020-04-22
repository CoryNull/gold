cmake_minimum_required( VERSION 3.0 )

project(3rdParty)

add_subdirectory(json)

include(cmake/uSockets.cmake)
include(cmake/uWebSockets.cmake)
include(cmake/bgfxConfig.cmake)
