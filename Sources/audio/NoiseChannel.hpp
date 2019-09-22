#pragma once

#include "core/Memory.hpp"
#include "utils/Types.hpp"
#include "AudioChannel.hpp"
#include "PeriodCounter.hpp"
#include "EnvelopeGenerator.hpp"
#include "NoiseGenerator.hpp"

struct NoiseChannel
	: public AudioChannel
{
	using super = AudioChannel;

	template <BusOperation _Operation, typename _Host, typename _Data>
	void tick (_Host&& host, word addr, _Data&& data)
	{
		if (_Operation == kPeek)
			return;
		if (_Operation == kPoke)
		{
			switch (addr)
			{
			case 0xC:
				{
					auto [volper, cstvol, lencth, _]
						= bits::unpack_as_tuple<4, 1, 1, 2> (data);
					_envlp.load (volper);
					_envlp.cvol (cstvol);
					_envlp.loop (cstvol);
					_lengc.halt (lencth);
					break;
				}
			case 0xE:
				{
					auto [index, _, modef]
						= bits::unpack_as_tuple<4, 3, 1> (data);
					_timer.load (_perio [index]);
					_noise.modef (modef);
					break;
				}
			case 0xF:
				{
					auto [_, lenct]
						= bits::unpack_as_tuple<3, 5> (data);
					_lengc.load (lenct);
					_envlp.start ();
					break;
				}
			}
		}
	}

	template <byte _Clk, typename _Host>
	auto step (_Host&& host)
	{
		if (_Clk & kStepChannel)
			if (status ())
				if (_timer.step ())
					_value = _noise.step ();
		if (_Clk & kStepEnvelope)
			_envlp.tick ();
		super::step<_Clk> (host);
	}

	byte value () const
	{
		if (!status ())
			return 0;
		if (!_timer.valid ())
			return 0;
		if (!super::value ())
			return 0;
		return _envlp.level ();
	}

private:
	NoiseGenerator _noise;
	PeriodCounter<8, 0x7ff> _timer;
	EnvelopeGenerator _envlp;

	constexpr static inline const word _perio [] = {
		4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
	};
};