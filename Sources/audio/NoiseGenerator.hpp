#pragma once

#include "utils/Types.hpp"
#include "utils/Bitarray.hpp"

struct NoiseGenerator
{
	byte step ()
	{
		auto b = byte (_shreg [0]);
		_shreg.right (b ^ _shreg [_modef ? 6 : 1]);
		return b;
	}

	void modef (bool v)
	{
		_modef = v;
	}
private:
	bool _modef{false};
	Bitarray<1, 15> _shreg{1};
};
