#include "Window.hpp"

Window::Window (const std::vector<std::string>& args)
: _handle{}
{
	open ();
}

Window::~Window ()
{}

void Window::open ()
{
	if (!_handle)
	{
		const auto q = SDL_WINDOWPOS_CENTERED;
		const auto w = 3u*CoreConfig::ctHorizontalPixels;
		const auto h = 3u*CoreConfig::ctVerticalPixels;
		_handle = SDL_CreateWindow ("Subor", q, q, w, h, SDL_WINDOW_RESIZABLE);
		SDL_SetWindowMinimumSize (_handle, w, h);
	}
}

void Window::close ()
{
	if (_handle)
	{
		SDL_DestroyWindow (_handle);
		_handle = nullptr;
	}
}
