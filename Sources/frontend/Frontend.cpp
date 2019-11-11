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
	if (SDL_IsGameController(0))
		_gctrl0.emplace(0);
	if (SDL_IsGameController(1))
		_gctrl1.emplace(1);
}

Frontend::~Frontend ()
{
	SDL_DestroyWindow (_window);
	_window = nullptr;
}

bool Frontend::pushFrame (Backend& backend, const AudioVideoFrame& frame)
{
	if (_lockFrame.try_lock ())
	{
		_frame = &frame;
		_lockFrame.unlock ();
		backend.input(_inpst0.load(), _inpst1.load());
		return true;
	}
	return false;
}

void Frontend::consume ()
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
	switch(ev.type)
	{
	case SDL_CONTROLLERAXISMOTION:
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		if (_gctrl0.has_value() 
		  &&_gctrl0->update(ev))
			_inpst0.store (_gctrl0->read().bits);
		if (_gctrl1.has_value() 
		  &&_gctrl1->update(ev))
			_inpst1.store (_gctrl1->read().bits);
		break;
	}

	consume();
	return 0;
}

SDL_Window* Frontend::handle () const
{
	return _window;
}

int Frontend::mainthread ()
{

	SDL_Event event;
	for (;;)
	{
		if (SDL_PollEvent (&event))
			if (event.type == SDL_QUIT)
				break;
		consume();
	}

	return 0;
}
