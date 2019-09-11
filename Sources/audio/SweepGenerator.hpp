#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"

template <bool _AlternateCarry = false>
struct SweepGenerator
{

	void load(byte _val)
	{
		_param.bits = _val;
		_rload = true;
	}

	void tick(word& _period)
	{
		if (_rload)
		{
			if (_param.e && !_ckdiv)
				update (_period);
			_rload = false;
			_ckdiv = byte(_param.p);
		}
		else if (_ckdiv > 0u)
		{
			--_ckdiv;
		}
		else
		{
			if (_param.e)
				update (_period);
			_ckdiv = _param.p;
		}
	}

protected:
	void update(word& _period)
	{
		auto dt = _period >> byte(_param.s);
		if (!_param.n)
			_period += dt;
		else
		{
			_period -= dt;
			if (_AlternateCarry)
				--_period;
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
