
#include "engine.hpp"
#include <SDL2/SDL.h>

redObject* engineProto = NULL;

redArray* redEngineStart(redObject* self, redArray* args) {
	if (!self)
		return NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) != SDL_FALSE) {
		printf("[SDL2] %s\n", SDL_GetError());
	}

	int32_t width = redGetObjectInt32(engineProto, "width", 1360);
	int32_t height = redGetObjectInt32(engineProto, "height", 800);

	uint32_t flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN |
									 SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS |
									 SDL_WINDOW_ALLOW_HIGHDPI;

	SDL_Window* window =
			SDL_CreateWindow("RED2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
											 width, height, flags);
	redSetObjectUInt64(self, "window", (uint64_t)window);

	redSetObjectBool(self, "running", true);
	SDL_Event e;
	while (true) {
		SDL_PollEvent(&e);
		if (!redGetObjectBool(self, "running", false) || e.type == SDL_QUIT) {
			break;
		}
	}
	return NULL;
}

redArray* redEngineOnDestroy(redObject* self, redArray* args) {
	SDL_Window* window = (SDL_Window*)redGetObjectUInt64(self, "window", 0);
	if (window)
		SDL_DestroyWindow(window);
	redSetObjectBool(self, "running", false);
	SDL_Quit();
	return NULL;
}

redObject* redGetEnginePrototype() {
	if (engineProto)
		return engineProto;
	engineProto = redNewObject(0);
	redSetObjectInt32(engineProto, "width", 1360);
	redSetObjectInt32(engineProto, "height", 800);
	redSetObjectMethod(engineProto, "start", redEngineStart);
	redSetObjectMethod(engineProto, "destroy", redEngineOnDestroy);
	return engineProto;
}

redEngine* redNewEngine() {
	redEngine* engine = redNewObject(redGetEnginePrototype());
	return engine;
}