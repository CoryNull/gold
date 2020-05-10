#include "sprite.hpp"

#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <bx/timer.h>

#include <vector>

#include "entity.hpp"
#include "shaderSprite.hpp"
#include "texture.hpp"
#include "transform.hpp"

namespace gold {
	using namespace bgfx;

	static ShaderHandle SpriteFragShader =
		ShaderHandle{kInvalidHandle};
	static ShaderHandle SpriteVertShader =
		ShaderHandle{kInvalidHandle};
	static ProgramHandle SpriteProgram =
		ProgramHandle{kInvalidHandle};

	struct PosColorVertex {
		typedef vector<PosColorVertex> vec;

		float x, y, z;
		float u, v;

		static VertexLayout& getLayout() {
			static VertexLayout layout = VertexLayout();
			if (layout.m_stride == 0) {
				layout.begin()
					.add(Attrib::Position, 3, AttribType::Float)
					.add(Attrib::TexCoord0, 2, AttribType::Float)
					.end();
			}
			return layout;
		}
	};

	obj& sprite::getPrototype() {
		static auto proto = obj({
			{"offset", vec2f(0.5, 0.5)},
			{"size", vec2f(1.0, 1.0)},
			{"area", vec4f(0.0, 0.0, 1.0, 1.0)},
			{"draw", method(&sprite::draw)},
			{"initialize", method(&sprite::initialize)},
			{"destroy", method(&sprite::destroy)},
			{"proto", renderable::getPrototype()},
		});
		return proto;
	}

	void sprite::updateVertexBuffer() {
		auto cvbh = VertexBufferHandle{
			getUInt16("vbh", bgfx::kInvalidHandle)};
		auto rebuild = getBool("rebuild") || !isValid(cvbh);
		if (rebuild && isValid(cvbh)) bgfx::destroy(cvbh);
		if (rebuild) {
			auto tex = getObject<texture>("texture");
			if (!tex) return;
			auto offset = getVar("offset");
			auto size = getVar("size");
			auto area = getVar("area");
			auto texWidth = tex.getFloat("width");
			auto texHeight = tex.getFloat("height");
			if (!area.isFloating() && area.isNumber()) {
				auto x = area.getFloat(0) / texWidth;
				auto y = area.getFloat(1) / texHeight;
				auto w = area.getFloat(2) / texWidth;
				auto h = area.getFloat(3) / texHeight;
				area = vec4f(x, y, w, h);
			}
			if (!size.isFloating() && size.isNumber()) {
				auto w =
					(texWidth * area.getFloat(2)) / size.getFloat(0);
				auto h =
					(texHeight * area.getFloat(3)) / size.getFloat(1);
				size = vec2f(w, h);
			}
			if (!offset.isFloating() && offset.isNumber()) {
				auto x = offset.getFloat(0) /
								 (texWidth * area.getFloat(2)) *
								 size.getFloat(0);
				auto y = -(offset.getFloat(1) /
									 (texHeight * area.getFloat(3))) *
								 size.getFloat(1);
				offset = vec2f(x, y);
			}
			auto width = size.getFloat(0);
			auto height = size.getFloat(1);

			auto tx0 = area.getFloat(0);
			auto ty0 = area.getFloat(1);
			auto tx1 = area.getFloat(0) + area.getFloat(2);
			auto ty1 = -area.getFloat(3) + area.getFloat(1);

			auto px0 = -offset.getFloat(0);
			auto py0 = -offset.getFloat(1);
			auto px1 = -offset.getFloat(0) + width;
			auto py1 = -offset.getFloat(1) + height;
			PosColorVertex verts[4] = {
				{px0, py1, 1, tx0, ty1},
				{px1, py1, 1, tx1, ty1},
				{px0, py0, 1, tx0, ty0},
				{px1, py0, 1, tx1, ty0},
			};
			auto vData =
				bgfx::copy(verts, sizeof(PosColorVertex) * 4);
			auto vbh =
				createVertexBuffer(vData, PosColorVertex::getLayout());
			if (isValid(vbh)) setUInt16("vbh", vbh.idx);
			erase("rebuild");
		}
	}

