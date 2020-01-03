#pragma once

#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <utils/Types.hpp>
#include <utils/NonCopyable.hpp>
#include <input/InputPort.hpp>


#include <array>
#include <variant>
#include <cassert>

namespace detail
{

	struct InputGamepadImpl
	:	public NonCopyable
	{
		static inline constexpr auto close = &SDL_GameControllerClose;
		static inline constexpr auto open = &SDL_GameControllerOpen;
		static inline constexpr auto get_button = &SDL_GameControllerGetButton;

		using button_type = SDL_GameControllerButton;
		using handle_type = SDL_GameController*;

		InputGamepadImpl(InputGamepadImpl&& prev)
		:	handle { std::exchange (prev.handle, nullptr) }
		{}

		InputGamepadImpl& operator = (InputGamepadImpl&& prev)
		{
			this->~InputGamepadImpl();
			new (this) InputGamepadImpl (std::move(prev));
			return *this;
		}

		bool poll_button (byte buttidx) const
		{
			static const button_type butt [10u] =
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

			return !!get_button (handle, butt [buttidx]);
		}

		InputGamepadImpl (std::size_t index = 0u)
		: handle { open ((int)index) }
		{
			if (!handle)
				throw std::runtime_error (SDL_GetError ());
		}

		~InputGamepadImpl ()
		{
			if (handle != nullptr)
				close (handle);
		}

	private:
		handle_type handle { nullptr };
	};

	struct InputKeyboardImpl
	{
		InputKeyboardImpl (std::size_t index = 0u)
		: index_ { index }
		{}

		bool poll_button (byte buttidx) const
		{
			auto keys = SDL_GetKeyboardState (nullptr);
			return keys [keytbl_ [index_][buttidx]];
		}

	private:
		std::size_t index_;

		inline static constexpr int keytbl_ [4][10] =
		{
			{
				SDL_SCANCODE_LCTRL,
				SDL_SCANCODE_LALT,
				SDL_SCANCODE_RSHIFT,
				SDL_SCANCODE_RETURN,
				SDL_SCANCODE_UP,
				SDL_SCANCODE_DOWN,
				SDL_SCANCODE_LEFT,
				SDL_SCANCODE_RIGHT,
				SDL_SCANCODE_Z,
				SDL_SCANCODE_X
			},
			{
				SDL_SCANCODE_KP_0,
				SDL_SCANCODE_KP_COMMA,
				SDL_SCANCODE_KP_ENTER,
				SDL_SCANCODE_KP_PLUS,
				SDL_SCANCODE_KP_8,
				SDL_SCANCODE_KP_2,
				SDL_SCANCODE_KP_4,
				SDL_SCANCODE_KP_6,
				SDL_SCANCODE_KP_1,
				SDL_SCANCODE_KP_3
			},
			{
				SDL_SCANCODE_LCTRL,
				SDL_SCANCODE_LALT,
				SDL_SCANCODE_RETURN,
				SDL_SCANCODE_RSHIFT,
				SDL_SCANCODE_UP,
				SDL_SCANCODE_DOWN,
				SDL_SCANCODE_LEFT,
				SDL_SCANCODE_RIGHT,
				SDL_SCANCODE_Z,
				SDL_SCANCODE_X
			},
			{
				SDL_SCANCODE_KP_0,
				SDL_SCANCODE_KP_COMMA,
				SDL_SCANCODE_KP_ENTER,
				SDL_SCANCODE_KP_PLUS,
				SDL_SCANCODE_KP_8,
				SDL_SCANCODE_KP_2,
				SDL_SCANCODE_KP_4,
				SDL_SCANCODE_KP_6,
				SDL_SCANCODE_KP_1,
				SDL_SCANCODE_KP_3
			}
		};
	};

	struct DisconnectedImpl
	{
		bool poll_button (byte buttidx) const
		{
			return false;
		}
	};

}
struct InputController
{
	using impl = std::variant
	<	detail::InputGamepadImpl,
		detail::InputKeyboardImpl,
		detail::DisconnectedImpl>;

	bool poll_button (byte port_index, byte button_index) const
	{
		return std::visit ([button_index] (auto&& port) {		
			return port.poll_button(button_index);
		}, ports[port_index]);
	}

	static inline impl connect_controller (const Options&, std::size_t index)
	{
		using namespace detail;
		try { return InputGamepadImpl  { index }; } catch (...) {}
		try { return InputKeyboardImpl { index }; } catch (...) {}
		return DisconnectedImpl {};
	}

	InputController (const Options& options)
	:	ports 
		{
			connect_controller(options, 0),
			connect_controller(options, 1),
			connect_controller(options, 2),
			connect_controller(options, 3)
		}
	{}

	byte poll_every_button (byte port_index, bool rapid_fire_clock) const
	{
		return std::visit ([rapid_fire_clock] (auto&& port)
		{
			return InputPort::pack ({
				.a = port.poll_button (0) || (rapid_fire_clock && port.poll_button (8)),
				.b = port.poll_button (1) || (rapid_fire_clock && port.poll_button (9)),
				.s = port.poll_button (2),
				.t = port.poll_button (3),
				.u = port.poll_button (4),
				.d = port.poll_button (5),
				.l = port.poll_button (6),
				.r = port.poll_button (7)
				});
		}, ports [port_index]);
	}

	template <typename _Host>
	bool strobe_clock (_Host&& host) const
	{
		return !!((host.count_frames () >> 1u) & 1u);
	}

	template <typename _Host>
	auto read (_Host&& host)
		->std::array<byte, 4u>
	{
		std::array<byte, 4u> controller_states;
		const auto rapid_fire_clock = strobe_clock (host);
		for (auto port_index = 0; port_index < 4; ++port_index)
			controller_states [port_index] = poll_every_button (
				port_index, rapid_fire_clock);
		return controller_states;
	}
private:
	impl ports [4u];
};
