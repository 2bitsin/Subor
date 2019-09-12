// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "utils/ClockDivider.hpp"
#include "input/InputPort.hpp"
#include "video/RicohPPU.hpp"
#include "audio/Sequencer.hpp"
#include "audio/AudioBuffer.hpp"
#include "audio/AudioChannel.hpp"
#include "audio/Mixer.hpp"
#include "audio/PulseChannel.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
: public CoreConfig
{
	RicohAPU ()		
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{
		if (_clock_osc.tick ())
		{
			_sq0ch.step<0b0001>(); 
			_sq1ch.step<0b0001>();
			_trich.step<0b0001>();
			_noich.step<0b0001>();
			_dmcch.step<0b0001>();
		}
		if (_clock_mix.tick ())
			_mix.step (_sq0ch, _sq1ch, _trich, _noich, _dmcch);
		if (_clock_seq.tick ())
			_seq.step (_sq0ch, _sq1ch, _trich, _noich, _dmcch);

		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = latch;
			if constexpr (_Operation == kPoke)
				latch = byte (data);

			_sq0ch.tick<_Operation>(addr, data); 
			_sq1ch.tick<_Operation>(addr, data);
			_trich.tick<_Operation>(addr, data);
			_noich.tick<_Operation>(addr, data);
			_dmcch.tick<_Operation>(addr, data);

			switch (addr)
			{
			case 0x16:
				if constexpr (_Operation == kPoke)
					_input.write (data);
				if constexpr (_Operation == kPeek)
					_input.read<0> (data);
				break;
			case 0x17:
				if constexpr (_Operation == kPeek)
					_input.read<1> (data);
				break;
			}
		}
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{}

	template <typename _Sink>
	void grabFrame (_Sink&& sink)
	{
		_mix.grab(sink);
	}

	template <typename... _Args>
	auto input (_Args&& ... args)
	{
		return _input.set (std::forward<_Args> (args)...);
	}

private:

	byte latch{0u};
	ClockDivider<ctCPUTicksPerSecond, ctSamplingRate> _clock_mix;
	ClockDivider<ctCPUTicksPerSecond, ctSEQTicksPerSecond> _clock_seq;
	ClockDivider<ctCPUTicksPerSecond, ctCPUTicksPerSecond / 2> _clock_osc;
	InputPort	_input;
	Sequencer _seq;
	PulseChannel<0> _sq0ch;
	PulseChannel<1> _sq1ch;

	AudioChannel _noich;
	AudioChannel _dmcch;
	AudioChannel _trich;

	Mixer _mix;
};