// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "core/Console.hpp"
#include "test/Nestest.hpp"
#include "utils/Bitfield.hpp"
#include "audio/RicohApu.hpp"
#include "Window.hpp"

#include <cassert>
#include <cmath>
#include <chrono>
#include <string>
#include <optional>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <map>


#if __has_include("Config.hpp")
	#include "Config.hpp"
#else
	#define DEBUG_DATA_PATH ""s
#endif


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

	InputProxy (Mode m, const std::string& logPath = "")
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
		auto&& runSingleFrame = frames [findex];
		m.input ((byte)runSingleFrame.port [0].bits, (byte)runSingleFrame.port [1].bits, byte{ }, byte{ });
		if (runSingleFrame.signals.reset)
			m.template reset<kSoftReset> ();
		++findex;
	}
};

template <typename _Envp, typename _Dest>
void parse_env(_Envp envp, _Dest& dest)
{
	for(auto p = envp; *p; ++p)
	{
		std::string sp = *p;
		auto eq = std::find(sp.begin(), sp.end(), '=');
		if (eq == sp.end())
			continue;
		std::string k{sp.begin(), eq};
		std::string v{eq+1, sp.end()};
		dest.emplace(k, v);
	}
}


int main (int argc, 
	const ccptr_t<char> argv, 
	const ccptr_t<char> envp)
{

	SDL_Init (SDL_INIT_EVERYTHING);
	std::atexit (SDL_Quit);


	using namespace std::chrono;
	using namespace std::string_literals;
	//return nestest::NestestMain();

	std::vector<std::string> args{argv, argv+argc};
	std::map<std::string, std::string> env;
	parse_env(envp, env);
	auto DEBUG_DataPath = "./Data"s;
	if (env.count("DEBUG_DATA_PATH"s))
		DEBUG_DataPath = env["DEBUG_DATA_PATH"s];
	if (argc < 2)
		throw std::runtime_error ("No arguments"s);
	const auto file_to_load = DEBUG_DataPath + argv [1] + ".nes"s;
	bool rerecord_mode = argc >= 3 ? argv [2] == "r"s : false;
	bool playback_mode = argc >= 3 ? argv [2] == "p"s : false;


	auto window = Window(args);

/*
	auto console = std::make_unique<Console> ();

	console->load(file_to_load);

	InputProxy input (
		rerecord_mode
		? InputProxy::Recording
		: (playback_mode
			? InputProxy::Playback
			: InputProxy::Passtrough),
		DEBUG_DATA_PATH + argv [1] + ".input"s);

	SDL_AudioSpec want{}, have{};
	want.freq			= RicohAPU::ctSamplingRate;
	want.channels = RicohAPU::ctSampleChannels;
	want.format		= AUDIO_F32;
	want.samples	= RicohAPU::ctSamplesPerFrame;
	want.callback = nullptr;
	want.userdata = nullptr;
	auto audiod = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);

	auto pixelb = PixelBuffer<dword>{ console->width (), console->height () };	
	auto audiob = AudioBuffer<float>{ CoreConfig::ctSamplesPerFrame };
	if (!audiod)
		std::printf("Failed to open audio device, sound disabled!\n");
	SDL_PauseAudioDevice(audiod, 0);

	auto xctrl = SDL_GameControllerOpen (0);

	auto dt = RicohPPU::ctFrameLengthInNs;
	auto tt = high_resolution_clock::now () + dt;	

	for (;;)
	{		
		auto kstlen = 0;
		auto kstate = SDL_GetKeyboardState (&kstlen);

		InputProxy::Frame controller{ { } };
		if (xctrl)
		{
			controller.port [0].a				= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_A) || SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_Y);
			controller.port [0].b				= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_B) || SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_X);
			controller.port [0].select	= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_BACK);
			controller.port [0].start		= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_START);
			controller.port [0].up			= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_UP);
			controller.port [0].down		= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			controller.port [0].left		= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			controller.port [0].right		= SDL_GameControllerGetButton (xctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		}
		else
		{
			auto ks = SDL_GetKeyboardState(nullptr);
			controller.port [0].a				= !!ks[SDL_SCANCODE_SPACE];
			controller.port [0].b				= !!(ks[SDL_SCANCODE_LCTRL] || ks[SDL_SCANCODE_LCTRL]);
			controller.port [0].select	= !!ks[SDL_SCANCODE_RSHIFT];
			controller.port [0].start		= !!ks[SDL_SCANCODE_RETURN];
			controller.port [0].up			= !!ks[SDL_SCANCODE_UP];
			controller.port [0].down		= !!ks[SDL_SCANCODE_DOWN];
			controller.port [0].left		= !!ks[SDL_SCANCODE_LEFT];
			controller.port [0].right		= !!ks[SDL_SCANCODE_RIGHT];
		}
		input.pushFrame (controller);

		input.next (*console);

		console->assignAudioBuffer(&audiob);
		console->assignPixelBuffer(&pixelb);
		console->runSingleFrame ();

		SDL_BlitScaled (pixelb.handle(), nullptr, SDL_GetWindowSurface (window.handle()), nullptr);
		SDL_UpdateWindowSurface (window.handle());
		SDL_QueueAudio(audiod, (std::uint8_t*)audiob.data(), (Uint32)(audiob.size() * sizeof(audiob[0])));
		audiob.clear();
		SDL_Event event {};
		auto t1 = high_resolution_clock::now ();

		while (high_resolution_clock::now () < tt 
			&& event.type != SDL_QUIT)
		{
			SDL_PollEvent (&event);
		}

		if (event.type == SDL_QUIT)
			break;
		tt += dt;
	}

	if (xctrl != nullptr)
		SDL_GameControllerClose (xctrl);
	SDL_CloseAudioDevice(audiod);	
	
	window.close();
	*/
	return 0;
}

