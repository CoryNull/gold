#pragma once

#include "object.hpp"

namespace gold {
	class component : public object {
	 protected:
		var update(varList args);
		var draw(varList args);

	 public:
		static object proto;
		component();
		component(object config);
	};
}  // namespace gold