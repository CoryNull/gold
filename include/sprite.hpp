#pragma once

#include "object.hpp"
#include "renderable.hpp"

namespace red {
	class sprite : public renderable {
	 protected:
		static object proto;

		static var draw(object& self, var args);

	 public:
		sprite();
		sprite(object& config);
	};
}  // namespace red