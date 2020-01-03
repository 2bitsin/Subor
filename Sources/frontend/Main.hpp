#pragma once

#include <core/Console.hpp>
#include <test/Nestest.hpp>
#include <audio/AudioDevice.hpp>
#include <input/InputPort.hpp>
#include <input/InputProxy.hpp>
#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <frontend/InputController.hpp>

#include <exception>
#include <mutex>
#include <functional>

struct Main
{
	template <typename T>
	using auto_ptr = std::unique_ptr<T, void(*)(T*)>;
	using renderer_ptr = auto_ptr<SDL_Renderer>;
	using window_ptr = auto_ptr<SDL_Window>;

	Main (const Options& options)
	: options      { options },
		window       { create_window (options) },
		renderer     { create_renderer (window.get(), options) },
		gamepad			 { options },
		input_proxy  { gamepad, options },
		console      { std::make_unique<Console> () },
		audio_device { *this }
	{
		if (options.cmd_load ().has_value ())
			console->load (*options.cmd_load ());
		else
			console->load (nestest::get_rom ());
	}

	void audio_callback (array_view<float> buff)
	{
		try
		{
			PixelBuffer video;
			Console::AudioBuffer audio { .buff = buff };
			console->emulate (audio, video, input_proxy);
			std::lock_guard g { sync_lock };
			std::swap (video, next_frame);
		}
		catch (...)
		{
			auto ex = std::current_exception ();
			uncaught.emplace (std::move (ex));
		}
	}


	static auto create_window (const Options& opts)
		-> window_ptr
	{
		constexpr auto e = SDL_WINDOWPOS_CENTERED;
		constexpr auto f = SDL_WINDOW_RESIZABLE;
		const auto r = SDL_CreateWindow ("", e, e, 768u, 720u, f);
		return {r ? r : throw std::runtime_error (SDL_GetError ()),
			&SDL_DestroyWindow};
	}

	static auto create_renderer (SDL_Window* window, const Options& opts)
		-> renderer_ptr
	{
		assert (window != nullptr);
		constexpr auto e = SDL_RENDERER_ACCELERATED
			| SDL_RENDERER_PRESENTVSYNC;
		const auto r = SDL_CreateRenderer (window, -1, e);
		return {r ? r : throw std::runtime_error (SDL_GetError ()),
			&SDL_DestroyRenderer};
	}

	int main ()
	{
		using namespace std::chrono;
		using namespace std::string_literals;
		audio_device.start ();
		while (true)
		{
			SDL_Event ev;
			if (SDL_PollEvent (&ev))
			{
				if (ev.type == SDL_QUIT)
					break;
				continue;
			}

			{
				std::lock_guard g { sync_lock };
				next_frame.blit_to (renderer.get());
				if (uncaught.has_value ())
					std::rethrow_exception (*uncaught);
			}
			SDL_RenderPresent (renderer.get());
		}

		return 0;
	}

	~Main ()
	{}

	Options options;
	window_ptr window;
	renderer_ptr renderer;
	InputController gamepad;
	InputProxy<InputController> input_proxy;
	std::unique_ptr<Console> console;
	AudioDevice<Main> audio_device;
	std::mutex sync_lock;
	PixelBuffer next_frame;
	std::optional<std::exception_ptr> uncaught;
};
