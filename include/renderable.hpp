#pragma once

#include "object.hpp"

namespace red {
	class renderable : public object {
	 protected:
		static var draw(object& self, var args);

	 public:
		static object proto;
		renderable();
		renderable(object config);
	};
}  // namespace red