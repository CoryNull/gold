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
	};

	obj& sprite::getPrototype() {
		static auto proto = obj({
			{"draw", method(&sprite::draw)},
			{"initialize", method(&sprite::initialize)},
			{"destroy", method(&sprite::destroy)},
			{"proto", renderable::getPrototype()},
		});
		return proto;
	}

	static auto m_timeOffset = bx::getHPCounter();

	var sprite::draw(list args) {
		auto view = args[0].getUInt16();
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
		float time =
			(float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));
		float mtx[16];

		auto parentObject = getObject("object");
		auto parentTrans = transform();
		auto parentComps = parentObject.getList("components");
		auto parentIt = parentComps.find(getPrototype());
		if (parentIt != parentComps.end())
			parentTrans = parentIt->getObject();
		parentTrans.getWorldMatrix(mtx);

		setTransform(mtx);
		setVertexBuffer(0, vbh);
		tex.bind({0, texColor.idx});
		setIndexBuffer(ibh);
		setState(
			0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA);
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
		auto layout = (VertexLayout*)getPtr("layout");
		if (!layout) {
			layout = new VertexLayout();
			memset(layout, 0, sizeof(VertexLayout));
			layout->begin()
				.add(Attrib::Position, 3, AttribType::Float)
				.add(Attrib::TexCoord0, 2, AttribType::Float)
				.end();
			getPrototype().setPtr("layout", layout);
		}
		PosColorVertex verts[4] = {
			{-1, 1, 1, 0, 1},
			{1, 1, 1, 1, 1},
			{-1, -1, 1, 0, 0},
			{1, -1, 1, 1, 0},
		};
		auto vData = bgfx::copy(verts, sizeof(PosColorVertex) * 4);
		auto vbh = createVertexBuffer(vData, *layout);
		if (isValid(vbh)) setUInt16("vbh", vbh.idx);
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
			float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			setUniform(colorUniform, white);
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

	sprite::sprite() : renderable() {
	}

	sprite::sprite(obj config) : renderable() {
		setParent(sprite::getPrototype());
		copy(config);
		initialize();
	}
}  // namespace gold