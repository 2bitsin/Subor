// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "core/Memory.hpp"
#include "fileio/INes.hpp"

#include <memory>


template <typename _MapperImpl>
struct Mapper
{
	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&&, word addr, _Value&& data)
	{
		return kOpenBus;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{
		return kOpenBus;
	}

	template <ResetType _Type>
	void reset ()
	{ }

	static word basicMirror (Mirroring type, word addr)
	{
		switch (type)
		{
		case kHorizontal:
			return addr&~0x400u;
		case kVertical:
			return addr&~0x800u;

		default:
		case kFourScreen:
			return addr;
		}
		return addr;

	}
};

