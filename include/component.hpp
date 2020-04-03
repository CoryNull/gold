#pragma once

#include "object.hpp"

namespace red {
	class component : public object {
	 protected:
		static var update(object& self, var args);
		static var draw(object& self, var args);

	 public:
		static object proto;
		component();
		component(object config);
	};
}  // namespace red