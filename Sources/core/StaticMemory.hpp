// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "core/Memory.hpp"
#include <array>

template <MemoryType _Perm, dword _Bottom, dword _Top, dword _Size = _Top - _Bottom>
struct StaticMemory
{
	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&&, word addr, _Value&& data)
	{
		if constexpr (!(byte (_Perm) & _Operation))
			return kSuccess;
		if (addr < _Bottom || addr >= _Top)
			return kOpenBus;
		addr -= _Bottom;
		addr %= _Size;
		if constexpr (_Operation == BusOperation::kPeek)
			data = bits [addr];
		else
			bits [addr] = (byte)data;
		return kSuccess;
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&&, word addr, _Value&& data) const
	{
		if constexpr (!(byte (_Perm) & _Operation) || _Operation != BusOperation::kPeek)
			return kSuccess;
		if (addr < _Bottom || addr >= _Top)
			return kOpenBus;
		addr -= _Bottom;
		addr %= _Size;
		data = bits [addr];
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{
		if constexpr (_Type == kHardReset)
			std::fill (bits.begin (), bits.end (), 0);
	}

	std::array<byte, _Size> bits;
};