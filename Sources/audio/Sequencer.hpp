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

	template <typename _Host, typename... _Channel>
	void step (_Host&& host, _Channel&& ... channel)
	{
		if (_mode5)
		{
			switch(_cstep)
			{
			case 0: (channel.template step<0b1110>(host), ...); break;
			case 1: (channel.template step<0b0010>(host), ...); break;
			case 2: (channel.template step<0b1110>(host), ...); break;
			case 3: (channel.template step<0b0010>(host), ...); break;
			case 4: (channel.template step<0b0000>(host), ...); break;
			}
			_cstep = (_cstep + 1)%5;
		}
		else
		{
			switch(_cstep)
			{
			case 0: (channel.template step<0b0010>(host), ...); break;
			case 1: (channel.template step<0b1110>(host), ...); break;
			case 2: (channel.template step<0b0010>(host), ...); break;
			case 3: (channel.template step<0b1110>(host), ...);
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
