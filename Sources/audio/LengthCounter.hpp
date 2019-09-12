#pragma once

#include "utils/Types.hpp"

struct LengthCounter
{
	void tick ()
	{
		if (_value > 0u && !_halt)
			--_value;
	}

	void disable(bool val)
	{
		if (_disable = val)
			_value = 0u;
	}

	void halt(bool val)
	{
		_halt = val;
	}

	auto&& value () const
	{
		return _value;
	}

	auto status () const
	{
		return value () != 0u && !_disable;
	}

	void load (word value)
	{
		_value = _lut[value & 0x1f];
	}

private:
	byte _value{0u};
	bool _halt{false};
	bool _disable{false};

	static inline constexpr const byte _lut[] = {
	 10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
   12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};
};