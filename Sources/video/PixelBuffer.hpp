#pragma once

#include <utils/Types.hpp>
#include <utils/SDL2.hpp>

#include <cassert>
#include <utility>
#include <memory>

template <typename _PixelType>
struct PixelBufferT
{
	using pixel_type = _PixelType;

	void set(qword x, qword y, pixel_type pixel)
	{
		auto& s = *surface_;
		assert(x < s.w);
		assert(y < s.h);
		assert(s.pixels != nullptr);
		auto* dst = ((byte*)s.pixels) + y*s.pitch;
		((pixel_type*)dst)[x] = pixel;
	}

	PixelBufferT(dword w = CoreConfig::ctHorizontalPixels, 
							 dword h = CoreConfig::ctVerticalPixels)
	: surface_(SDL_CreateRGBSurfaceWithFormat (0u, w, h, 0u, SDL_PIXELFORMAT_ARGB8888)) 
	{}

	PixelBufferT(const PixelBufferT& rvalue)
	: surface_(SDL_CreateRGBSurfaceWithFormat (
			0u, rvalue.surface_.w, 
			rvalue.surface_.h, 0u, 
			SDL_PIXELFORMAT_ARGB8888
		)) 
	{
		SDL_BlitSurface(rvalue.surface_, nullptr, surface_, nullptr);
	}

	~PixelBufferT()
	{
		SDL_FreeSurface(surface_);
	}

	auto& operator = (const PixelBufferT& rvalue) 
	{
		this->~PixelBufferT();
		new (this) PixelBufferT(rvalue);
		return *this;	
	}

	PixelBufferT(PixelBufferT&& xvalue)
	: surface_ (std::exchange(xvalue.surface_, nullptr))
	{}

	auto& operator = (PixelBufferT&& xvalue)
	{
		this->~PixelBufferT();
		new (this) PixelBufferT(std::move(xvalue));
		return *this;
	}

	void lock()	
	{
		SDL_LockSurface(surface_);
	}

	void unlock()
	{
		SDL_UnlockSurface(surface_);
	}

	auto handle()
	{
		return surface_;
	}

	void blit_to (SDL_Window* window) const
	{
		SDL_BlitScaled(surface_, nullptr, SDL_GetWindowSurface(window), nullptr);
	}

	void blit_to (SDL_Renderer* renderer) const
	{
		auto tex = create_texture(renderer);
		SDL_RenderCopy(renderer, tex.get(), nullptr, nullptr);
	}


	auto create_texture(SDL_Renderer* renderer) const
		-> std::unique_ptr<SDL_Texture, void (*)(SDL_Texture*)>
	{
		return {
			SDL_CreateTextureFromSurface (renderer, surface_),
			&SDL_DestroyTexture
		};
	}

	void swap(PixelBufferT& other)
	{
		std::swap(surface_, other.surface_);
	}

private:
	SDL_Surface* surface_;
};

namespace std
{
	template <typename T>
	void swap(PixelBufferT<T>& lhs, PixelBufferT<T>& rhs)
	{
		lhs.swap(rhs);
	}
}

using PixelBuffer = PixelBufferT<dword>;