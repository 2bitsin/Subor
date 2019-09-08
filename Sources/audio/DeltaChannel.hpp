#pragma once

#include "AudioChannel.hpp"

template <word _BaseAddress>
struct DeltaChannel
: public AudioChannel
{
	auto irq () const
	{
		return 0u;
	}
};
