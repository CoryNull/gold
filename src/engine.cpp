
#include "engine.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#include "array.hpp"
#include "component.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "var.hpp"
#include "window.hpp"
#include "worker.hpp"

namespace gold {
	using namespace std;
	object engine::proto = object({
		{"start", method(&engine::start)},
		{"destroy", method(&engine::destroy)},
		{"loadSettings", method(&engine::loadSettings)},
		{"handleEntity", method(&engine::handleEntity)},
		{"saveSettings", method(&engine::saveSettings)},
		{"addElement", method(&engine::addElement)},
	});

	var engine::destroy(varList) {
		auto window = getObject("window");
		if (window != nullptr) (*window)("destroy");
		setNull("window");
		setBool("running", false);
		SDL_Quit();
		return var();
	}

	string engine::getSettingsDir() {
		auto gameName = getString("gameName");
		auto company = getString("company");
		auto dir = string(
			SDL_GetPrefPath(company.c_str(), gameName.c_str()));
		if (dir.size() == 0) dir = "./";
		return dir;
	}

	string engine::getSettingsPath() {
		return string(engine::getSettingsDir() + "config.json");
	}

	var engine::loadSettings(varList) {
		auto configPath = getSettingsPath();
		auto configJSON = object::loadJSON(configPath);
		object config;
		if (configJSON.isObject()) {
			auto o = configJSON.getObject();
			config = o ? object(*o) : config;
			cout << "Loading settings: " << configPath << endl;
			setObject("config", config);
			return var(config);
		}
		return var(config);
	}

	var engine::saveSettings(varList) {
		auto config = getObject("config");
		auto win = (window*)getObject("window");
		auto gfx = (backend*)getObject("graphics");
		if (config && win && gfx) {
			auto windowConfigVar = win->getConfig({});
			auto windowConfig = windowConfigVar.getObject();
			if (windowConfig != nullptr) {
				config->setObject("window", *windowConfig);
			} else if (windowConfigVar.isError())
				return windowConfigVar;

			auto gfxConfigVar = gfx->getConfig({});
			auto gfxConfig = gfxConfigVar.getObject();
			if (gfxConfig != nullptr && gfxConfig->getSize() > 0) {
				config->setObject("graphics", *gfxConfig);
			} else if (gfxConfigVar.isError())
				return gfxConfigVar;

			auto configPath = getSettingsPath();
			object::saveJSON(configPath, *config);
		}
		return var();
	}

	var engine::addElement(varList args) {
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto obj = it->getObject();
			if (obj) (*this) += {*it};
		}

		return var();
	}

	var engine::handleEntity(varList args) {
		auto obj = (args[0]).getObject();
		if (obj) {
			auto comps = obj->getArray("components");
			auto objs = obj->getArray("children");
			if (comps) {
				for (auto it = comps->begin(); it != comps->end();
						 ++it) {
					auto comp = it->getObject();
					if (comp) {
						if (it->isObject(component::proto))
							updateWorker.add(
								comp->getMethod("update"), *comp, varList());
						if (it->isObject(renderable::proto))
							drawWorker.add(
								comp->getMethod("draw"), *comp, varList());
					}
				}
			}
			if (objs)
				for (auto it = objs->begin(); it != objs->end(); ++it)
					handleEntity({*it});
		}
		return var();
	}

	var engine::start(varList) {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			cout << "[SDL2]" << SDL_GetError() << endl;
			return genericError(SDL_GetError());
		}

		auto configVar = loadSettings({});
		auto config = *configVar.getObject();
		auto gameName = getString("gameName");

		auto windowConfig = config.getObject("window");
		auto backendConfig = config.getObject("graphics");
		auto win = create<window>("window", windowConfig);
		win.setTitle({gameName});
		auto gfx = create<backend>("graphics", backendConfig);
		gfx.initialize({win});

		auto entites = getArray("entites");

		setBool("running", true);
		SDL_Event e;
		while (getBool("running")) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) setBool("running", false);
				win.handleEvent({var((void*)&e)});
			}
			gfx.preFrame();
			if (entites)
				for (auto it = entites->begin(); it != entites->end();
						 ++it)
					handleEntity({(*it)});
			updateWorker.wait();
			drawWorker.wait();
			gfx.renderFrame();
		}
		saveSettings();
		updateWorker.wait();
		drawWorker.wait();
		updateWorker.killAll();
		drawWorker.killAll();
		gfx.destroy();
		destroy();
		return var();
	}

	engine::engine(string company, string gameName)
		: object(&proto), updateWorker(), drawWorker() {
		setArray("entites", array());
		setString("company", company);
		setString("gameName", gameName);
	}

	set<string> engine::allowedConfigNames() {
		return {"window", "graphics"};
	}

	engine& engine::operator+=(varList list) {
		auto a = getArray("entites");
		if (a == nullptr) return *this;
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->isObject(entity::proto)) (*a) += {*it};
		}
		return *this;
	}

	engine& engine::operator-=(varList list) {
		auto a = getArray("entites");
		if (a == nullptr) return *this;
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->isObject(entity::proto)) (*a) -= {*it};
		}
		return *this;
	}

}  // namespace gold