
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

namespace red {
	using namespace std;
	object engine::proto =
		object({{"start", engine::start},
						{"destroy", engine::destroy},
						{"loadSettings", engine::loadSettings},
						{"handleEntity", engine::handleEntity},
						{"saveSettings", engine::saveSettings},
						{"addElement", engine::addElement}});

	var engine::destroy(object& self, var args) {
		auto window = self.getObject("window");
		if (window != nullptr) (*window)("destroy");
		self.setNull("window");
		self.setBool("running", false);
		SDL_Quit();
	}

	string engine::getSettingsDir() {
		auto dir =
			string(SDL_GetPrefPath("MountainAndValley", "RED2D"));
		if (dir.size() == 0) dir = "./";
		return dir;
	}

	string engine::getSettingsPath() {
		return string(engine::getSettingsDir() + "config.json");
	}

	var engine::loadSettings(object& self, var args) {
		auto configPath = engine::getSettingsPath();
		auto configJSON = object::loadJSON(configPath);
		object config;
		if (configJSON.isObject()) {
			auto o = configJSON.getObject();
			config = o ? object(*o) : config;
			cout << "Loading settings: " << configPath << endl;
			self.setObject("config", config);
			return var(config);
		}
		return var(config);
	}

	var engine::saveSettings(object& self, var args) {
		auto config = self.getObject("config");
		auto win = (window*)self.getObject("window");
		auto gfx = (backend*)self.getObject("graphics");
		if (config && win && gfx) {
			auto windowConfigVar = (*win)("getConfig");
			auto windowConfig = windowConfigVar.getObject();
			if (windowConfig != nullptr) {
				config->setObject("window", *windowConfig);
			} else if (windowConfigVar.isError())
				return windowConfigVar;

			auto gfxConfigVar = (*gfx)("getConfig");
			auto gfxConfig = gfxConfigVar.getObject();
			if (gfxConfig != nullptr && gfxConfig->getSize() > 0) {
				config->setObject("graphics", *gfxConfig);
			} else if (gfxConfigVar.isError())
				return gfxConfigVar;

			auto configPath = engine::getSettingsPath();
			object::saveJSON(configPath, *config);
		}
	}

	var engine::addElement(object& self, var args) {
		auto eng = (engine*)(&self);
		if (self.getParent() == &proto && args.isObject())
			eng += args;
	}

	var engine::handleEntity(object& self, var args) {
		auto eng = (engine*)(&self);
		if (args.isObject(entity::proto)) {
			auto obj = args.getObject();
			auto comps = obj->getArray("components");
			auto objs = obj->getArray("children");
			if (comps) {
				for (auto it = comps->begin(); it != comps->end();
						 ++it) {
					auto comp = it->getObject();
					if (comp) {
						if (it->isObject(component::proto))
							eng->updateWorker.add(
								comp->getMethod("update"), *comp, var());
						if (it->isObject(renderable::proto))
							eng->drawWorker.add(
								comp->getMethod("draw"), *comp, var());
					}
				}
			}
			if (objs)
				for (auto it = objs->begin(); it != objs->end(); ++it)
					self("handleEntity", *it);
		}
	}

	var engine::start(object& self, var args) {
		auto eng = (engine*)(&self);
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			cout << "[SDL2]" << SDL_GetError() << endl;
			return runtime_error(SDL_GetError());
		}

		auto configVar = self("loadSettings");
		auto config = *configVar.getObject();

		auto windowConfig = config.getObject("window");
		auto backendConfig = config.getObject("graphics");
		auto win = self.create<window>("window", windowConfig);
		auto gfx = self.create<backend>("graphics", backendConfig);
		gfx("initialize", win);

		auto entites = self.getArray("entites");

		self.setBool("running", true);
		SDL_Event e;
		while (self.getBool("running")) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) self.setBool("running", false);
				win("handleEvent", (void*)&e);
			}
			gfx("preFrame");
			if (entites)
				for (auto it = entites->begin(); it != entites->end();
						 ++it)
					self("handleEntity", (*it));
			eng->updateWorker.wait();
			eng->drawWorker.wait();
			gfx("renderFrame");
		}
		self("saveSettings");
		eng->updateWorker.wait();
		eng->drawWorker.wait();
		eng->updateWorker.killAll();
		eng->drawWorker.killAll();
		gfx("destroy");
		self("destroy");
	}

	engine::engine()
		: object(&proto), updateWorker(), drawWorker() {
		setArray("entites", array());
	}

	set<string> engine::allowedConfigNames() {
		return {"window"};
	}

	engine& engine::operator+=(var element) {
		auto a = getArray("entites");
		if (a == nullptr) return *this;
		if (element.isObject(entity::proto)) (*a) += element;
		return *this;
	}

	engine& engine::operator-=(var element) {
		auto a = getArray("entites");
		if (a == nullptr) return *this;
		if (element.isObject(entity::proto)) (*a) -= element;
		return *this;
	}

}  // namespace red