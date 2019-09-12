#pragma once

#include "utils/Types.hpp"
#include "utils/Types.hpp"

struct Sequencer
{

	void mode5(bool val)
	{
		_mode5 = val;
	}

	template <typename... _Channel>
	void step (_Channel&& ... channel)
	{
		if (_mode5)
		{
			switch(_cstep)
			{
			case 0: (channel.step<0b1110>(), ...); break;
			case 1: (channel.step<0b0010>(), ...); break;
			case 2: (channel.step<0b1110>(), ...); break;
			case 3: (channel.step<0b0010>(), ...); break;
			case 4: (channel.step<0b0000>(), ...); break;
			}
			_cstep = (_cstep + 1)%5;
		}
		else
		{
			switch(_cstep)
			{
			case 0: (channel.step<0b0010>(), ...); break;
			case 1: (channel.step<0b1110>(), ...); break;
			case 2: (channel.step<0b0010>(), ...); break;
			case 3: (channel.step<0b1110>(), ...); break;
			}
			_cstep = (_cstep + 1)%4;
		}
	}

private:
	byte _mode5{0};
	byte _cstep{0};
};