#pragma once

#include "core/Memory.hpp"
#include "utils/Types.hpp"
#include "AudioChannel.hpp"
#include "PeriodCounter.hpp"
#include "TriangleGenerator.hpp"

struct TriangleChannel
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
		case 0x8:
			{
				auto [r, c] = bits::unpack_as_tuple<7, 1> (data);
				_cflag = !!c;
				_lengc.halt (_cflag);
				_lcrel = r;
				break;
			}
		case 0xA:
			{
				_timer.load<0, 8> (data);
				break;
			}
		case 0xB:
			{
				auto [htime, lenct]
					= bits::unpack_as_tuple<3, 5> (data);
				_lengc.load (lenct);
				_timer.load<8, 3> (htime);
				_rflag = true;
				break;
			}
		}
	}

	template <byte _Clk>
	void step ()
	{
		if (_Clk & kStepChannel)
			if (status () && _lcntr != 0)
				if (_timer.step ())
					_value = _trgen.step ();

		if (_Clk & kStepEnvelope) // linear counter clk (envelope clock)
		{
			if (_rflag)
			{
				_lcntr = _lcrel;
				_rflag = _cflag;
			}
			else if (_lcntr != 0u)
				--_lcntr;
		}
		super::step<_Clk> ();
	}

	byte value () const
	{
		if (!status ())
			return 0;
		if (!_lcntr)
			return 0;
		if (!_timer.valid ())
			return 0;
		return super::value ();
	}

private:
	byte _lcntr{0};									// Linear counter
	byte _lcrel{0};									// Linear counter reload value
	bool _cflag{false};							// Control flag
	bool _rflag{false};							// Linear counter reload flag
	TriangleGenerator _trgen;				// Triangle generator
	PeriodCounter<8, 0x7ff> _timer; // 11-bit period timer
};