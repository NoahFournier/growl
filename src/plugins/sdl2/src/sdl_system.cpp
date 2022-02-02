#include "sdl_system.h"
#include "sdl_window.h"
#include <SDL.h>
#include <assert.h>
#include <iostream>

using Growl::SDL2SystemAPI;
using Growl::Window;

void SDL2SystemAPI::init() {
	assert(SDL_WasInit(SDL_INIT_VIDEO) == 0);

	running = true;
}

std::unique_ptr<Window>
SDL2SystemAPI::createWindow(const WindowConfig& config) {
	int flags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_ALLOW_HIGHDPI;
	flags |= SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	SDL_Window* win = SDL_CreateWindow(
		config.getTitle().c_str(),
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0,
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0, config.getWidth(),
		config.getHeight(), flags);

	return std::make_unique<SDL2Window>(win);
}

void SDL2SystemAPI::tick() {
	SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		running = false;
	}
}

void SDL2SystemAPI::dispose() {
	SDL_Quit();
}

bool SDL2SystemAPI::isRunning() {
	return running;
}
