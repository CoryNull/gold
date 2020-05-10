#pragma once

/* <Includes> */
#include <set>

#include "types.hpp"
#include "renderable.hpp"
#include "component.hpp"
/* </Includes> */

namespace gold {
	struct engine : public object {
	 protected:
		static object& getPrototype();
		friend struct world;

		void registerComponent(component& comp);

	 public:
		string getSettingsDir();
		string getSettingsPath();

		engine();
		engine(string company, string gameName);
		static set<string> allowedConfigNames();

		var destroy(list args = {});
		var start(list args = {});
		var initialize(list args = {});
		var loadSettings(list args = {});
		var saveSettings(list args = {});
		var handleEntity(list args);
		var addElement(list args);
		var getPrimaryCamera(list args = {});
		var addRegisterCompnentCallback(list args);

		engine& operator+=(list element);
		engine& operator-=(list element);
	};
}  // namespace gold
