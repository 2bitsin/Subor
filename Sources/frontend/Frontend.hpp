#pragma once

#include <vector>
#include <string>

#include <core/CoreConfig.hpp>
#include <utils/Options.hpp>
#include <utils/SDL2.hpp>

struct Frontend: CoreConfig
{
	Frontend (const Options& options);
	~Frontend ();

	void open ();
	void close ();

	auto handle() const { return _window; }

private:

	SDL_Window* _window{nullptr};
};