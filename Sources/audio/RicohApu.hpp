// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "video/RicohPPU.hpp"
#include "audio/TriangleChannel.hpp"
#include "audio/PulseChannel.hpp"
#include "audio/NoiseChannel.hpp"
#include "audio/DeltaChannel.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
	: public CoreConfig
{
	using AudioBuffer = std::vector<std::pair<float, float>>;

	RicohAPU ()
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{
		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = latch;
			if constexpr (_Operation == kPoke)
				latch = byte(data);

			switch (addr)
			{
			case 0x15:
				if constexpr (_Operation == kPoke)
					pokeStatus (data);
				if constexpr (_Operation == kPeek)
					peekStatus (data);
				break;
			case 0x16:
				if constexpr (_Operation == kPoke)
					inputStrobe (data);
				if constexpr (_Operation == kPeek)
					inputRead<0> (data);
				break;
			case 0x17:
				if constexpr (_Operation == kPoke)
					pokeFrameCycle(data);
				if constexpr (_Operation == kPeek)
					inputRead<1> (data);
				break;
			}
		}
		tickInternal<_Operation> (host, addr, data);
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{
		trich.reset ();
		sq0ch.reset ();
		sq1ch.reset ();
		noich.reset ();
	}

	template <typename... Args>
	void input (Args&& ... values)
	{
		byte new_state [4u] = {values...};
		input_state [0] = new_state [0];
		input_state [1] = new_state [1];
		input_state [2] = new_state [2];
		input_state [3] = new_state [3];
	}

	template <typename _Sink>
	void grabFrame (_Sink&& sink)
	{
		sink (buffer);
		buffer.clear ();
	}

private:

	template<typename _Data>
	void pokeFrameCycle(_Data&& data)
	{
		_.irq_disable = !!(byte(data) & 0x40u);
		_.frame_cycle = !!(byte(data) & 0x80u);
	}

	template<typename _Data>
	void pokeStatus (_Data&& data)
	{
		sq0ch.lcEnable (!!(byte(data) & 0x01u));
		sq1ch.lcEnable (!!(byte(data) & 0x02u));
		trich.lcEnable (!!(byte(data) & 0x04u));
		noich.lcEnable (!!(byte(data) & 0x08u));
		dmcch.lcEnable (!!(byte(data) & 0x10u));
	}

	template<typename _Data>
	void peekStatus (_Data&& data)
	{
		data = bits::pack<1, 1, 1, 1, 1, 1, 1, 1> (
			sq0ch.lcStatus () ? 1u : 0u,
			sq1ch.lcStatus () ? 1u : 0u,
			trich.lcStatus () ? 1u : 0u,
			noich.lcStatus () ? 1u : 0u,
			dmcch.lcStatus () ? 1u : 0u,
			0u,
			dmcch.irq () ? 1u : 0u,
			_.irq_raised ? 1u : 0u);
	}

	template<int _Port, typename _Value>
	void inputRead (_Value&& data)
	{
		data = (data & 0xe0) | (input_shift [_Port] & 1u);
		if (!(input_latch & 1u))
			input_shift [_Port] >>= 1u;
	}

	template<typename _Value>
	void inputStrobe (_Value&& data)
	{
		for (auto i = 0; i < std::size (input_shift); ++i)
		{
			input_latch = (byte)data;
			if (input_latch & 1u)
				input_shift [i] = input_state [i];
		}
	}

	void mixAudio ()
	{
		auto _sq0 = 15.0f * sq0ch.value ();
		auto _sq1 = 15.0f * sq1ch.value ();
		auto _tri = 15.0f * trich.value ();
		auto _noi = 15.0f * noich.value ();
		auto _dmc = 127.0f * dmcch.value ();

		float g0 = 0.0f;
		if (_tri || _noi || _dmc)
		{
			g0 = (_tri / 8227.0f) + (_noi / 12241.0f) + (_dmc / 22638.0f);
			g0 = 1.0f / g0 + 100.0f;
			g0 = 159.79f / g0;
		}

		float g1 = 0.0f;
		if (_sq0 || _sq1)
		{
			g1 = (8128.0f / (_sq0 + _sq1)) + 100.0f;
			g1 = 95.88f / g1;
		}

		g0 = g0 + g1;
		buffer.emplace_back (g0, g0);
	}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tickInternal (_Host& host, word addr, _Value&& data)
	{
		trich.tick<_Operation> (host, addr, data);
		sq0ch.tick<_Operation> (host, addr, data);
		sq1ch.tick<_Operation> (host, addr, data);
		noich.tick<_Operation> (host, addr, data);
		dmcch.tick<_Operation> (host, addr, data);

		clkdiv += ctSamplingClockDivider;
		if (clkdiv >= 0x100000000ull)
		{
			clkdiv &= 0xffffffffull;
			mixAudio ();
		}
	}

private:
	byte										latch{0u};

	byte										input_state [4u] = {0, 0, 0, 0};
	byte										input_shift [4u] = {0, 0, 0, 0};
	byte										input_latch{0};

	qword										clkdiv{0ull};

	union
	{
		Bitfield<0, 1, 8>			irq_raised;
		Bitfield<1, 1, 8>			frame_cycle;
		Bitfield<2, 1, 8>			irq_disable;
		byte bits;
	} _{0};

	PulseChannel<0x4000>		sq0ch;
	PulseChannel<0x4004>		sq1ch;
	TriangleChannel<0x4008> trich;
	NoiseChannel<0x400C>		noich;
	DeltaChannel<0x4010>	  dmcch;

	AudioBuffer							buffer;
};