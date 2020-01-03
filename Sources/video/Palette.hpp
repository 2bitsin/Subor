#pragma once

#include "utils/Types.hpp"
#include <initializer_list>
#include <type_traits>
#include <algorithm>

struct Palette
{
protected:
	template <typename _This>
	constexpr static auto&& I(_This&& p, word addr)
	{
		addr &= 0x1fu;
		if (addr >= 0x10u && !(addr & 0x3u))
			addr &= ~0x10;
		return p.bits[addr];
	}
public:
	constexpr Palette()
	:	Palette {{0}}
	{}

	constexpr Palette(const byte (&init) [0x20u])
	:	Palette(init, std::make_index_sequence<0x20>{})
	{}

	constexpr auto&& operator [] (word addr) { return I(*this, addr); }
	constexpr auto&& operator [] (word addr) const { return I(*this, addr); }

	constexpr dword rgba(word addr) const
	{
		return cvttbl[(*this)[addr] & 0x3fu];
	}
private:

	template <std::size_t ... _Index>
	constexpr Palette (const byte (&init) [0x20u], std::index_sequence<_Index...>)
	:	bits{init[_Index]...}
	{}

	byte bits[0x20u];

	static constexpr inline dword cvttbl [64u] =
	{
		0xFF666666, 0xFF002A88, 0xFF1412A7, 0xFF3B00A4, 0xFF5C007E, 0xFF6E0040, 0xFF6C0600, 0xFF561D00,
		0xFF333500, 0xFF0B4800, 0xFF005200, 0xFF004F08, 0xFF00404D, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFFADADAD, 0xFF155FD9, 0xFF4240FF, 0xFF7527FE, 0xFFA01ACC, 0xFFB71E7B, 0xFFB53120, 0xFF994E00,
		0xFF6B6D00, 0xFF388700, 0xFF0C9300, 0xFF008F32, 0xFF007C8D, 0xFF000000, 0xFF000000, 0xFF000000,
		0xFFFFFEFF, 0xFF64B0FF, 0xFF9290FF, 0xFFC676FF, 0xFFF36AFF, 0xFFFE6ECC, 0xFFFE8170, 0xFFEA9E22,
		0xFFBCBE00, 0xFF88D800, 0xFF5CE430, 0xFF45E082, 0xFF48CDDE, 0xFF4F4F4F, 0xFF000000, 0xFF000000,
		0xFFFFFEFF, 0xFFC0DFFF, 0xFFD3D2FF, 0xFFE8C8FF, 0xFFFBC2FF, 0xFFFEC4EA, 0xFFFECCC5, 0xFFF7D8A5,
		0xFFE4E594, 0xFFCFEF96, 0xFFBDF4AB, 0xFFB3F3CC, 0xFFB5EBF2, 0xFFB8B8B8, 0xFF000000, 0xFF000000,
	};
};
