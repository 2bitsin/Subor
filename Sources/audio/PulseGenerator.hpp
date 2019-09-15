#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"
#include "utils/Bitarray.hpp"

struct PulseGenerator
{
	
	auto step()
	{		
		static const Bitarray<1, 8*4> _lut {0b11111100'00001111'00000011'00000001};
		byte d = _.duty;
		byte t = _.tick;		
		--_.tick;
		return _lut[d*8 + t];
	}
	
	void load(byte d)
	{
		_.duty = d;
	}

	void reset()
	{
		_.tick = 0;
	}

private:
	union
	{
		Bitfield<0, 3, 8> tick;
		Bitfield<3, 2, 8> duty;
	}_{0};

	
};