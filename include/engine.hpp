#pragma once

/* <Includes> */
#include <set>

#include "object.hpp"
#include "renderable.hpp"
#include "worker.hpp"
/* </Includes> */

namespace red {
	class engine : public object {
	 protected:
		static object proto;
		static string getSettingsDir();
		static string getSettingsPath();

		static var destroy(object& self, var args);
		static var start(object& self, var args);
		static var loadSettings(object& self, var args);
		static var saveSettings(object& self, var args);
		static var handleEntity(object& self, var args);
		static var addElement(object& self, var args);

		worker updateWorker;
		worker drawWorker;

	 public:
		engine();
		static set<string> allowedConfigNames();

		engine& operator+=(var element);
		engine& operator-=(var element);
	};
}  // namespace red
