#pragma once

#include "object.hpp"

namespace red {
	class texture : public object {
	 protected:
		static object proto;

	 public:
		texture();
		texture(object config);
	};
}  // namespace red