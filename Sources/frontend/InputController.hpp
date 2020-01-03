#pragma once

#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <utils/Types.hpp>
#include <input/InputPort.hpp>


#include <array>
#include <cassert>

struct InputController
{
	SDL_GameController* handle{nullptr};

	InputController (const Options&)
	: handle{SDL_GameControllerOpen (0)}
	{
		if (!handle)
			throw std::runtime_error("Controller `0` not found.");
	}

	~InputController ()
	{
		auto gcc = &SDL_GameControllerClose;
		if (handle != nullptr)
			gcc (handle);
	}

	bool poll(byte index, SDL_GameControllerButton gcb, bool mask = true) const
	{
		if (index == 0u)
			return false;
		constexpr auto q = &SDL_GameControllerGetButton;
		return (q(handle, gcb) && mask);
	}

	byte poll_all (byte i, bool s) const
	{
		using ip = InputPort;
		return ip::pack ({
			.a = poll (i, butt [0]) || poll (i, butt [8], s),
			.b = poll (i, butt [1]) || poll (i, butt [9], s),
			.s = poll (i, butt [2]),
			.t = poll (i, butt [3]),
			.u = poll (i, butt [4]),
			.d = poll (i, butt [5]),
			.l = poll (i, butt [6]),
			.r = poll (i, butt [7])
		});
	}

	template <typename _Host>
	bool strobe (_Host&& host) const
	{
		return !!((host.count_frames () >> 1u) & 1u);
	}

	template <typename _Host>
	auto read (_Host&& host)
		-> std::array<byte, 4u>
	{
		std::array<byte, 4u> state;

		const auto s = strobe(host);		
		for (auto i = 0; i < 4; ++i)
			state[i] = poll_all (i, s);
		return state;
	}

	using gcb = SDL_GameControllerButton;

	static inline const gcb butt [10u] =
	{
		SDL_CONTROLLER_BUTTON_B,					// A
		SDL_CONTROLLER_BUTTON_A,					// B

		SDL_CONTROLLER_BUTTON_BACK,				// Select
		SDL_CONTROLLER_BUTTON_START,			// Start
		SDL_CONTROLLER_BUTTON_DPAD_UP,		// Up
		SDL_CONTROLLER_BUTTON_DPAD_DOWN,	// Down
		SDL_CONTROLLER_BUTTON_DPAD_LEFT,	// Left
		SDL_CONTROLLER_BUTTON_DPAD_RIGHT,	// Right

		SDL_CONTROLLER_BUTTON_Y,					// Turbo A
		SDL_CONTROLLER_BUTTON_X,					// Turbo B
	};
};
