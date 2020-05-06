
#include "engine.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#include "component.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "promise.hpp"
#include "window.hpp"
#include "worker.hpp"

namespace gold {
	using namespace std;
	object& engine::getPrototype() {
		static auto proto = object({
			{"initialize", method(&engine::initialize)},
			{"start", method(&engine::start)},
			{"destroy", method(&engine::destroy)},
			{"loadSettings", method(&engine::loadSettings)},
			{"handleEntity", method(&engine::handleEntity)},
			{"saveSettings", method(&engine::saveSettings)},
			{"addElement", method(&engine::addElement)},
		});
		return proto;
	}

	var engine::destroy(list) {
		auto win = getObject<window>("window");
		if (win) win("destroy");
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

	var engine::loadSettings(list) {
		auto configPath = getSettingsPath();
		auto configJSON = object::loadJSON(configPath);
		object config;
		if (configJSON.isObject()) {
			auto o = configJSON.getObject();
			config = o ? object(o) : config;
			cout << "Loading settings: " << configPath << endl;
			setObject("config", config);
			return var(config);
		}
		return var(config);
	}

	var engine::saveSettings(list) {
		auto config = getObject("config");
		auto win = getObject<window>("window");
		auto gfx = getObject<backend>("graphics");
		if (config && win && gfx) {
			auto windowConfigVar = win.getConfig({});
			auto windowConfig = windowConfigVar.getObject();
			if (windowConfig) {
				config.setObject("window", windowConfig);
			} else if (windowConfigVar.isError())
				return windowConfigVar;

			auto gfxConfigVar = gfx.getConfig();
			auto gfxConfig = gfxConfigVar.getObject();
			if (gfxConfig && gfxConfig.size() > 0) {
				config.setObject("graphics", gfxConfig);
			} else if (gfxConfigVar.isError())
				return gfxConfigVar;

			auto configPath = getSettingsPath();
			object::saveJSON(configPath, config);
		}
		return var();
	}

	var engine::addElement(list args) {
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto o = it->getObject();
			if (o) (*this) += {*it};
		}

		return var();
	}

	var engine::handleEntity(list args) {
		auto o = object();
		args[0].returnObject(o);
		if (o) {
			auto comps = o.getList("components");
			auto objs = o.getList("children");
			auto drawJobs = getList("drawJobs");
			auto updateJobs = getList("updateJobs");
			if (comps) {
				for (auto it = comps.begin(); it != comps.end(); ++it) {
					auto comp = object();
					it->returnObject(comp);
					if (comp) {
						if (it->isObject(component::getPrototype())) {
							auto m = comp.getMethod("update");
							if (m)
								updateJobs.pushObject(promise(comp, m, {}));
						}
						if (it->isObject(renderable::getPrototype())) {
							auto m = comp.getMethod("draw");
							if (m) drawJobs.pushObject(promise(comp, m, {0}));
						}
					}
				}
			}
			if (objs)
				for (auto it = objs.begin(); it != objs.end(); ++it)
					handleEntity({*it});
		}
		return var();
	}

	var engine::initialize(list) {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			cout << "[SDL2]" << SDL_GetError() << endl;
			return genericError(SDL_GetError());
		}
		setList("entites", list({}));
		setList("updateJobs", list({}));
		setList("drawJobs", list({}));

		auto configVar = loadSettings({});
		auto config = configVar.getObject();
		auto gameName = getString("gameName");

		auto windowConfig = config.getObject("window");
		auto backendConfig = config.getObject("graphics");
		auto win = create<window>("window", windowConfig);
		win.setTitle({gameName});
		win.create();
		auto gfx = create<backend>("graphics", backendConfig);
		return gfx.initialize({win});
	}

	void awaitList(list& promises) {
		for (auto it = promises.begin(); it != promises.end();
				 ++it) {
			auto job = it->getObject<promise>();
			job.await();
		}
		promises.resize(0);
	}

	var engine::start(list) {
		auto win = getObject<window>("window");
		auto gfx = getObject<backend>("graphics");
		gfx.setObject("window", win);

		auto entites = getList("entites");
		auto drawJobs = getList("drawJobs");
		auto updateJobs = getList("updateJobs");

		setBool("running", true);
		SDL_Event e;
		while (getBool("running")) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) setBool("running", false);
				win.handleEvent({var((void*)&e)});
			}
			gfx.preFrame();
			if (entites)
				for (auto it = entites.begin(); it != entites.end();
						 ++it)
					handleEntity({(*it)});
			awaitList(updateJobs);
			awaitList(drawJobs);
			gfx.renderFrame();
		}
		saveSettings();
		awaitList(updateJobs);
		awaitList(drawJobs);
		promise::joinThreads();
		gfx.destroy();
		destroy();
		return var();
	}

	engine::engine(string company, string gameName) : obj() {
		setParent(getPrototype());
		auto arr = list();
		setList("entites", arr);
		setString("company", company);
		setString("gameName", gameName);
		initialize();
	}

	set<string> engine::allowedConfigNames() {
		return {"window", "graphics"};
	}

	engine& engine::operator+=(list items) {
		auto a = getList("entites");
		if (!a) return *this;
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it->isObject(entity::getPrototype())) a += {*it};
		}
		return *this;
	}

	engine& engine::operator-=(list items) {
		auto a = getList("entites");
		if (!a) return *this;
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it->isObject(entity::getPrototype())) a -= {*it};
		}
		return *this;
	}

}  // namespace gold