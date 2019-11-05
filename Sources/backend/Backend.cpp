#include "Backend.hpp"

Backend::Backend ()
{
	_bgThread([this] () 
	{ 
		emulate (); 
	});
}

Backend::~Backend ()
{
	_quit.store(true);
	_bgThread.join();
}

void Backend::emulate ()
{
	while (!_quit.load())
	{
		
	}
}
