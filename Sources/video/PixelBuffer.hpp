#pragma once

#include <utils/Types.hpp>
#include <utils/SDL2.hpp>
#include <cassert>

template <typename _PixelType>
struct PixelBufferT
{
	using pixel_type = _PixelType;

	void set(qword x, qword y, pixel_type pixel)
	{
		auto& s = *_surface;
		assert(x < s.w);
		assert(y < s.h);
		assert(s.pixels != nullptr);
		auto* dst = ((byte*)s.pixels) + y*s.pitch;
		((pixel_type*)dst)[x] = pixel;
	}

	PixelBufferT(dword w, dword h)
	: _surface(SDL_CreateRGBSurfaceWithFormat (0u, w, h, 0u, SDL_PIXELFORMAT_ARGB8888)) 
	{}

	PixelBufferT(const PixelBufferT& rvalue)
	: _surface(SDL_CreateRGBSurfaceWithFormat (
			0u, rvalue._surface.w, 
			rvalue._surface.h, 0u, 
			SDL_PIXELFORMAT_ARGB8888
		)) 
	{
		SDL_BlitSurface(rvalue._surface, nullptr, _surface, nullptr);
	}

	auto& operator = (const PixelBufferT& rvalue) 
	{
		this->~PixelBufferT();
		new (this) PixelBufferT(rvalue);
		return *this;	
	}

	PixelBufferT(PixelBufferT&& xvalue)
	: _surface (std::exchange(xvalue._surface, nullptr))
	{}

	auto& operator = (PixelBufferT&& xvalue)
	{
		this->~PixelBufferT();
		new (this) PixelBufferT(std::move(xvalue));
		return *this;
	}

	void lock()	
	{
		SDL_LockSurface(_surface);
	}

	void unlock()
	{
		SDL_UnlockSurface(_surface);
	}

	auto handle()
	{
		return _surface;
	}

	void blit_to (SDL_Window* window) const
	{
		SDL_BlitScaled(_surface, nullptr, SDL_GetWindowSurface(window), nullptr);
	}

private:
	SDL_Surface* _surface;
};

using PixelBuffer = PixelBufferT<dword>;