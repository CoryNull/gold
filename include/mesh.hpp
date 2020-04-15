#pragma once

#include <vector>

#include "object.hpp"
#include "renderable.hpp"

namespace gold {
	using namespace std;
	class mesh : public renderable {
	 protected:
	 public:
		mesh();
		mesh(object config);
	};
}  // namespace gold
