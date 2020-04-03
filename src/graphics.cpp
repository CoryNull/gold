#include "graphics.hpp"

#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/os.h>

#include <algorithm>
#include <cctype>
#include <iostream>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
#include <wayland-egl.h>
#endif
#elif BX_PLATFORM_WINDOWS
#define SDL_MAIN_HANDLED
#endif

#include "SDL2/SDL.h"

namespace red {
	bgfx::PlatformData pd = bgfx::PlatformData();
	object backend::proto =
		object({{"backend", "OpenGL"},
						{"vSync", true},
						{"maxAnisotropy", false},
						{"stats", false},
						{"debug", false},
						{"initialize", (method)backend::initialize},
						{"renderFrame", (method)backend::renderFrame},
						{"preFrame", (method)backend::preFrame},
						{"destroy", (method)backend::destroy},
						{"getConfig", (method)backend::getConfig}});

	object defaultBackendConfig =
		object({{"backend", "OpenGL"},
						{"vSync", true},
						{"maxAnisotropy", false},
						{"stats", false},
						{"debug", false}});

	bgfx::RendererType::Enum varToRenderType(var arg) {
		if (arg.isNumber()) {
			return (bgfx::RendererType::Enum)arg.getUInt8();
		} else if (arg.isString()) {
			auto str = string(arg.getString());
			std::transform(
				str.begin(), str.end(), str.begin(),
				[](unsigned char c) { return std::tolower(c); });
			if (str == "noop")
				return bgfx::RendererType::Noop;
			else if (str == "direct3d9")
				return bgfx::RendererType::Direct3D9;
			else if (str == "direct3d11")
				return bgfx::RendererType::Direct3D11;
			else if (str == "direct3d12")
				return bgfx::RendererType::Direct3D12;
			else if (str == "gnm")
				return bgfx::RendererType::Gnm;
			else if (str == "metal")
				return bgfx::RendererType::Metal;
			else if (str == "nvn")
				return bgfx::RendererType::Nvn;
			else if (str == "opengles")
				return bgfx::RendererType::OpenGLES;
			else if (str == "opengl")
				return bgfx::RendererType::OpenGL;
			else if (str == "vulkan")
				return bgfx::RendererType::Vulkan;
		}
		return bgfx::RendererType::Noop;
	}

	var backend::initialize(object& self, var args) {
		cout << "Starting BGFX" << endl;
		auto win = (window*)args.getObject();
		auto handle = (SDL_Window*)win->getPtr("handle");
		self.setObject("window", *win);
		SDL_SysWMinfo wmi;
		SDL_VERSION(&wmi.version);
		SDL_GetWindowWMInfo(handle, &wmi);

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
		wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(
			_window, "wl_egl_window");
		if (!win_impl) {
			int width, height;
			SDL_GetWindowSize(_window, &width, &height);
			struct wl_surface* surface = wmi.info.wl.surface;
			if (surface) {
				win_impl = wl_egl_window_create(surface, width, height);
				SDL_SetWindowData(_window, "wl_egl_window", win_impl);
				pd.nwh = (void*)(uintptr_t)win_impl;
				pd.ndt = wmi.info.wl.display;
			}
		}
#else
		pd.nwh = (void*)wmi.info.x11.window;
		pd.ndt = wmi.info.x11.display;
#endif
#elif BX_PLATFORM_OSX
		pd.nwh = wmi.info.cocoa.window;
		pd.ndt = nullptr;
#elif BX_PLATFORM_WINDOWS
		pd.nwh = wmi.info.win.window;
		pd.ndt = nullptr;
#endif  // BX_PLATFORM_
		pd.context = nullptr;
		pd.backBuffer = nullptr;
		pd.backBufferDS = nullptr;
		bgfx::setPlatformData(pd);
		bgfx::Init init = bgfx::Init();
		init.platformData = pd;
		auto vSync = self.getBool("vSync");
		auto maxAni = self.getBool("maxAnisotropy");
		auto bType = self.getVar("backend");
		init.type = varToRenderType(bType);
		init.vendorId = BGFX_PCI_ID_NONE;
		init.resolution.width = win->getUInt32("width");
		init.resolution.height = win->getUInt32("height");
		init.resolution.reset =
			(vSync ? BGFX_RESET_VSYNC : 0) |
			(maxAni ? BGFX_RESET_MAXANISOTROPY : 0);
		auto success = bgfx::init(init);
		if (success) {
			auto debug = self.getBool("debug");
			auto stats = self.getBool("stats");
			bgfx::setDebug(
				(debug ? BGFX_DEBUG_TEXT : 0) |
				(stats ? BGFX_DEBUG_STATS : 0));
			bgfx::setViewClear(
				0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff,
				1.0f, 0);
		} else {
			cerr << "Failed to start BGFX" << endl;
		}
	}

	var backend::renderFrame(object& self, var args) {
		return bgfx::frame(false);
	}

	var backend::getConfig(object& self, var args) {
		auto allowed = defaultBackendConfig;
		auto config = object();
		for (auto it = self.begin(); it != self.end(); ++it) {
			auto def = allowed[it->first];
			if (def.getType() != typeNull && it->second != def)
				config.setVar(it->first, it->second);
		}
		return config;
	}

	var backend::destroy(object& self, var args) {
		bgfx::shutdown();
	}

	var backend::preFrame(object& self, var args) {
		auto win = self.getObject("window");
		auto width = win->getUInt16("width");
		auto height = win->getUInt16("height");
		bgfx::setViewRect(0, 0, 0, width, height);
		bgfx::touch(0);
	}

	backend::backend() : object(&proto) {}

	backend::backend(object config) : object(config, &proto) {}
}  // namespace red