	var sprite::setSize(list args) {
		if (args[0].isVec2()) {
			setVar("size", args[0]);
		} else if (args.size() == 2) {
			if (args.isAllFloating())
				setVar(
					"size",
					vec2f(args[0].getFloat(), args[1].getFloat()));
			else if (args.isAllNumber())
				setVar(
					"size",
					vec2i32(args[0].getInt32(), args[1].getInt32()));
		}
		setBool("rebuild", true);
		return var();
	}

	var sprite::setArea(list args) {
		if (args[0].isVec4()) {
			setVar("area", args[0]);
		} else if (args.size() == 4) {
			if (args.isAllFloating())
				setVar(
					"area",
					vec4f(
						args[0].getFloat(),
						args[1].getFloat(),
						args[2].getFloat(),
						args[3].getFloat()));
			else if (args.isAllNumber())
				setVar(
					"area",
					vec4i32(
						args[0].getInt32(),
						args[1].getInt32(),
						args[2].getInt32(),
						args[3].getInt32()));
		}
		setBool("rebuild", true);
		return var();
	}

	var sprite::setOffset(list args) {
		if (args[0].isVec2()) {
			setVar("offset", args[0]);
		} else if (args.size() == 2) {
			if (args.isAllFloating())
				setVar(
					"offset",
					vec2f(args[0].getFloat(), args[1].getFloat()));
			else if (args.isAllNumber())
				setVar(
					"offset",
					vec2i32(args[0].getInt32(), args[1].getInt32()));
		}
		setBool("rebuild", true);
		return var();
	}

