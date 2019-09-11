#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"

template <bool _AlternateCarry = false>
struct SweepGenerator
{

	void load(byte _val)
	{
		_param.bits = _val;
	}

	void tick(word& _period)
	{
		if (!_param.n)
		{
			_period <<= byte(_param.s + 1u);
		}
		else
		{
			_period >>= byte(_param.s + 1u);
		}
	}

private:
	union
	{
		Bitfield<0, 3, 8> s;
		Bitfield<3, 1, 8> n;
		Bitfield<4, 3, 8> p;
		Bitfield<7, 1, 8> e;
		byte bits;
	} _param{0u};
	byte _ckdiv{0u};
	bool _rload{false};
};
