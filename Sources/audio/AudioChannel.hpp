#pragma once

#include "utils/Types.hpp"
#include "LengthCounter.hpp"

namespace 
{
	
	inline constexpr auto kStepChannel	= 0b0001;
	inline constexpr auto kStepEnvelope	= 0b0010;
	inline constexpr auto kStepSweeper	= 0b0100;
	inline constexpr auto kStepLength		= 0b1000;
}

struct AudioChannel
{

	template <byte _Clk>
	void step ()
	{
		if (_Clk & kStepLength)
			_lengc.tick ();
	}

	template <BusOperation _Operation, typename _Data>
	void tick (word addr, _Data&& data)
	{}

	void enable (bool v)
	{
		_enabl = v;
	}

	auto enabled () const
	{
		return _enabl;
	}

	auto irq () const
	{
		return false;
	}

	byte value () const
	{
		return _value;
	}

	auto status () const
	{
		return _lengc.value () != 0u;
	}

protected:
	byte _value{0};
	bool _enabl{false};
	LengthCounter _lengc;
};
