// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "core/Memory.hpp"
#include "fileio/ProgramROM.hpp"
#include "utils/Literals.hpp"

#include <memory>


template <typename _MapperImpl>
struct MapperBase
{
	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&&, word addr, _Value&& data)
	{
		if (addr >= 32_K)
		{
			if constexpr (_Operation == kPeek)
				data = 0xeau;
		}
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{
		if (addr < 8_K)
		{
			if constexpr (_Operation == kPeek)
				data = 0xff;
		}
	}

	template <ResetType _Type>
	void reset ()
	{}

	static word basicMirror (Mirroring type, word addr)
	{
		switch (type)
		{
		case kHorizontal:
			return addr & ~0x400u;
		case kVertical:
			return addr & ~0x800u;

		default:
		case kFourScreen:
			return addr;
		}
		return addr;
	}

	word ppuMirror (word addr) const
	{
		return basicMirror (kFourScreen, addr);
	}

	template <typename _Result, typename _Total, typename _BankSize>
	inline static auto calcBanks (_Total&& total, _BankSize&& bankSize)
	{
		return _Result((total + bankSize - 1) / bankSize);
	}

};

