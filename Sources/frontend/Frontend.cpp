#include "Frontend.hpp"
#include <backend/Backend.hpp>

Frontend::Frontend (const Options& args)
	: _window{nullptr}
{
	const auto q = SDL_WINDOWPOS_CENTERED;
	const auto w = 1u * CoreConfig::ctHorizontalPixels;
	const auto h = 1u * CoreConfig::ctVerticalPixels;
	_window = SDL_CreateWindow ("Subor", q, q, w, h, SDL_WINDOW_RESIZABLE);
	SDL_SetWindowMinimumSize (_window, w, h);
	SDL_AddEventWatch ([] (auto data, auto event)
	{
		auto& fe = *(Frontend*)data;
		return fe.dispatch (*event);
	}, this);
}

Frontend::~Frontend ()
{
	SDL_DestroyWindow (_window);
	_window = nullptr;
}

bool Frontend::notifyFrame (const AudioVideoFrame& frame)
{
	if (_lockFrame.try_lock ())
	{
		_frame = &frame;
		_lockFrame.unlock ();
		return true;
	}
	return false;
}

void Frontend::frameConsume ()
{
	if (!_frame)
		return;
	if (_lockFrame.try_lock ())
	{
		_frame->video.blit_to (_window);
		SDL_UpdateWindowSurface (_window);
		_frame = nullptr;
		_lockFrame.unlock ();
	}
}

int Frontend::dispatch (const SDL_Event& ev)
{
	frameConsume();
	return 0;
}

SDL_Window* Frontend::handle () const
{
	return _window;
}

int Frontend::mainThread ()
{

	SDL_Event event;
	for (;;)
	{
		if (SDL_PollEvent (&event))
		{
			if (event.type == SDL_QUIT)
				break;
		}
		frameConsume();
	}

	return 0;
}
