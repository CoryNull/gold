
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
			{"destroy", method(&engine::destroy)},
			{"loadSettings", method(&engine::loadSettings)},
			{"handleEntity", method(&engine::handleEntity)},
			{"saveSettings", method(&engine::saveSettings)},
		});
		return proto;
	}

	void engine::registerComponent(component& comp) {
		auto callbacks = getList("registerCallbacks");
		for (auto it = callbacks.begin(); it != callbacks.end();
				 ++it) {
			auto pair = it->getList();
			auto proto = pair.getObject<component>(0);
			if (comp.getParent() == proto) {
				auto fu = pair.getFunction(1);
				fu({comp});
			}
		}
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

	var engine::getPrimaryCamera(list args) {
		auto cameras = getList("cameras");
		for (auto it = cameras.begin(); it != cameras.end(); ++it) {
			auto cam = it->getObject<camera>();
			if (cam && cam.getUInt16("view") == 0) return *it;
		}
		return var();
	}

	var engine::handleEntity(list args) {
		auto o = args[0].getObject();
		if (o) {
			auto comps = o.getList("components");
			auto objs = o.getList("children");
			if (comps) {
				for (auto it = comps.begin(); it != comps.end(); ++it) {
					auto comp = it->getObject();
					if (comp) {
						if (it->isObject(component::getPrototype())) {
							auto m = comp.getMethod("update");
							if (m) (comp.*m)({});
						}
						if (it->isObject(renderable::getPrototype())) {
							auto m = comp.getMethod("draw");
							if (m) {
								auto views = comp.getList("view");
								for (auto vit = views.begin();
										 vit != views.end();
										 ++vit)
									(comp.*m)({vit->getUInt16()});
							}
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

		auto configVar = loadSettings({});
		auto config = configVar.getObject();
		auto gameName = getString("gameName");

		auto windowConfig = config.getObject("window");
		auto backendConfig = config.getObject("graphics");
		auto win = create<window>("window", windowConfig);
		win.setTitle({gameName});
		win.create();
		auto gfx = create<backend>("graphics", backendConfig);
		gfx.initialize({win});
		auto phys = create<world>("world", object{});
		phys.initialize({*this});

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

	var engine::start(list) {
		auto win = getObject<window>("window");
		auto gfx = getObject<backend>("graphics");
		gfx.setObject("window", win);

		auto entites = getList("entites");
		auto cameras = getList("cameras");

		setBool("running", true);
		SDL_Event e;
		while (getBool("running")) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) setBool("running", false);
				win.handleEvent({var((void*)&e)});
			}
			gfx.preFrame();
			for (auto it = cameras.begin(); it != cameras.end();
					 ++it) {
				auto c = it->getObject<camera>();
				c.setView();
			}
			for (auto it = entites.begin(); it != entites.end(); ++it)
				handleEntity({(*it)});
			gfx.renderFrame();
		}
		saveSettings();
		gfx.destroy();
		destroy();
		return var();
	}

	var engine::addRegisterCompnentCallback(list args) {
		auto callbacks = getList("registerCallbacks");
		auto obit = args.find(typeObject);
		auto comp = component();
		if (obit != args.end()) comp = obit->getObject<component>();
		auto fuit = args.find(typeFunction);
		func fu;
		if (fuit != args.end()) fu = fuit->getFunction();
		callbacks.pushList(list{comp, fu});
		return var();
	}

	engine::engine() : obj() {}

	engine::engine(string company, string gameName) : obj() {
		setParent(getPrototype());
		setList("entites", list());
		setList("cameras", list());
		setList("registerCallbacks", list());
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
		if (!a) return *this;
		function<void(entity&)> forEntity = [&](entity& ent) {
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
				if (comp) registerComponent(comp);
			}
		};
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it->isObject(camera::getPrototype()))
				c += {*it};
			else if (it->isObject(entity::getPrototype())) {
				a += {*it};
				auto ent = it->getObject<entity>();
				forEntity(ent);
			}
		}
		return *this;
	}

	engine& engine::operator-=(list items) {
		auto a = getList("entites");
		auto c = getList("cameras");
		if (!a) return *this;
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (it->isObject(camera::getPrototype()))
				c -= {*it};
			else if (it->isObject(entity::getPrototype()))
				a -= {*it};
		}
		return *this;
	}

}  // namespace gold