#include "window.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <map>

#include "array.hpp"

namespace gold {
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
		{"setSize", method(&window::setSize)},
		{"setPos", method(&window::setPos)},
		{"setTitle", method(&window::setTitle)},
		{"setFullscreen", method(&window::setFullscreen)},
		{"setBorderless", method(&window::setBorderless)},
		{"create", method(&window::create)},
		{"destroy", method(&window::destroy)},
		{"handleEvent", method(&window::handleEvent)},
		{"getConfig", method(&window::getConfig)},
	});

	auto windowConfigDefault = object({
		{"x", SDL_WINDOWPOS_CENTERED},
		{"y", SDL_WINDOWPOS_CENTERED},
		{"width", 1360},
		{"height", 800},
		{"fullscreen", false},
		{"borderless", false},
		{"matchDesktop", false},
	});

	const auto DefaultWindowFlags =
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN |
		SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS |
		SDL_WINDOW_ALLOW_HIGHDPI;

	var window::setSize(varList args) {
		auto handle = (SDL_Window*)getPtr("handle");
		int32_t width = SDL_WINDOWPOS_CENTERED;
		int32_t height = SDL_WINDOWPOS_CENTERED;

		if (args[0].getType() == typeArray) {
			auto array = args[0].getArray();
			if (array->getType(0) == typeInt32)
				width = array->getInt32(0);
			if (array->getType(1) == typeInt32)
				height = array->getInt32(1);
		}
		setInt32("width", width);
		setInt32("height", height);
		if (handle != nullptr)
			SDL_SetWindowSize(handle, width, height);
		return var();
	}

	var window::setPos(varList args) {
		auto handle = (SDL_Window*)getPtr("handle");
		int32_t x = SDL_WINDOWPOS_CENTERED;
		int32_t y = SDL_WINDOWPOS_CENTERED;

		if (args[0].getType() == typeArray) {
			auto array = args[0].getArray();
			if (array->getType(0) == typeInt32)
				x = array->getInt32(0);
			if (array->getType(1) == typeInt32)
				y = array->getInt32(1);
		}
		setInt32("x", x);
		setInt32("y", y);
		if (handle != nullptr) {
			SDL_SetWindowPosition(handle, x, y);
		}
		return var();
	}

	var window::setTitle(varList args) {
		auto handle = (SDL_Window*)getPtr("handle");
		string title;
		if (args[0].getType() == typeString)
			title = args[0].getString();
		if (title.size() > 0) {
			setString("title", title);
			if (handle != nullptr)
				SDL_SetWindowTitle(handle, title.c_str());
		} else
			setNull("title");
		return var();
	}

	var window::setFullscreen(varList args) {
		auto handle = (SDL_Window*)getPtr("handle");
		bool fullscreen = false;
		bool desktop = false;
		if (args[0].getType() == typeBool)
			fullscreen = (bool)args[0];
		else if (args[0].getType() == typeArray) {
			auto array = args[0].getArray();
			if (array->getType(0) == typeBool)
				fullscreen = array->getBool(0);
			if (array->getType(1) == typeBool)
				desktop = array->getBool(1);
		}
		setBool("fullscreen", fullscreen);
		setBool("desktop", desktop);
		if (handle)
			SDL_SetWindowFullscreen(
				handle,
				(fullscreen ? (desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP
															 : SDL_WINDOW_FULLSCREEN)
										: 0));
		return var();
	}

	var window::setBorderless(varList args) {
		auto handle = (SDL_Window*)getPtr("handle");
		bool borderless = false;
		if (args[0].getType() == typeBool)
			borderless = args[0].getBool();
		setBool("borderless", borderless);
		if (handle)
			SDL_SetWindowBordered(handle, (SDL_bool)(!borderless));
		return var();
	}

	var window::create(varList) {
		if (getType("handle") == typePtr) callMethod("destroy");
		int32_t windowX = getInt32("x");
		int32_t windowY = getInt32("y");
		int32_t width = getInt32("width");
		int32_t height = getInt32("height");
		bool fullscreen = getBool("fullscreen");
		bool borderless = getBool("borderless");
		bool maximize = getBool("maximize");
		bool desktop = getBool("desktop");
		auto title = getString("title");

		uint32_t flags =
			DefaultWindowFlags |
			(fullscreen ? (desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP
														 : SDL_WINDOW_FULLSCREEN)
									: 0) |
			(borderless ? SDL_WINDOW_BORDERLESS : 0) |
			(maximize ? SDL_WINDOW_MAXIMIZED : 0);

		SDL_Window* window = SDL_CreateWindow(
			title.c_str(), windowX, windowY, width, height, flags);
		SDL_SetWindowData(window, "object", this);

		setPtr("handle", window);
		return var();
	}

	var window::destroy(varList) {
		SDL_Window* window = (SDL_Window*)getPtr("handle");
		if (window != nullptr) SDL_DestroyWindow(window);
		return var();
	}

	var window::handleEvent(varList args) {
		if (args[0].getType() == typePtr) {
			auto event = (SDL_Event*)args[0].getPtr();
			auto window = (SDL_Window*)getPtr("handle");
			if (event && event->type == SDL_WINDOWEVENT) {
				switch (event->window.event) {
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						setBool("hidden", false);
						break;
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_HIDDEN:
						setBool("hidden", true);
						break;
					case SDL_WINDOWEVENT_MOVED:
						setInt32("x", event->window.data1);
						setInt32("y", event->window.data2);
						break;
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED: {
						setInt32("width", event->window.data1);
						setInt32("height", event->window.data2);
						auto flags = SDL_GetWindowFlags(window);
						setBool(
							"fullscreen", flags & SDL_WINDOW_FULLSCREEN);
						setBool(
							"desktop", flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
						setBool("maximize", flags & SDL_WINDOW_MAXIMIZED);
						break;
					}
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						setBool("active", true);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						setBool("active", false);
						break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
					case SDL_WINDOWEVENT_TAKE_FOCUS:
						setBool("active", true);
						break;
					case SDL_WINDOWEVENT_HIT_TEST:
						setBool("active", true);
						break;
#endif
					default:
						break;
				}
			}
		}
		return var();
	}

	var window::getConfig(varList) {
		auto allowed = windowConfigDefault;
		auto config = object();
		for (auto it = begin(); it != end(); ++it) {
			auto def = allowed[it->first];
			if (def.getType() != typeNull && it->second != def)
				config.setVar(it->first, it->second);
		}
		return config;
	}

	window::window() : object(proto) { create(); }

	window::window(object config) : object(config, &proto) {
		create();
	}

}  // namespace gold