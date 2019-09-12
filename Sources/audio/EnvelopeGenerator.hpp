#pragma once

#include "utils/Types.hpp"

struct EnvelopeGenerator
{
	void load (byte v)
	{
		_param = v & 0xf;
	}

	void start()
	{
		_start = true;
	}

	void loopc(bool v)
	{
		_loopc = v;
	}

	void cvol(bool v)
	{
		_ctvol = v;
	}

	void tick () 
	{
		if (_start)
		{
			_level = 0xf;
			_count = _param;
			_start = false;
		}
		else
		{
			if (!_count)
			{
				if (_level != 0u)				
					--_level;
				else if (_loopc)
					_level = 0xfu;				
				_count = _param;
			}
			else
				--_count;
		}
	}

	auto level () const
	{
		return _ctvol ? _param : _level;
	}

private:
	byte _level{0u};
	byte _count{0u};
	byte _param{0u};
	bool _ctvol{false};
	bool _start{false};
	bool _loopc{false};
};