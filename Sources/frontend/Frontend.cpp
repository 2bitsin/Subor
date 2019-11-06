#include "Frontend.hpp"

Frontend::Frontend (const Options& args)
: _window{nullptr}
{
	open ();
}

Frontend::~Frontend ()
{}

void Frontend::open ()
{
	if (!_window)
	{
		const auto q = SDL_WINDOWPOS_CENTERED;
		const auto w = 3u*CoreConfig::ctHorizontalPixels;
		const auto h = 3u*CoreConfig::ctVerticalPixels;
		_window = SDL_CreateWindow ("Subor", q, q, w, h, SDL_WINDOW_RESIZABLE);
		SDL_SetWindowMinimumSize (_window, w, h);
	}
}

void Frontend::close ()
{
	if (_window)
	{
		SDL_DestroyWindow (_window);
		_window = nullptr;
	}
}
