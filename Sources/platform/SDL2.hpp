#pragma once

#define SDL_MAIN_HANDLED
#if __has_include(<SDL.h>)
	#include <SDL.h>
#elif __has_include(<SDL2/SDL.h>)
	#include <SDL2/SDL.h>
#else
	#error SDL2 wasn't found in the include path
#endif
