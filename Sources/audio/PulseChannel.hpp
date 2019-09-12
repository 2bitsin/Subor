#pragma once

#include "PulseGenerator.hpp"
#include "LengthCounter.hpp"
#include "EnvelopeGenerator.hpp"
#include "SweepGenerator.hpp"

template <int _ChannelNum>
struct PulseChannel
{

	template <MemoryOperation _Operation, typename _Data>
	void tick(word addr, _Data&& data)
	{
		if (_Operation == kPeek)
			return;
		switch (addr)
		{		
		case 4*_ChannelNum+0:
			{
				auto [volper, cstvol, lencth, dutycy] 
					= bits::unpack_as_tuple<4, 1, 1, 2>(data);
				_envlp.load(volper);
				_envlp.cvol(cstvol);
				_lengc.halt(lencth);
				_pulse.load(dutycy);
				break;
			}
		case 4*_ChannelNum+1:
			{
				_sweep.load(byte(data));
				break;
			}
		case 4*_ChannelNum+2:
			{
				_timer = (_timer & 0xff00) | byte(data);
				break;
			}
		case 4*_ChannelNum+3:
			{
				auto [htime, lenct] 
					= bits::unpack_as_tuple<3, 5>(data);
				_lengc.load(lenct);
				_envlp.start();
				_timer = (_timer & 0x00ff) | (word(byte(data)) << 8u);
				break;
			}
		}
	}
	
	template <byte clk>
	void step()
	{
		if (clk & 0b0001)
		{
			if (_ckdiv > 0)
				--_ckdiv;
			else
			{
				_ckdiv = _timer;
				_value = _pulse.tick();
			}
		}

		if (clk & 0b0010)
			_envlp.tick();
		if (clk & 0b0100)
			_sweep.tick(_timer);
		if (clk & 0b1000)
			_lengc.tick();
	}

	byte value() const
	{
		if (!_enabl)
			return 0;
		if (!_lengc.value())
			return 0;
		if (_timer < 8u || _timer > 0x7ff)
			return 0;
		if (!_value)
			return 0;
		return _envlp.level();
	}

	void enable(bool val)
	{
		_enabl = val;
	}

	void status() const
	{
		return _lengc.value() != 0u;
	}

private:
	byte _value {0};
	word _ckdiv {0};
	word _timer {0};
	bool _enabl {true};
	PulseGenerator _pulse;
	EnvelopeGenerator _envlp;
	SweepGenerator<_ChannelNum != 0> _sweep;
	LengthCounter _lengc;
};