#pragma once

#include "object.hpp"

namespace red {
	class entity : public object {
	 protected:
		friend class engine;
		static object proto;

		static var initialize(object& self, var args);
		static var add(object& self, var args);
		static var remove(object& self, var args);
		static var enable(object& self, var args);
		static var disable(object& self, var args);

	 public:
		entity();
		entity(object config);

		entity& operator+=(var args);
		entity& operator-=(var args);
	};
}  // namespace red