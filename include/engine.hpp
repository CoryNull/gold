#pragma once

/* <Includes> */
#include <set>

#include "object.hpp"
/* </Includes> */

namespace red {
	class engine : public object {
	 protected:
		static object proto;
		static string getSettingsDir();
		static string getSettingsPath();

		static var destroy(object& self, var& args);
		static var start(object& self, var& args);
		static var loadSettings(object& self, var& args);
		static var saveSettings(object& self, var& args);

	 public:
		engine();
		static set<string> allowedConfigNames();
	};
}  // namespace red
