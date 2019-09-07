// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include <initializer_list>
#include <algorithm>

struct Palette
{
	Palette(): bits {0u} {};
	Palette(const byte (&init) [0x20u])
	{
		std::copy(
			std::begin(init), 
			std::end(init), 
			std::begin(bits));
	}

	auto&& operator [] (word addr)			 { return I(*this, addr); }
	auto&& operator [] (word addr) const { return I(*this, addr); }

	dword rgba(word addr) const;

protected:
	template <typename _This>
	static auto&& I(_This&& p, word addr)
	{
		addr &= 0x1fu;
		if (addr >= 0x10u && !(addr & 0x3u))
			addr &= ~0x10;
		return p.bits[addr];
	}

private:
	byte bits[0x20u];
};