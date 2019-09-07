// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "core/Console.hpp"
#include "test/Nestest.hpp"
#include "utils/Bitfield.hpp"

#include <cassert>
#include <cmath>
#include <chrono>
#include <string>
#include <optional>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


template <typename T>
using cptr_t = const T*;
template <typename T>
using ccptr_t = cptr_t<cptr_t<T>>;


struct InputProxy
{

	enum Mode
	{
		Recording,
		Playback,
		Passtrough
	};

	union Buttons
	{
		int bits;
		Bitfield<0, 1> a;
		Bitfield<1, 1> b;
		Bitfield<2, 1> select;
		Bitfield<3, 1> start;
		Bitfield<4, 1> up;
		Bitfield<5, 1> down;
		Bitfield<6, 1> left;
		Bitfield<7, 1> right;
		Buttons (): bits{ 0 } { }
	};

	union Signals
	{
		int bits;
		Bitfield<0, 1> reset;
		Signals (): bits{ 0 } { }
	};


	struct Frame
	{
		Buttons port [2];
		Signals signals;
	};

	Mode mode;
	std::size_t findex{ 0 };
	std::ofstream sink;
	std::vector<Frame> frames{ };

	InputProxy (Mode m, const std::filesystem::path& logPath = "")
	: mode{ m },
		findex{ 0 }
	{
		if (m == Recording)
			sink.open (logPath, std::ios::binary);
		if (m == Playback)
		{
			std::ifstream source (logPath, std::ios::binary);
			Frame f{ };
			while (source.read ((char*)&f, sizeof (f)))
				frames.push_back (f);
		}
	}

	void pushFrame (const Frame& f)
	{
		switch (mode)
		{
		case Passtrough:
			frames.clear ();
		case Recording:
			findex = frames.size ();
			frames.push_back (f);
			break;
		case Playback:
			break;
		}
		if (mode == Recording)
			sink.write ((const char*)&f, sizeof (f));
	}

	bool empty () const
	{
		return findex >= frames.size ();
	}

	template <typename _Host>
	void next (_Host&& m)
	{
		if (empty ())
			return;
		auto&& frame = frames [findex];
		m.input ((byte)frame.port [0].bits, (byte)frame.port [1].bits, byte{ }, byte{ });
		if (frame.signals.reset)
			m.reset<kSoftReset> ();
		++findex;
	}
};



int main (std::size_t argc, const ccptr_t<char> argv)
{
	using namespace std::string_literals;
	//return nestest::NestestMain();
	if (argc < 2)
		throw std::runtime_error ("No arguments");
	auto console = std::make_unique<Console> (argv [1] + ".nes"s);
	bool rerecordMode = argc >= 3 ? argv [2] == "r"s : false;
	bool playbackMode = argc >= 3 ? argv [2] == "p"s : false;

	InputProxy input (
		rerecordMode
		? InputProxy::Recording
		: (playbackMode
			? InputProxy::Playback
			: InputProxy::Passtrough),
		argv [1] + ".input"s);

	SDL_AudioSpec want{}, have{};
	want.freq			= 48000;
	want.channels = 2;
	want.format		= AUDIO_F32;
	want.samples	= 400;
	want.callback = nullptr;
	want.userdata = nullptr;

	SDL_Init (SDL_INIT_EVERYTHING);
	std::atexit (SDL_Quit);


	const auto q = SDL_WINDOWPOS_CENTERED;
	auto window = SDL_CreateWindow (nullptr, q, q, 3u*console->width (), 3u*console->height (), 0u);
	auto buffer = SDL_GetWindowSurface (window);
	auto screen = SDL_CreateRGBSurfaceWithFormat (0u, console->width (), console->height (), 0u, SDL_PIXELFORMAT_ARGB8888);
	auto audiod = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 1);
	if (!audiod)
		std::printf("Failed to open audio device, sound disabled!\n");
	SDL_PauseAudioDevice(audiod, 0);
	using namespace std::chrono;
	auto dt = 0us;

	console->rateOfSampling(have.freq);	
	auto xctrl = SDL_GameControllerOpen (0);

	int fctr = 0;
	for (;;)
	{
		auto t0 = high_resolution_clock::now ();
		auto kstlen = 0;
		auto kstate = SDL_GetKeyboardState (&kstlen);

		if (xctrl)
		{
			InputProxy::Frame controller{ { } };
			controller.port [0].a = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_A)
				|| SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_Y);
			controller.port [0].b = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_B)
				|| SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_X);
			controller.port [0].select = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_BACK);
			controller.port [0].start = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_START);
			controller.port [0].up = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_UP);
			controller.port [0].down = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			controller.port [0].left = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			controller.port [0].right = SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
			input.pushFrame (controller);
		}

		input.next (*console);
		SDL_LockSurface (screen);
		console->frame 
		(	[screen] (auto x, auto y, auto c) {
				auto dst = (dword*)screen->pixels;
				assert (int(x) < screen->w && int(y) < screen->h && x >= 0 && y >= 0);
				dst[(screen->pitch >> 2)*y + x] = c;
			}, 
			[audiod](auto&& buff) {
				auto len = (Uint32)(buff.size() * sizeof(buff[0]));
				//if (SDL_GetQueuedAudioSize(audiod) > len*10)
				//	return;
				SDL_QueueAudio(audiod, (std::uint8_t*)buff.data(), len);
			});

		SDL_UnlockSurface (screen);
		SDL_BlitScaled (screen, nullptr, buffer, nullptr);
		SDL_UpdateWindowSurface (window);

		SDL_Event event;
		auto t1 = high_resolution_clock::now ();
		do
		{
			if (SDL_PollEvent (&event))
			{
				if (event.type == SDL_QUIT)
					break;
				if (event.type == SDL_KEYDOWN 
					||event.type == SDL_KEYUP)
				{}
				continue;
			}
			t1 = high_resolution_clock::now ();
		} while (t1 < t0 + (16666666ns * !!xctrl));

		if (event.type == SDL_QUIT)
			break;
		
	}


	if (xctrl != nullptr)
		SDL_GameControllerClose (xctrl);
	SDL_CloseAudioDevice(audiod);
	SDL_FreeSurface (screen);
	SDL_DestroyWindow (window);
	
	return 0;
}