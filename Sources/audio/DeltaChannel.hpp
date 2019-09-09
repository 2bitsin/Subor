#pragma once

#include "AudioChannel.hpp"

template <word _BaseAddress>
struct DeltaChannel
: public AudioChannel<DeltaChannel<_BaseAddress>>
{
	auto irq () const
	{
		return 0u;
	}
};
