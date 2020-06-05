#include "image.hpp"

#include <string_view>
#include <bx/allocator.h>

namespace gold {
	image::image() : object() {}
	image::image(object config) : object(config) {
		auto con = bimg::ImageContainer();
		con.m_allocator = &gold::defaultAllocator;
		auto view = getStringView("data");
		auto err = (bx::Error*)(nullptr);
		auto img =
			bimg::imageParse(con, view.data(), view.size(), err);
		if (img) {
			binary data;
			if (con.m_data) {
				data = binary((size_t)con.m_size);
				memcpy(data.data(), con.m_data, con.m_size);
			} else
				data = binary(view.begin(), view.end());
			setBinary("data", data);
			setUInt32("format", con.m_format);
			setUInt8("orientation", con.m_orientation);
			setUInt32("width", con.m_width);
			setUInt32("height", con.m_height);
			setUInt32("depth", con.m_depth);
			setUInt16("numLayers", con.m_numLayers);
			setUInt8("numMips", con.m_numMips);
			setUInt8("hasAlpha", con.m_hasAlpha);
			setBool("cubeMap", con.m_cubeMap);
			setBool("ktx", con.m_ktx);
			setBool("ktxLE", con.m_ktxLE);
			setBool("srgb", con.m_srgb);
		}
	}

	void image::update(object data) {}
	void image::destroy() {}
}  // namespace gold