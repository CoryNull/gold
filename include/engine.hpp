#pragma once

/* <Includes> */
#include <set>

#include "types.hpp"
#include "renderable.hpp"
/* </Includes> */

namespace gold {
	struct engine : public object {
	 protected:
		static object& getPrototype();

	 public:
		string getSettingsDir();
		string getSettingsPath();

		engine(string company, string gameName);
		static set<string> allowedConfigNames();

		var destroy(list args = list::initList{});
		var start(list args = list::initList{});
		var initialize(list args = list::initList{});
		var loadSettings(list args = list::initList{});
		var saveSettings(list args = list::initList{});
		var handleEntity(list args);
		var addElement(list args);

		engine& operator+=(list element);
		engine& operator-=(list element);
	};
}  // namespace gold
