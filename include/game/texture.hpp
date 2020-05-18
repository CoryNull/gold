#pragma once

#include "file.hpp"
#include "types.hpp"

namespace gold {
	struct texture : public file {
	 protected:
		static object& getPrototype();

	 public:
		texture();
		texture(file copy);
		texture(path fpath);
		texture(binary data);

		var load(list args = {});
		var bind(list args);
	};
}  // namespace gold