#pragma once

#include "core/Memory.hpp"
#include "utils/Types.hpp"
#include "utils/Bitarray.hpp"
#include "AudioChannel.hpp"
#include "PeriodCounter.hpp"
#include "EnvelopeGenerator.hpp"

struct NoiseChannel
:	public AudioChannel
{
	template <byte _Clk>
	auto step ()
	{
	
	}

private:
	PeriodCounter<8, 0x7ff> _timer;
	EnvelopeGenerator _envlp;
};