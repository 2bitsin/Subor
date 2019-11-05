#pragma once

#include <utils/Types.hpp>
#include <utils/SDL2.hpp>

template <typename _PixelType>
struct PixelBuffer
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

	PixelBuffer(dword w, dword h)
	: _surface(SDL_CreateRGBSurfaceWithFormat (0u, w, h, 0u, SDL_PIXELFORMAT_ARGB8888)) 
	{}

	PixelBuffer(const PixelBuffer& rvalue)
	: _surface(SDL_CreateRGBSurfaceWithFormat (
			0u, rvalue._surface.w, 
			rvalue._surface.h, 0u, 
			SDL_PIXELFORMAT_ARGB8888
		)) 
	{
		SDL_BlitSurface(rvalue._surface, nullptr, _surface, nullptr);
	}

	auto& operator = (const PixelBuffer& rvalue) 
	{
		this->~PixelBuffer();
		new (this) PixelBuffer(rvalue);
		return *this;	
	}

	PixelBuffer(PixelBuffer&& xvalue)
	: _surface (std::exchange(xvalue._surface, nullptr))
	{}

	auto& operator = (PixelBuffer&& xvalue)
	{
		this->~PixelBuffer();
		new (this) PixelBuffer(std::move(xvalue));
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

private:
	SDL_Surface* _surface;
};
