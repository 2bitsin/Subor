#pragma once
#include "AudioChannel.hpp"

template <word _BaseAddress>
struct TrianChannel
:	public AudioChannel<TrianChannel<_BaseAddress>>
{
	
};

