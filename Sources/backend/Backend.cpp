#include "Backend.hpp"

Backend::Backend (const Options& args)
{
	_console = std::make_unique<Console>();
	_console->load(args.loadrom());

	_bgThread = std::thread{[this] ()
	{
		emulate ();
	}};
}

Backend::~Backend ()
{
	_quit.store (true);
	_bgThread.join ();
}

void Backend::emulate ()
{
	while (!_quit.load ())
	{

	}
}
