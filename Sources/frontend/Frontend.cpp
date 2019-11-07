#include "Frontend.hpp"

Frontend::Frontend (const Options& args)
	: _window{nullptr}
{
	open ();
}

Frontend::~Frontend ()
{
	if (_window)
		close ();
}

void Frontend::open ()
{
	if (!_window)
	{
		const auto q = SDL_WINDOWPOS_CENTERED;
		const auto w = 1u * CoreConfig::ctHorizontalPixels;
		const auto h = 1u * CoreConfig::ctVerticalPixels;
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

void Frontend::pushFrame (const AudioBuffer<float>& audio,
	const PixelBuffer<dword>& video)
{
	if (_mutex_buff.try_lock())
	{
		_audio_buff = &audio;
		_pixel_buff = &video;
		_mutex_buff.unlock();
	}
}

int Frontend::mainThread ()
{
	SDL_Event event;

	bool update_window = false;

	for (;;)
	{
		if (update_window)
		{
			SDL_UpdateWindowSurface(_window);
			update_window = false;
		}

		if (SDL_PollEvent (&event))
		{
			if (event.type == SDL_QUIT)
				break;
		}
		
		std::lock_guard _ {_mutex_buff};

		if (_pixel_buff)
		{
			_pixel_buff->blit_to(_window);
			_pixel_buff = nullptr;
			update_window = true;
		}

		if (_audio_buff)
		{	
			// Consume audio
			_audio_buff = nullptr;
		}
	}

	return 0;
}
