#pragma once

#include "object.hpp"
#include "renderable.hpp"

namespace gold {
	class sprite : public renderable {
	 protected:
		static object proto;

	 public:
		sprite();
		sprite(object config);

		var draw(varList args = {});
	};
}  // namespace gold