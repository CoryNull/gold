#pragma once

#include "object.hpp"

namespace red {
	class renderable : public object {
	 protected:
		static object proto;

		static var draw(object& self, var args);

	 public:
		renderable();
		renderable(object& config);
	};
}  // namespace red