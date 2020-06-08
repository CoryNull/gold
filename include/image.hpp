#pragma once

#include <bimg/bimg.h>

#include "types.hpp"

namespace gold {
	struct image : object {
		bimg::ImageContainer* getContainer();

	 public:
		static image checkerboard(
			uint32_t w, uint32_t h, uint32_t step, uint32_t _0,
			uint32_t _1);
		static image solid(uint32_t w, uint32_t h, uint32_t solid);
		image();
		image(object config);

		bool isCompressed();
		bool isColor();
		bool isDepth();
		bool isValid();
		bool isFloat();
		uint8_t getBitsPerPixel();

		var getFormat(list);
		var getFormatName(list);

		var convert(list args);
		var toLinearRGBA32F();
		var toGammaRGBA32F();
		var linearDownsampleRGBA32F2x2(list args);
		var downsampleRGBA82x2(list args);
		var downsampleRGBA32F2x2(list args);
		var downsampleRGBA32F2x2NormalMap(list args);
		var swizzleBGRA8(list args);
		var decodeToR8(list args);
		var decodeToBGRA8(list args);
		var decodeToRGBA8(list args);
		var decodeToRGBA32F(list args);
		var getRawData(list args);

		var writeTGA(list args);
		var writePNG(list args);
		var writeEXR(list args);
		var writeHDR(list args);
		var writeDDS(list args);
		var writeKTX(list args);

		var destroy();
	};
}  // namespace gold
