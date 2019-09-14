#pragma once

#include "utils/Types.hpp"
#include "LengthCounter.hpp"

struct AudioChannel
{

	template <byte _What>
	void step ()
	{}

	template <MemoryOperation _Operation, typename _Data>
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
