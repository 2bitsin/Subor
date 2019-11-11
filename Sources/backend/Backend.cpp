#include <chrono>

#include <backend/Backend.hpp>
#include <frontend/Frontend.hpp>
#include <test/Nestest.hpp>

Backend::Backend (const Options& args, Frontend& frontend)
: _frontend{frontend},
	_console{std::make_unique<Console> ()},
	_quit{false}
{
	if (args.cmd_load ().has_value ())
		_console->load (args.cmd_load ().value ());
	else
		_console->load (nestest::AsProgramROM ());
	_emuThread = std::thread{[this] ()
	{
		emulate ();
	}};
}

Backend::~Backend ()
{
	_quit.store (true);
	_emuThread.join ();
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
		auto& frame = _frame [_index % ctFrameCount];
		_console->emulate (frame);
		if (_frontend.pushFrame(*this, frame))
			++_index;
		std::this_thread::sleep_until(tn);
		t0 = tn;
		tn = t0 + dt;
	}
}
