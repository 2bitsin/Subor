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

	template <byte _Offset = 0u, byte _Length = 8u>
	void load (word value)
	{
		static constexpr auto _Mask = ((1u << _Length) - 1u) << _Offset;
		_value = ((value << _Offset) & _Mask) | (_value & ~_Mask);
	}

private:
	word _value{0u};
	bool _halt{false};
	bool _disable{false};
};