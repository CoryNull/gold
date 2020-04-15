#pragma once

#include "object.hpp"

namespace gold {
	class entity : public object {
	 protected:
		friend class engine;
		static object proto;

	 public:
		entity();
		entity(object config);

		var initialize(varList args = {});
		var add(varList args);
		var remove(varList args);
		var enable(varList args);
		var disable(varList args);

		entity& operator+=(varList args);
		entity& operator-=(varList args);
	};
}  // namespace gold