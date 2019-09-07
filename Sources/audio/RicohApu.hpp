// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "core/Memory.hpp"
#include "audio/TriangleChannel.hpp"
#include "audio/PulseChannel.hpp"
#include "audio/NoiseChannel.hpp"
#include "audio/DeltaChannel.hpp"

struct RicohAPU
{
	using AudioBuffer = std::vector<std::pair<float, float>>;

	RicohAPU()
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host& master, word addr, _Value&& data)
	{
		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = 0u;
			if constexpr (_Operation == kPoke);

			switch (addr)
			{
			case 0x00: 
			case 0x01: 
			case 0x02: 
			case 0x03: 
			case 0x04: 
			case 0x05: 
			case 0x06:
			case 0x07:
			case 0x08: 
			case 0x09: 
			case 0x0A: 
			case 0x0B: 
			case 0x0C: 
			case 0x0D: 
			case 0x0E: 
			case 0x0F:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x18:
			case 0x19:
			case 0x1A:
			case 0x1B:
			case 0x1C:
			case 0x1D:
			case 0x1E:
			case 0x1F:
				break;

			case 0x16:
				inputStrobe<_Operation, _Host>(data);
				if constexpr (_Operation == kPoke)
					break;
			case 0x17:
				inputRead<_Operation, _Host>(addr, data);
				break;
			}			
		}		
		return kSuccess;
	}

	template <ResetType _Type>
	void reset()
	{
		triangle.reset();
		pulse0.reset();
		pulse1.reset();
		noise.reset();		
	}

	template <typename... Args>
	void input(Args&& ... new_state)
	{
		input_state = std::array<byte, 4u>{new_state...};
	}

	template <typename _Sink>
	void grabFrame(_Sink&& sink)
	{
		sink(buffer);
	}

	void rateOfSampling(int _)
	{
		spclk = _;
	}

private:

	template<MemoryOperation _Operation, typename _Host, typename _Value>
	void inputRead(const word& addr, _Value&& data)
	{
		if constexpr (_Operation == kPeek)
		{
			auto i = (addr - 0x16) & 1u;
			data = (data & 0xe0) | (input_shift[i] & 1u);
			if (!(input_latch & 1u))
				input_shift[i] >>= 1u;
		}
	}

	template<MemoryOperation _Operation, typename _Host, typename _Value>
	void inputStrobe(_Value&& data)
	{
		if constexpr (_Operation == kPoke)
		{
			for (auto i = 0; i < input_shift.size(); ++i)
			{
				input_latch = (byte)data;
				if (input_latch & 1u)
					input_shift[i] = input_state[i];
			}
		}
	}

private:
	std::array<byte, 4u> input_state{0, 0, 0, 0};
	std::array<byte, 4u> input_shift{0, 0, 0, 0};
	byte input_latch{0};

	int							spclk;
	AudioBuffer			buffer;
	TriangleChannel triangle;
	PulseChannel		pulse0;
	PulseChannel		pulse1;
	NoiseChannel		noise;
	DeltaChannel	  delta;
};