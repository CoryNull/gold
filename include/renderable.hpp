#pragma once

#include "object.hpp"

namespace gold {
	class renderable : public object {
	 public:
		static object proto;
		renderable(object* parent = nullptr);
		renderable(object data, object* parent);

		var draw(varList args = {});
	};
}  // namespace gold