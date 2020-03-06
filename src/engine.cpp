
#include "engine.hpp"
#include <SDL2/SDL.h>
#include "memory.hpp"

namespace red {
	object* engineProto = NULL;

	array* engineStart(object* self, array* args) {
		if (!self)
			return NULL;
		
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
			printf("[SDL2] %s\n", SDL_GetError());
			return NULL;
		}

		int32_t width = self->getInt32("width", 1360);
		int32_t height = self->getInt32("height", 800);

		uint32_t flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN |
										 SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS |
										 SDL_WINDOW_ALLOW_HIGHDPI;

		SDL_Window* window =
				SDL_CreateWindow("RED2D", SDL_WINDOWPOS_CENTERED,
												 SDL_WINDOWPOS_CENTERED, width, height, flags);
		self->setUInt64("window", (uint64_t)window);

		self->setBool("running", true);
		SDL_Event e;
		while (true) {
			SDL_PollEvent(&e);
			if (!self->getBool("running", false) || e.type == SDL_QUIT) {
				break;
			}
		}
		return NULL;
	}

	array* engineOnDestroy(object* self, array* args) {
		SDL_Window* window = (SDL_Window*)self->getUInt64("window");
		if (window)
			SDL_DestroyWindow(window);
		self->setBool("running", false);
		SDL_Quit();
		return NULL;
	}

	object* getEnginePrototype() {
		if (engineProto)
			return engineProto;
		auto address = (object*)alloc(sizeof(engine), typeObject);
		engineProto = new (address) object();
		engineProto->setInt32("width", 1360);
		engineProto->setInt32("height", 800);
		engineProto->setMethod("start", engineStart);
		engineProto->setMethod("destroy", engineOnDestroy);
		return engineProto;
	}

	engine::engine() : object(getEnginePrototype()) {

	}

}  // namespace red