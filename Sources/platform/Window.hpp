#pragma once

#include <vector>
#include <string>

#include <core/CoreConfig.hpp>
#include "SDL2.hpp"

struct Window: CoreConfig
{

	Window (const std::vector<std::string>& args);
	~Window ();

	void open ();
	void close ();

	auto handle() const { return _handle; }

private:

	SDL_Window* _handle{nullptr};
};