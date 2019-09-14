#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"
#include "utils/Bitarray.hpp"

struct PulseGenerator
{
	
	auto tick()
	{
		std::size_t d = _.duty;
		std::size_t t = _.tick;		
		--_.tick;
		return _lut[d*8u + t];
	}
	
	void load(byte d)
	{
		_.duty = d;
		_.tick = 0;
	}

private:
	union
	{
		Bitfield<0, 3> tick;
		Bitfield<3, 2> duty;
	}_{0};

	static inline const Bitarray<1, 8*4> _lut {0b11111100'00001111'00000011'00000001};
};