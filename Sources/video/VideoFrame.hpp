#pragma once

#include <utils/Types.hpp>
#include <utils/SDL2.hpp>

struct VideoFrame
{
	void assign (SDL_Surface& canvas);
	void unassign ();
	void set(qword x, qword y, dword pixel);
private:
	SDL_Surface* canvas = nullptr;
};
