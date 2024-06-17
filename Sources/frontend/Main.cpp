#include "Main.hpp"
#include <filesystem>
#include <iostream>

int main (int argc, Options::cca argv, Options::cca envp)
try
{
	using namespace std::filesystem;
	//nestest::self_test();
	//return 0;

	auto options = Options{argc, argv, envp};
	auto const base_path = path(argv[0]).parent_path().parent_path() / "data";
	std::cout << "Base path : " << base_path << "\n";
	current_path(base_path);

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