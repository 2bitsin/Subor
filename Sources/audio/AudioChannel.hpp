#pragma once

#include "utils/Types.hpp"

struct AudioChannel
{

	template <byte _What>
	void step ()
	{}

	template <MemoryOperation _Operation, typename _Data>
	void tick (word addr, _Data&& data)
	{}

	void enable (bool)
	{}

	auto status () const
	{
		return false;
	}

	auto irq() const {return false;}

	byte value () const
	{
		return 0;
	}
};
