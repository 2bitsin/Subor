// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include <iterator>

template <std::size_t _Size>
struct OAMemory
{

#pragma pack(push, 1)
	union SpriteAttribute
	{
		SpriteAttribute (byte init = 0u): bits{ init } { }
		auto& operator = (const SpriteAttribute& prev)
		{
			bits = prev.bits;
			return *this;
		}

		byte bits;
		Bitfield<0, 2, 8> palette;
		Bitfield<2, 3, 8> unused;
		Bitfield<5, 1, 8> priority;
		Bitfield<6, 1, 8> flipX;
		Bitfield<7, 1, 8> flipY;
	};

	struct Object
	{
		byte y;
		byte tile;
		SpriteAttribute attr;
		byte x;
	};
#pragma pack(pop)

	auto&& operator [] (std::size_t idx) { return I(*this, idx); }
	auto&& operator [] (std::size_t idx) const { return I(*this, idx); }

	template <typename _Addr>
	auto peek (_Addr addr) const
	{
		if ((addr >> 2u) >= _Size)
			return byte{0xffu};
		auto& o = objects [addr >> 2u];
		switch (addr & 3u)
		{
		case 0u: return (byte)o.y;
		case 1u: return (byte)o.tile;
		case 2u: return (byte)o.attr.bits;
		case 3u: return (byte)o.x;
		}
		assert (!"This shouldn't happen.");
		return byte{0xffu};
	}

	template <typename _Addr, typename _Data>
	auto poke (_Addr addr, _Data data)
	{
		if ((addr >> 2u) >= _Size)
			return;
		auto& o = objects [addr >> 2u];
		switch (addr & 3u)
		{
		case 0u: o.y = data & 0xffu; return;
		case 1u: o.tile = data & 0xffu; return;
		case 2u: o.attr.bits = data & 0xffu; return;
		case 3u: o.x = data & 0xffu; return;
		}
		assert (!"This shouldn't happen.");
	}

	OAMemory ()
	{
		std::fill (
			std::begin (objects),
			std::end (objects),
			Object{ });
	}

private:
	template <typename _This>
	static auto&& I(_This&& that, std::size_t idx)
	{
		return that.objects [idx % std::size (that.objects)];
	}

	Object objects [_Size];
};
