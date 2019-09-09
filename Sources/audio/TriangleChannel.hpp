#pragma once
#include "AudioChannel.hpp"

template <word _BaseAddress>
struct TriangleChannel
:	public AudioChannel<TriangleChannel<_BaseAddress>>
{
	
};

