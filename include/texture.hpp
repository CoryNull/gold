#pragma once

#include "types.hpp"
#include "file.hpp"

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