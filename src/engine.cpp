
#include "engine.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#include "graphics.hpp"
#include "var.hpp"
#include "window.hpp"

namespace red {
	using namespace std;
	object engine::proto = object({{"start", engine::start},
																 {"destroy", engine::destroy},
																 {"loadSettings", engine::loadSettings},
																 {"saveSettings", engine::saveSettings}});

	var engine::destroy(object& self, var& args) {
		auto window = self.getObject("window");
		if (window != nullptr)
			(*window)("destroy");
		self.setNull("window");
		self.setBool("running", false);
		SDL_Quit();
		return var();
	}

	string engine::getSettingsDir() {
		auto dir = string(SDL_GetPrefPath("MountainAndValley", "RED2D"));
		if (dir.size() == 0)
			dir = "./";
		return dir;
	}

	string engine::getSettingsPath() {
		return string(engine::getSettingsDir() + "config.json");
	}

	var engine::loadSettings(object& self, var& args) {
		auto configPath = engine::getSettingsPath();
		auto configJSON = object::loadJSON(configPath);
		object* config = nullptr;
		if (configJSON.isObject()) {
			config = configJSON.getObject();
			cout << "loading settings: " << configPath << endl;
			self.setObject("config", *config);
			return var(*config);
		}
		config = new object();
		return var(*config);
	}

	var engine::saveSettings(object& self, var& args) {
		auto config = self.getObject("config");
		auto primaryWindow = (window*)self.getObject("window");
		if (config && primaryWindow) {
			auto windowConfigVar = (*primaryWindow)("getConfig");
			auto windowConfig = windowConfigVar.getObject();
			if (windowConfig != nullptr) {
				config->setObject("window", *windowConfig);
			} else if (windowConfigVar.isError())
				return windowConfigVar;
			auto configPath = engine::getSettingsPath();
			object::saveJSON(configPath, *config);
		}
		return var();
	}

	var engine::start(object& self, var& args) {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			cout << "[SDL2]" << SDL_GetError() << endl;
			return var(runtime_error(SDL_GetError()));
		}

		auto configVar = self("loadSettings");
		auto config = *configVar.getObject();

		auto windowConfig = config.getObject("window");
		auto primaryWindow =
				windowConfig != nullptr ? window(*windowConfig) : window();
		self.setObject("window", primaryWindow);

		self.setBool("running", true);
		SDL_Event e;
		auto eVar = var(&e);
		while (true) {
			SDL_PollEvent(&e);
			if (!self.getBool("running") || e.type == SDL_QUIT)
				break;
			primaryWindow("handleEvent", eVar);
		}
		self("saveSettings");
		return var();
	}

	engine::engine() : object(proto) {}

	set<string> engine::allowedConfigNames() { return {"window"}; }

}  // namespace red