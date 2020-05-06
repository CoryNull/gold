#pragma once

#include "types.hpp"
#include "renderable.hpp"

namespace gold {
	using namespace std;
	struct mesh : public renderable {
	 protected:
	 public:
		mesh();
		mesh(object config);
	};
}  // namespace gold
