#include "window.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <map>

#include "array.hpp"

namespace red {
	using namespace std;
	object window::proto = object({
			{"x", SDL_WINDOWPOS_CENTERED},
			{"y", SDL_WINDOWPOS_CENTERED},
			{"width", 1360},
			{"height", 800},
			{"maximize", false},
			{"fullscreen", false},
			{"borderless", false},
			{"matchDesktop", false},
			{"title", (char*)"RED2D"},
			{"setSize", window::setSize},
			{"setPos", window::setPos},
			{"setTitle", window::setTitle},
			{"setFullscreen", window::setFullscreen},
			{"setBorderless", window::setBorderless},
			{"create", window::create},
			{"destroy", window::destroy},
			{"handleEvent", window::handleEvent},
			{"getConfig", window::getConfig},
	});
	auto windowConfigDefault = object({{"x", SDL_WINDOWPOS_CENTERED},
																		 {"y", SDL_WINDOWPOS_CENTERED},
																		 {"width", 1360},
																		 {"height", 800},
																		 {"fullscreen", false},
																		 {"borderless", false},
																		 {"matchDesktop", false}});

	const auto DefaultWindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN |
																	SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS |
																	SDL_WINDOW_ALLOW_HIGHDPI;

	var window::setSize(object& self, var& args) {
		auto handle = (SDL_Window*)self.getPtr("handle");
		int32_t width, height = SDL_WINDOWPOS_CENTERED;

		if (args.getType() == typeArray) {
			auto array = args.getArray();
			if (array->getType(0) == typeInt32) width = array->getInt32(0);
			if (array->getType(1) == typeInt32) height = array->getInt32(1);
		}
		self.setInt32("width", width);
		self.setInt32("height", height);
		if (handle != nullptr) SDL_SetWindowSize(handle, width, height);
	}

	var window::setPos(object& self, var& args) {
		auto handle = (SDL_Window*)self.getPtr("handle");
		int32_t x, y = SDL_WINDOWPOS_CENTERED;

		if (args.getType() == typeArray) {
			auto array = args.getArray();
			if (array->getType(0) == typeInt32) x = array->getInt32(0);
			if (array->getType(1) == typeInt32) y = array->getInt32(1);
		}
		self.setInt32("x", x);
		self.setInt32("y", y);
		if (handle != nullptr) {
			SDL_SetWindowPosition(handle, x, y);
		}
	}

	var window::setTitle(object& self, var& args) {
		auto handle = (SDL_Window*)self.getPtr("handle");
		char* title = nullptr;
		if (args.getType() == typeString) title = (char*)args.getString();
		if (title != nullptr) {
			self.setString("title", title);
			if (handle != nullptr) SDL_SetWindowTitle(handle, title);
		} else
			self.setNull("title");
	}

	var window::setFullscreen(object& self, var& args) {
		auto handle = (SDL_Window*)(void*)self["handle"];
		bool fullscreen = false;
		bool desktop = false;
		if (args.getType() == typeBool)
			fullscreen = (bool)args;
		else if (args.getType() == typeArray) {
			auto array = args.getArray();
			if (array->getType(0) == typeBool) fullscreen = array->getBool(0);
			if (array->getType(1) == typeBool) desktop = array->getBool(1);
		}
		self.setBool("fullscreen", fullscreen);
		self.setBool("desktop", desktop);
		if (handle)
			SDL_SetWindowFullscreen(
					handle,
					(fullscreen ? (desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN)
											: 0));
	}

	var window::setBorderless(object& self, var& args) {
		auto handle = (SDL_Window*)(void*)self["handle"];
		bool borderless = false;
		if (args.getType() == typeBool) borderless = args.getBool();
		self.setBool("borderless", borderless);
		if (handle) SDL_SetWindowBordered(handle, (SDL_bool)(!borderless));
	}

	var window::create(object& self, var& args) {
		if (self.getType("handle") == typePtr) self("destroy");
		int32_t windowX = self.getInt32("x");
		int32_t windowY = self.getInt32("y");
		int32_t width = self.getInt32("width");
		int32_t height = self.getInt32("height");
		int32_t refreshRate = self.getInt32("refreshRate");
		bool fullscreen = self.getBool("fullscreen");
		bool borderless = self.getBool("borderless");
		bool maximize = self.getBool("maximize");
		bool desktop = self.getBool("desktop");
		auto title = self.getString("title");

		uint32_t flags =
				DefaultWindowFlags |
				(fullscreen ? (desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN)
										: 0) |
				(borderless ? SDL_WINDOW_BORDERLESS : 0) | (maximize ? SDL_WINDOW_MAXIMIZED : 0);

		SDL_Window* window = SDL_CreateWindow(title, windowX, windowY, width, height, flags);
		SDL_SetWindowData(window, "object", &self);

		self.setPtr("handle", window);
	}

	var window::destroy(object& self, var& args) {
		SDL_Window* window = (SDL_Window*)(void*)self["handle"];
		if (window != nullptr) SDL_DestroyWindow(window);
	}

	var window::handleEvent(object& self, var& args) {
		if (args.getType() == typePtr) {
			auto event = (SDL_Event*)args.getPtr();
			auto window = (SDL_Window*)self.getPtr("handle");
			if (event && event->type == SDL_WINDOWEVENT) {
				switch (event->window.event) {
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						self.setBool("hidden", false);
						break;
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_HIDDEN:
						self.setBool("hidden", true);
						break;
					case SDL_WINDOWEVENT_MOVED:
						self.setInt32("x", event->window.data1);
						self.setInt32("y", event->window.data2);
						break;
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED: {
						self.setInt32("width", event->window.data1);
						self.setInt32("height", event->window.data2);
						auto flags = SDL_GetWindowFlags(window);
						self.setBool("fullscreen", flags & SDL_WINDOW_FULLSCREEN);
						self.setBool("desktop", flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
						self.setBool("maximize", flags & SDL_WINDOW_MAXIMIZED);
						break;
					}
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						self.setBool("active", true);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						self.setBool("active", false);
						break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
					case SDL_WINDOWEVENT_TAKE_FOCUS:
						self.setBool("active", true);
						break;
					case SDL_WINDOWEVENT_HIT_TEST:
						self.setBool("active", true);
						break;
#endif
					default:
						break;
				}
			}
		}
		return var();
	}

	var window::getConfig(object& self, var& args) {
		auto allowed = windowConfigDefault;
		auto config = object();
		for (auto it = self.begin(); it.operator!=(self.end()); ++it) {
			auto def = allowed[it->first];
			if (!def.isEmpty() && it->second != def) config.setVar(it->first, it->second);
		}
		return var(config);
	}

	window::window() : object(proto) { (*this)("create"); }

	window::window(object& config) : object(config, &proto) { (*this)("create"); }

}  // namespace red