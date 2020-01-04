#include "Main.hpp"

int main (int argc, Options::cca argv, Options::cca envp)
try
{
	nestest::self_test();
	//return 0;

	auto options = Options{argc, argv, envp};

	SDL_LogSetAllPriority (SDL_LOG_PRIORITY_VERBOSE);
	SDL_Init (SDL_INIT_EVERYTHING);
	std::atexit (SDL_Quit);

	Main m (options);
	return m.main ();
}
catch (const std::exception & ex)
{
	std::printf ("\nUnhandled exception : %s\n", ex.what ());
	return -1;
}

