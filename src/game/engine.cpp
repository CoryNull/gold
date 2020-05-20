
#include "engine.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#include "camera.hpp"
#include "component.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "promise.hpp"
#include "window.hpp"
#include "world.hpp"

namespace gold {
	using namespace std;
	object& engine::getPrototype() {
		static auto proto = object({
			{"initialize", method(&engine::initialize)},
			{"start", method(&engine::start)},
			{"loadSettings", method(&engine::loadSettings)},
			{"saveSettings", method(&engine::saveSettings)},
		});
		return proto;
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

	var engine::loadSettings() {
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

	var engine::saveSettings() {
		auto config = getObject("config");
		auto win = getObject<window>("window");
		auto gfx = getObject<gfxBackend>("graphics");
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

	var engine::getPrimaryCamera() {
		auto cameras = getList("cameras");
		for (auto it = cameras.begin(); it != cameras.end(); ++it) {
			auto cam = it->getObject<camera>();
			if (cam && cam.getUInt16("view") == 0) return *it;
		}
		return var();
	}

	var engine::initialize() {
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			cout << "[SDL2]" << SDL_GetError() << endl;
			return genericError(SDL_GetError());
		}
		setList("entites", list({}));

		auto configVar = loadSettings();
		auto config = configVar.getObject();
		auto gameName = getString("gameName");

		auto windowConfig = config.getObject("window");
		auto backendConfig = config.getObject("graphics");
		auto win = create<window>("window", windowConfig);
		win.setTitle({gameName});
		win.create();
		auto gfx = create<gfxBackend>("graphics", backendConfig);
		gfx.initialize({win});
		auto phys = world(obj{});
		phys.initialize({*this});
		setObject("world", phys);

		auto w = win.getUInt32("width");
		auto h = win.getUInt32("height");

		setList(
			"cameras",
			list({
				camera({
					{"size", vec2f(w, h)},
				}),
			}));

		return var();
	}

	void awaitList(list& promises) {
		for (auto it = promises.begin(); it != promises.end();
				 ++it) {
			auto job = it->getObject<promise>();
			job.await();
		}
		promises.resize(0);
	}

	var engine::start() {
		auto win = getObject<window>("window");
		auto gfx = getObject<gfxBackend>("graphics");
		auto phys = getObject<world>("world");
		gfx.setObject("window", win);

		auto cameras = getList("cameras");
		auto comps = getList("components");

		setBool("running", true);
		SDL_Event e;
		while (getBool("running")) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) setBool("running", false);
				win.handleEvent({var((void*)&e)});
			}
			phys.step();
			gfx.preFrame();
			for (auto it = cameras.begin(); it != cameras.end();
					 ++it) {
				auto c = it->getObject<camera>();
				c.setView();
			}
			comps.sort([](var a, var b) {
				auto objA = a.getObject();
				auto objB = b.getObject();
				auto aP = objA.getUInt64("priority");
				auto bP = objB.getUInt64("priority");
				return aP < bP;
			});
			auto it = comps.begin();
			for (; it != comps.end(); ++it) {
				auto comp = it->getObject<component>();
				if (!comp.getBool("_inited")) {
					comp.callMethod("initialize");
					comp.setBool("_inited", true);
				}
			}
			it = comps.begin();
			for (; it != comps.end(); ++it) {
				auto comp = it->getObject<component>();
				auto priority = comp.getUInt64("priority");
				if (priority < priorityEnum::drawPriority) {
					comp.callMethod("update");
				} else {
					comp.callMethod("update");
					comp.callMethod("draw");
				}
			}
			phys.debugDraw();
			gfx.renderFrame();
		}
		cleanUp();
		return var();
	}

	engine::engine() : obj() {}

	engine::engine(string company, string gameName) : obj() {
		setParent(getPrototype());
		setList("entites", list({}));
		setList("cameras", list({}));
		setList("components", list({}));
		setString("company", company);
		setString("gameName", gameName);
		initialize();
	}

	set<string> engine::allowedConfigNames() {
		return {"window", "graphics"};
	}

	engine& engine::operator+=(list items) {
		auto a = getList("entites");
		auto c = getList("cameras");
		auto components = getList("components");
		if (!a) return *this;
		function<void(entity&)> forEntity = [&](entity& ent) {
			a.pushObject(ent);
			ent.setObject("engine", *this);
			auto children = ent.getList("children");
			for (auto cit = children.begin(); cit != children.end();
					 ++cit) {
				auto child = cit->getObject<entity>();
				forEntity(child);
			}
			auto comps = ent.getList("components");
			for (auto cit = comps.begin(); cit != comps.end();
					 ++cit) {
				auto comp = cit->getObject<component>();
				if (comp) {
					components.pushObject(comp);
				}
			}
		};
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it->isObject(camera::getPrototype()))
				c += {*it};
			else if (it->isObject(entity::getPrototype())) {
				auto ent = it->getObject<entity>();
				forEntity(ent);
			}
		}
		return *this;
	}

	engine& engine::operator-=(list items) {
		auto a = getList("entites");
		auto c = getList("cameras");
		auto components = getList("components");
		if (!a) return *this;
		function<void(var&)> forComp = [&](var& compVar) {
			auto comp = compVar.getObject<component>();
			if (comp) {
				comp.callMethod("destroy");
			}
			auto it = components.find(compVar);
			if (it != components.end()) components.erase(it);
		};
		function<void(var&)> forEntity = [&](var& entVar) {
			auto ent = entVar.getObject<entity>();
			if (ent) {
				auto children = ent.getList("children");
				for (auto cit = children.begin(); cit != children.end();
						 ++cit)
					forEntity(*cit);
				auto comps = ent.getList("components");
				for (auto cit = comps.begin(); cit != comps.end();
						 ++cit) {
					forComp(*cit);
				}
				ent.callMethod("destroy");
				ent.erase("engine");
			}
			auto it = a.find(entVar);
			if (it != a.end()) a.erase(it);
		};

		while (items.size() > 0) {
			auto it = items.begin();
			if (it->isObject(entity::getPrototype())) {
				forEntity(*it);
			} else if (it->isObject(component::getPrototype())) {
				forComp(*it);
			}
		}
		return *this;
	}

	void engine::cleanUp() {
		auto win = getObject<window>("window");
		auto gfx = getObject<gfxBackend>("graphics");
		auto phys = getObject<world>("world");

		saveSettings();

		auto a = getList("entites");
		auto c = getList("cameras");
		auto components = getList("components");

		function<void(var&)> forComp = [&](var& compVar) {
			auto comp = compVar.getObject<component>();
			if (comp) {
				comp.callMethod("destroy");
			}
			auto it = components.find(compVar);
			if (it != components.end()) components.erase(it);
		};
		function<void(var&)> forEntity = [&](var& entVar) {
			auto ent = entVar.getObject<entity>();
			if (ent) {
				auto children = ent.getList("children");
				for (auto cit = children.begin(); cit != children.end();
						 ++cit)
					forEntity(*cit);
				auto comps = ent.getList("components");
				for (auto cit = comps.begin(); cit != comps.end();
						 ++cit) {
					forComp(*cit);
				}
				ent.callMethod("destroy");
				ent.erase("engine");
			}
			auto it = a.find(entVar);
			if (it != a.end()) a.erase(it);
		};

		while (components.size() > 0) {
			auto it = components.begin();
			if (it->isObject(entity::getPrototype())) {
				forEntity(*it);
			} else if (it->isObject(component::getPrototype())) {
				forComp(*it);
			}
		}

		gfx.destroy();
		win.destroy();
		SDL_Quit();
		empty();
	}

}  // namespace gold