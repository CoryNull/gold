#include "meshRenderer.hpp"

#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <bx/timer.h>

#include "entity.hpp"
#include "mesh.hpp"
#include "shaderPBR.hpp"
#include "texture.hpp"
#include "transform.hpp"

namespace gold {
	using namespace bgfx;
	binary getPBRShaderData(shaderType stype);

	static ShaderHandle PBRFragShader =
		ShaderHandle{kInvalidHandle};
	static ShaderHandle PBRVertShader =
		ShaderHandle{kInvalidHandle};
	static ProgramHandle PBRProgram =
		ProgramHandle{kInvalidHandle};

	object& meshRenderer::getPrototype() {
		static auto proto = object{
			{"priority", priorityEnum::drawPriority},
			{"node", ""},
			{"mesh", var()},
			{"proto", renderable::getPrototype()},
		};
		return proto;
	}

	void meshRenderer::setMaterial(object matData) {}

	meshRenderer::meshRenderer() : renderable() {}

	meshRenderer::meshRenderer(object config) : renderable() {
		copy(config);
		setParent(getPrototype());
	}

	var meshRenderer::draw(list args) {
		auto view = args[0].getUInt16();
		auto program = PBRProgram;

		auto parentObject = getObject<entity>("object");
		auto parentTrans = parentObject.getTransform();
		auto mtx = parentTrans.getWorldMatrix();

		auto meshD = getObject<mesh>("mesh");
		auto node = getString("node");
		auto pCount = meshD.getPrimitiveCount(node).getUInt64();
		for (size_t i = 0; i < pCount; ++i) {
			auto vbh = VertexBufferHandle{
				meshD.getVertexBufferHandle(node, i).getUInt16(
					bgfx::kInvalidHandle)};
			auto ibh = IndexBufferHandle{
				meshD.getIndexBufferHandle(node, i).getUInt16(
					bgfx::kInvalidHandle)};
			auto mat = meshD.getMaterialFromPrimitive(node, i);
			setVertexBuffer(0, vbh);
			setIndexBuffer(ibh);
			setMaterial(mat);
			setState(
				0 | BGFX_STATE_BLEND_NORMAL | BGFX_STATE_WRITE_RGB |
				BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
				BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
			submit(view, program);
		}
		return var();
	}

	var meshRenderer::initialize(list) {
		if (
			getUInt16("LightDirection", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"LightDirection",
				bgfx::createUniform(
					"u_LightDirection", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("LightColor", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"LightColor", bgfx::createUniform(
												"u_LightColor", bgfx::UniformType::Vec4)
												.idx);
		if (
			getUInt16("AmbientLightColor", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"AmbientLightColor",
				bgfx::createUniform(
					"u_AmbientLightColor", bgfx::UniformType::Vec4)
					.idx);
		if (getUInt16("Camera", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"Camera",
				bgfx::createUniform("u_Camera", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("EmissiveFactor", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"EmissiveFactor",
				bgfx::createUniform(
					"u_EmissiveFactor", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("BaseColorFactor", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"BaseColorFactor",
				bgfx::createUniform(
					"u_BaseColorFactor", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("MetallicRoughnessValues", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"MetallicRoughnessValues",
				bgfx::createUniform(
					"u_MetallicRoughnessValues", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("ScaleDiffBaseMR", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"ScaleDiffBaseMR",
				bgfx::createUniform(
					"u_ScaleDiffBaseMR", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("ScaleFGDSpec", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"ScaleFGDSpec",
				bgfx::createUniform(
					"u_ScaleFGDSpec", bgfx::UniformType::Vec4)
					.idx);

		if (
			getUInt16("ScaleIBLAmbient", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"ScaleIBLAmbient",
				bgfx::createUniform(
					"u_ScaleIBLAmbient", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("ScaleIBLAmbient", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"ScaleIBLAmbient",
				bgfx::createUniform(
					"u_ScaleIBLAmbient", bgfx::UniformType::Vec4)
					.idx);
		if (
			getUInt16("DiffuseEnvSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"DiffuseEnvSampler",
				bgfx::createUniform(
					"u_DiffuseEnvSampler", bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("SpecularEnvSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"SpecularEnvSampler",
				bgfx::createUniform(
					"u_SpecularEnvSampler", bgfx::UniformType::Sampler)
					.idx);
		if (getUInt16("brdfLUT", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"brdfLUT", bgfx::createUniform(
										 "u_brdfLUT", bgfx::UniformType::Sampler)
										 .idx);
		if (
			getUInt16("BaseColorSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"BaseColorSampler",
				bgfx::createUniform(
					"u_BaseColorSampler", bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("NormalSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"NormalSampler",
				bgfx::createUniform(
					"u_NormalSampler", bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("EmissiveSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"EmissiveSampler",
				bgfx::createUniform(
					"u_EmissiveSampler", bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("MetallicRoughnessSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"MetallicRoughnessSampler",
				bgfx::createUniform(
					"u_MetallicRoughnessSampler",
					bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("OcclusionSampler", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"OcclusionSampler",
				bgfx::createUniform(
					"u_OcclusionSampler", bgfx::UniformType::Sampler)
					.idx);
		if (
			getUInt16("HasTable0", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"HasTable0", bgfx::createUniform(
											 "u_HasTable0", bgfx::UniformType::Vec4)
											 .idx);
		if (
			getUInt16("HasTable1", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"HasTable1", bgfx::createUniform(
											 "u_HasTable1", bgfx::UniformType::Vec4)
											 .idx);
		if (
			getUInt16("UseLightTable", kInvalidHandle) ==
			kInvalidHandle)
			getPrototype().setUInt16(
				"UseLightTable",
				bgfx::createUniform(
					"u_UseLightTable", bgfx::UniformType::Vec4)
					.idx);
		if (getUInt16("Data0", kInvalidHandle) == kInvalidHandle)
			getPrototype().setUInt16(
				"Data0",
				bgfx::createUniform("u_Data0", bgfx::UniformType::Vec4)
					.idx);

		if (PBRProgram.idx == kInvalidHandle) {
			auto fragBin = getPBRShaderData(FragmentShaderType);
			auto vertBin = getPBRShaderData(VertexShaderType);
			auto vertMem =
				bgfx::copy(&*vertBin.begin(), vertBin.size());
			PBRVertShader = createShader(vertMem);
			auto VertName = "PBRVert";
			setName(PBRVertShader, VertName, strlen(VertName));
			auto fragMem =
				bgfx::copy(&*fragBin.begin(), fragBin.size());
			PBRFragShader = createShader(fragMem);
			auto FragName = "PBRFrag";
			setName(PBRFragShader, FragName, strlen(FragName));
			if (isValid(PBRVertShader) && isValid(PBRFragShader)) {
				PBRProgram =
					createProgram(PBRVertShader, PBRFragShader, false);
				if (isValid(PBRProgram))
					meshRenderer::getPrototype().setUInt16(
						"program", PBRProgram.idx);
			} else {
				return genericError("Failed to load pbr shaders.");
			}
		}

		return var();
	}

	var meshRenderer::destroy(list) { return var(); }

	binary getPBRShaderData(shaderType stype) {
		auto renderType = getRendererType();
		switch (renderType) {
			case bgfx::RendererType::Direct3D11:
			case bgfx::RendererType::Direct3D12: {
				switch (stype) {
					case VertexShaderType:
						return dx11_vs_pbr;
					case FragmentShaderType:
						return dx11_fs_pbr;
					default:
						break;
				}
			}
			case bgfx::RendererType::Direct3D9: {
				switch (stype) {
					case VertexShaderType:
						return dx9_vs_pbr;
					case FragmentShaderType:
						return dx9_fs_pbr;
					default:
						break;
				}
			}
			case bgfx::RendererType::Metal: {
				switch (stype) {
					case VertexShaderType:
						return metal_vs_pbr;
					case FragmentShaderType:
						return metal_fs_pbr;
					default:
						break;
				}
			}
			case bgfx::RendererType::OpenGLES: {
				switch (stype) {
					case VertexShaderType:
						return essl_vs_pbr;
					case FragmentShaderType:
						return essl_fs_pbr;
					default:
						break;
				}
			}
			case bgfx::RendererType::OpenGL: {
				switch (stype) {
					case VertexShaderType:
						return glsl_vs_pbr;
					case FragmentShaderType:
						return glsl_fs_pbr;
					default:
						break;
				}
			}
			case bgfx::RendererType::Vulkan: {
				switch (stype) {
					case VertexShaderType:
						return spirv_vs_pbr;
					case FragmentShaderType:
						return spirv_fs_pbr;
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

}  // namespace gold