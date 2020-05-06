#include "texture.hpp"

#include <bgfx/bgfx.h>
#include <bimg/bimg.h>
#include <bx/allocator.h>
#include <bx/error.h>

namespace gold {
	namespace fs = std::filesystem;

	object& texture::getPrototype() {
		static auto proto = obj({
			{"load", method(&texture::load)},
			{"bind", method(&texture::bind)},
			{"proto", file::getPrototype()},
		});
		return proto;
	}

	texture::texture() : file() {}

	texture::texture(file copy) : file(copy) { load(); }

	texture::texture(path fpath) : file(fpath) { load(); }

	texture::texture(binary data) : file(data) { load(); }

	var texture::load(list args) {
		auto textRet = file::load(args);
		if (textRet.isBinary()) {
			auto textureBinary = textRet.getBinary();
			if (textureBinary.size() == 0)
				getBinary("data", textureBinary);
			auto dataSize = textureBinary.size();
			auto dataPtr = (void*)textureBinary.data();
			auto textureHandle = bgfx::TextureHandle();
			auto parseError = bx::Error();
			auto imageContainer = bimg::ImageContainer();
			auto parsed = bimg::imageParse(
				imageContainer, dataPtr, dataSize, &parseError);
			auto flags = uint64_t(0);
			if (parsed) {
				dataPtr =
					imageContainer.m_data != nullptr
						? (void*)((uint64_t)imageContainer.m_data + imageContainer.m_offset)
						: (void*)((uint64_t)dataPtr + imageContainer.m_offset);
				dataSize =
					imageContainer.m_data != nullptr
						? imageContainer.m_size - imageContainer.m_offset
						: dataSize - imageContainer.m_offset;
				auto mem = bgfx::copy(dataPtr, dataSize);
				setUInt32("width", imageContainer.m_width);
				setUInt32("height", imageContainer.m_height);
				setUInt32("depth", imageContainer.m_depth);
				setUInt8("numMips", imageContainer.m_numMips);
				setUInt16("numLayers", imageContainer.m_numLayers);
				setUInt8("orientation", imageContainer.m_orientation);
				setUInt32("format", imageContainer.m_format);
				setBool("hasAlpha", imageContainer.m_hasAlpha);
				setBool("isCube", imageContainer.m_cubeMap);
				setBool("isSRGB", imageContainer.m_srgb);
				setBool("isKTX", imageContainer.m_ktx);
				setBool("isKTXLE", imageContainer.m_ktxLE);
				if (imageContainer.m_cubeMap) {
					textureHandle = bgfx::createTextureCube(
						imageContainer.m_width,
						1 < imageContainer.m_numMips,
						imageContainer.m_numLayers,
						bgfx::TextureFormat::Enum(imageContainer.m_format),
						flags, mem);
				} else if (imageContainer.m_depth > 1) {
					textureHandle = bgfx::createTexture3D(
						imageContainer.m_width, imageContainer.m_height,
						imageContainer.m_depth,
						1 < imageContainer.m_numMips,
						bgfx::TextureFormat::Enum(imageContainer.m_format),
						flags, mem);
				} else if (bgfx::isTextureValid(
										 0, false, imageContainer.m_numLayers,
										 bgfx::TextureFormat::Enum(
											 imageContainer.m_format),
										 flags)) {
					textureHandle = bgfx::createTexture2D(
						imageContainer.m_width, imageContainer.m_height,
						1 < imageContainer.m_numMips,
						imageContainer.m_numLayers,
						bgfx::TextureFormat::Enum(imageContainer.m_format),
						flags, mem);
				}

				if (isValid(textureHandle)) {
					auto filePath =
						getString("path", "Generated Texture");
					bgfx::setName(textureHandle, filePath.c_str());
					setUInt16("handle", textureHandle.idx);
					return *this;
				}
			}
			auto msg = parseError.getMessage();
			return genericError(
				string(msg.getPtr(), msg.getLength()));
		}
		return textRet;
	}

	var texture::bind(list args) {
		auto textureHandle = bgfx::TextureHandle{
			getUInt16("handle", bgfx::kInvalidHandle)};
		auto stage = args.getUInt8(0);
		auto uniform = bgfx::UniformHandle{
			args.getUInt16(1, bgfx::kInvalidHandle)};
		auto flags = args.getUInt32(2, UINT32_MAX);
		bgfx::setTexture(stage, uniform, textureHandle, flags);
		return var();
	}
}  // namespace gold