#pragma once

#include <bimg/bimg.h>

#include "types.hpp"

namespace gold {
	struct image : object {
	 public:
		image();
		image(object config);

		void update(object data);
		void destroy();
	};
}  // namespace gold