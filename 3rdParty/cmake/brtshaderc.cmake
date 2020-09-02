# bgfx.cmake - bgfx building in cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

include( CMakeParseArguments )

add_library(brtshaderc 
	STATIC
	brtshaderc/tools/brtshaderc/brtshaderc.cpp
	brtshaderc/tools/brtshaderc/shaderc_spirv.cpp
	bgfx/tools/shaderc/shaderc.h 
	bgfx/tools/shaderc/shaderc_glsl.cpp 
	bgfx/tools/shaderc/shaderc_hlsl.cpp 
	bgfx/tools/shaderc/shaderc_pssl.cpp 
	bgfx/tools/shaderc/shaderc_metal.cpp)

target_include_directories(
	brtshaderc
	PRIVATE
		"bgfx/include"
	PUBLIC
		"brtshaderc/tools/brtshaderc/"
		"bgfx/tools/shaderc/"
		"bgfx/3rdparty/webgpu/include/"
		"bgfx/3rdparty/dxsdk/include/"
)

target_link_libraries (brtshaderc 
	PUBLIC
		bgfx
		bx
		fcpp
		glsl-optimizer
		glslang
		spirv-cross
		spirv-tools
)