	var sprite::draw(list args) {
		auto view = args[0].getUInt16();
		updateVertexBuffer();
		auto vbh = VertexBufferHandle{
			getUInt16("vbh", bgfx::kInvalidHandle)};
		auto ibh =
			IndexBufferHandle{getUInt16("ibh", bgfx::kInvalidHandle)};
		auto program = SpriteProgram;
		auto tex = getObject<texture>("texture");
		if (!tex) return genericError("Missing texture on sprite.");
		auto texColor = bgfx::UniformHandle{
			getUInt16("texColor", kInvalidHandle)};
		if (!isValid(texColor))
			return genericError("Invalid texture uniform.");
		if (!isValid(vbh))
			return genericError("Invalid vertex buffer handle");
		if (!isValid(ibh))
			return genericError("Invalid index buffer handle");

		auto parentObject = getObject<entity>("object");
		auto parentTrans = parentObject.getTransform();
		auto mtx = parentTrans.getWorldMatrix();

		setTransform(mtx.getPtr());
		setVertexBuffer(0, vbh);
		tex.bind({0, texColor.idx});
		setIndexBuffer(ibh);
		setState(
			0 | BGFX_STATE_BLEND_NORMAL | BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
		submit(view, program);
		return var();
	}

	enum shaderType {
		NullShaderType = 0,
		VertexShaderType,
		FragmentShaderType,
		ComputeShaderType,
	};

	binary getSpriteShaderData(shaderType stype) {
		auto renderType = getRendererType();
		switch (renderType) {
			case bgfx::RendererType::Direct3D11:
			case bgfx::RendererType::Direct3D12: {
				switch (stype) {
					case VertexShaderType:
						return dx11_vs_sprite;
					case FragmentShaderType:
						return dx11_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::Direct3D9: {
				switch (stype) {
					case VertexShaderType:
						return dx9_vs_sprite;
					case FragmentShaderType:
						return dx9_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::Metal: {
				switch (stype) {
					case VertexShaderType:
						return metal_vs_sprite;
					case FragmentShaderType:
						return metal_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::OpenGLES: {
				switch (stype) {
					case VertexShaderType:
						return essl_vs_sprite;
					case FragmentShaderType:
						return essl_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::OpenGL: {
				switch (stype) {
					case VertexShaderType:
						return glsl_vs_sprite;
					case FragmentShaderType:
						return glsl_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::Vulkan: {
				switch (stype) {
					case VertexShaderType:
						return spirv_vs_sprite;
					case FragmentShaderType:
						return spirv_fs_sprite;
					default:
						break;
				}
			}
			case bgfx::RendererType::Gnm:
			case bgfx::RendererType::Nvn:
			case bgfx::RendererType::Noop:
			default:
				break;
		}
		return binary();
	}

	var sprite::initialize(list) {
		updateVertexBuffer();
		uint16_t indicies[6] = {
			0, 1, 2,  // 1
			1, 3, 2   // 2
		};
		auto iData = bgfx::copy(indicies, sizeof(uint16_t) * 6);
		auto ibh = createIndexBuffer(iData);
		if (isValid(ibh)) setUInt16("ibh", ibh.idx);
		if (
			getUInt16("texColor", kInvalidHandle) == kInvalidHandle) {
			auto textureUniform = bgfx::createUniform(
				"s_texColor", bgfx::UniformType::Sampler);
			getPrototype().setUInt16("texColor", textureUniform.idx);
		}
		if (
			getUInt16("colorUniform", kInvalidHandle) ==
			kInvalidHandle) {
			auto colorUniform = bgfx::createUniform(
				"u_color0", bgfx::UniformType::Vec4);
			getPrototype().setUInt16(
				"colorUniform", colorUniform.idx);
			float white[4] = {1.0f, 1.0f, 1.0f, 0.0f};
			setUniform(colorUniform, white);
		}
		if (
			getUInt16("opacityUniform", kInvalidHandle) ==
			kInvalidHandle) {
			auto opacityUniform = bgfx::createUniform(
				"u_opacity", bgfx::UniformType::Vec4);
			getPrototype().setUInt16(
				"opacityUniform", opacityUniform.idx);
			float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			setUniform(opacityUniform, white);
		}

		if (SpriteProgram.idx == kInvalidHandle) {
			auto fragBin = getSpriteShaderData(FragmentShaderType);
			auto vertBin = getSpriteShaderData(VertexShaderType);
			auto vertMem =
				bgfx::copy(&*vertBin.begin(), vertBin.size());
			SpriteVertShader = createShader(vertMem);
			auto SpriteVertName = "SpriteVert";
			setName(
				SpriteVertShader,
				SpriteVertName,
				strlen(SpriteVertName));
			auto fragMem =
				bgfx::copy(&*fragBin.begin(), fragBin.size());
			SpriteFragShader = createShader(fragMem);
			auto SpriteFragName = "SpriteFrag";
			setName(
				SpriteFragShader,
				SpriteVertName,
				strlen(SpriteFragName));
			if (
				isValid(SpriteVertShader) &&
				isValid(SpriteFragShader)) {
				SpriteProgram = createProgram(
					SpriteVertShader, SpriteFragShader, false);
				if (isValid(SpriteProgram))
					sprite::getPrototype().setUInt16(
						"program", SpriteProgram.idx);
			} else {
				return genericError("Failed to load sprite shaders.");
			}
		}
		return var();
	}

	var sprite::destroy(list) {
		auto vbh = VertexBufferHandle{
			getUInt16("vbh", bgfx::kInvalidHandle)};
		auto ibh =
			IndexBufferHandle{getUInt16("ibh", bgfx::kInvalidHandle)};
		bgfx::destroy(vbh);
		bgfx::destroy(ibh);
		return var();
	}

	sprite::sprite() : renderable() {}

	sprite::sprite(obj config) : renderable() {
		setParent(sprite::getPrototype());
		copy(config);
		initialize();
	}
}  // namespace gold