#pragma once

#include "object.hpp"

namespace red {
	class component : public object {
	 protected:
		friend class entity;
		static object proto;

		static var update(object& self, var args);
		static var draw(object& self, var args);

	 public:
		component();
		component(object& config);
	};
}  // namespace red