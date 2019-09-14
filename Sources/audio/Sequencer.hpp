#pragma once

#include "utils/Types.hpp"
#include "utils/Types.hpp"

struct Sequencer
{

	void mode5(bool val)
	{
		_mode5 = val;
	}

	void irqdi(bool val)
	{
		_irqen = !val;
	}

	bool irq_rr()	
	{		
		if (!_mode5 && !_cstep)
			return _irqra;
		return std::exchange(_irqra, false);
	}

	bool irq() const
	{
		return _irqra;
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
			case 3: (channel.step<0b1110>(), ...); 
				_irqra = _irqen; 
				break;
			}
			_cstep = (_cstep + 1)%4;
		}
	}


private:
	bool _mode5{false};
	bool _irqen{false};
	bool _irqra{false};
	byte _cstep{0};
	
};