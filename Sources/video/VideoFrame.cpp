#include "VideoFrame.hpp"
#include <cassert>

void VideoFrame::assign (SDL_Surface& _canvas)
{
	canvas = &_canvas;
}

void VideoFrame::unassign ()
{
	canvas = nullptr;
}

void VideoFrame::set (qword x, qword y, dword pixel)
{
	if (canvas != nullptr)
	{
		auto dst = (dword*)canvas->pixels;
		assert (int (x) < canvas->w && int (y) < canvas->h && x >= 0 && y >= 0);
		dst [(canvas->pitch >> 2)* y + x] = pixel;
	}
}
