#pragma once

#include "Types.hpp"

template 
<	auto _InputClock,
	auto _OutputClock,
	auto _Precision = 0x100000000ull>
struct ClockDivider
{
private:
	static inline constexpr auto t = (_OutputClock * _Precision) / _InputClock;
	using value_type = decltype(t);
	value_type _value;

public:
	ClockDivider ()
	: _value (0) 
	{}

	auto tick()
	{
		_value += t;
		if (_value >= _Precision)
		{
			_value &= (_Precision - 1u);
			return true;
		}
		return false;
	}
};