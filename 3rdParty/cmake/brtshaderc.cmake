# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

add_library(brtshaderc 
	SHARED
	brtshaderc/tools/brtshaderc/brtshaderc.cpp
	${BGFX_DIR}/tools/shaderc/shaderc.h 
	${BGFX_DIR}/tools/shaderc/shaderc_glsl.cpp 
	${BGFX_DIR}/tools/shaderc/shaderc_hlsl.cpp 
	${BGFX_DIR}/tools/shaderc/shaderc_pssl.cpp 
	${BGFX_DIR}/tools/shaderc/shaderc_spirv.cpp 
	${BGFX_DIR}/tools/shaderc/shaderc_metal.cpp
)

target_include_directories(
	brtshaderc
	PUBLIC
		brtshaderc/tools/brtshaderc/
		/${BGFX_DIR}/include/
		/${BGFX_DIR}/3rdparty/webgpu/include/
)

target_link_libraries (brtshaderc 
	PUBLIC
		bx
		fcpp
		glsl-optimizer
		glslang
		spirv-cross
		spirv-tools
)