#pragma once

#include "object.hpp"

namespace gold {
	class texture : public object {
	 protected:
		static object proto;

	 public:
		texture();
		texture(object config);
	};
}  // namespace gold