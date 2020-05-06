
cmake_minimum_required(VERSION 3.10)

project(gold)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BGFX_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BGFX_CONFIG_DEBUG ON CACHE INTERNAL "")
set(ENTRY_CONFIG_USE_SDL ON CACHE INTERNAL "")
set(BGFX_OPENGL_VERSION "46" CACHE INTERNAL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(3rdParty)

unset(_MSC_VER CACHE)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

include(cmake/shaders.cmake)
include(cmake/shared.cmake)
include(cmake/game.cmake)
include(cmake/web.cmake)