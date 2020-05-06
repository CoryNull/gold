#pragma once

#include <filesystem>

#include "types.hpp"

namespace gold {
	using namespace std;
	using path = std::filesystem::path;
	struct file : public object {
	 protected:
		static object& getPrototype();

	 public:
		file();
		file(path p);
		file(binary data);

		var save(list args = {});
		var load(list args = {});
		var trash(list args = {});
		var getWriteTime(list args = {});
		var hash(list args = {});

		operator binary();
		operator string();

		static file& readFile(path p, file& results);
		static object& recursiveReadDirectory(
			path p, object& results);
	};
}  // namespace gold