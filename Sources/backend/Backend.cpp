#include <chrono>

#include <backend/Backend.hpp>
#include <frontend/Frontend.hpp>
#include <test/Nestest.hpp>

Backend::Backend (const Options& args, Frontend& frontend)
: _frontend{frontend},
	_console{std::make_unique<Console> ()},
	_audio_buff{
		AudioBuffer<float>{CoreConfig::ctSamplesPerFrame},
		AudioBuffer<float>{CoreConfig::ctSamplesPerFrame}
	},
	_pixel_buff{
		PixelBuffer<dword>{_console->width (), _console->height ()},
		PixelBuffer<dword>{_console->width (), _console->height ()}
	},
	_buff_index{0},
	_quit{false}
{
	if (args.cmd_load ().has_value ())
		_console->load (args.cmd_load ().value ());
	else
		_console->load (nestest::AsProgramROM ());
	_emu_thread = std::thread{[this] ()
	{
		emulate ();
	}};
}

Backend::~Backend ()
{
	_quit.store (true);
	_emu_thread.join ();
}


void Backend::emulate ()
{
	using namespace std::chrono_literals;
	using namespace std::chrono;
	using clock = high_resolution_clock;

	const auto dt = 16666666ns;
	auto t0 = clock::now();	
	auto tn = t0 + dt;

	while (!_quit.load ())	
	{	
		_console->emulate (_audio_buff [_buff_index], _pixel_buff [_buff_index]);
		_frontend.pushFrame (_audio_buff [_buff_index], _pixel_buff [_buff_index]);
		_buff_index = (_buff_index + 1u) % ct_buffer_count;
		std::this_thread::sleep_until(tn);
		t0 = tn;
		tn = t0 + dt;
	}
}
