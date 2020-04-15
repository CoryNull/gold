#pragma once

/* <Includes> */
#include <set>

#include "object.hpp"
#include "renderable.hpp"
#include "worker.hpp"
/* </Includes> */

namespace gold {
	class engine : public object {
	 protected:
		static object proto;

		worker updateWorker;
		worker drawWorker;

	 public:
		string getSettingsDir();
		string getSettingsPath();

		engine(string company, string gameName);
		static set<string> allowedConfigNames();

		var destroy(varList args = {});
		var start(varList args = {});
		var loadSettings(varList args = {});
		var saveSettings(varList args = {});
		var handleEntity(varList args);
		var addElement(varList args);

		engine& operator+=(varList element);
		engine& operator-=(varList element);
	};
}  // namespace gold
