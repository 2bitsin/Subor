// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "Types.hpp"
#include "Memory.hpp"

struct RicohAPU
{
	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host& master, word addr, _Value&& data)
	{
		if (addr < 0x4000u || addr >= 0x4200u)
			return kOpenBus;
		addr -= 0x4000u;
		addr &= 0x1fu;

		if constexpr (_Operation == kPeek)
			data = 0u;
		if constexpr (_Operation == kPoke)
			;
	
		switch (addr)
		{
		case 0x00: case 0x10:
		case 0x01: case 0x11:
		case 0x02: case 0x12:
		case 0x03: case 0x13:
		case 0x04: case 0x14:
		case 0x05: case 0x15:
		case 0x06: 
		case 0x07: 
		case 0x08: case 0x18:
		case 0x09: case 0x19:
		case 0x0A: case 0x1A:
		case 0x0B: case 0x1B:
		case 0x0C: case 0x1C:
		case 0x0D: case 0x1D:
		case 0x0E: case 0x1E:
		case 0x0F: case 0x1F:
			break;
		case 0x16:
			if constexpr (_Operation == kPoke)
			{
				for (auto i = 0; i < input_shift.size(); ++i)
				{
					input_latch = (byte)data;
					if (input_latch & 1u)
						input_shift[i] = input_state[i];
				}
				break;
			}
		case 0x17:
			auto i = (addr - 0x16)&1u;
			if constexpr (_Operation == kPeek)
			{
				data = (data & 0xe0) | (input_shift[i] & 1u);
				if (!(input_latch & 1u))
					input_shift[i] >>= 1u;
			}
			break;
		}
		return kSuccess;
	}

	template <ResetType _Type>
	void reset()
	{}

	template <typename... Args>
	void input(Args&& ... new_state)
	{
		input_state = std::array<byte, 4u>{new_state...};
	}

	std::array<byte, 4u> input_state{0, 0, 0, 0};
	std::array<byte, 4u> input_shift{0, 0, 0, 0};
	byte input_latch{0};
};