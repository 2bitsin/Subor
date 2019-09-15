#pragma once

#include "AudioChannel.hpp"
#include "PulseGenerator.hpp"
#include "PeriodCounter.hpp"
#include "LengthCounter.hpp"
#include "EnvelopeGenerator.hpp"
#include "SweepGenerator.hpp"

template <int _ChannelNum>
struct PulseChannel
	: public AudioChannel
{
	using super = AudioChannel;

	template <BusOperation _Operation, typename _Data>
	void tick (word addr, _Data&& data)
	{
		if (_Operation == kPeek)
			return;
		switch (addr)
		{
		case 4 * _ChannelNum + 0:
			{
				auto [volper, cstvol, lencth, dutycy]
					= bits::unpack_as_tuple<4, 1, 1, 2> (data);
				_envlp.load (volper);
				_envlp.cvol (cstvol);
				_envlp.loop (cstvol);
				_lengc.halt (lencth);
				_pulse.load (dutycy);
				break;
			}
		case 4 * _ChannelNum + 1:
			{
				_sweep.load (data);
				break;
			}
		case 4 * _ChannelNum + 2:
			{
				_timer.load<0, 8> (data);
				break;
			}
		case 4 * _ChannelNum + 3:
			{
				auto [htime, lenct]
					= bits::unpack_as_tuple<3, 5> (data);
				_lengc.load (lenct);
				_envlp.start ();
				_timer.load<8, 3> (htime);
				break;
			}
		}
	}

	template <byte _Clk>
	void step ()
	{
		if (_Clk & kStepChannel)
			if (status())
				if (_timer.step ())
					_value = _pulse.step ();
		if (_Clk & kStepEnvelope)
			_envlp.tick ();
		if (_Clk & kStepSweeper)
			_sweep.tick (_timer);
		super::step<_Clk>();
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
	PeriodCounter<8, 0x7ff> _timer;
	PulseGenerator _pulse;
	EnvelopeGenerator _envlp;
	SweepGenerator<_ChannelNum != 0> _sweep;
};