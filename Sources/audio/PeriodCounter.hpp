#pragma once

#include "utils/Types.hpp"
#include "utils/BitCommon.hpp"

template <auto _MinValid = 8u, auto _MaxValid = 0x7ffu>
struct PeriodCounter
{
	template <auto _Offset = 0u, auto _Size = 11u, typename _Value>
	void load (_Value&& val)
	{
		_const = bits::splice<_Offset, _Size> (_const, val);
	}

	auto step ()
	{
		if (_value != 0u)
		{
			--_value;
			return false;
		}
		_value = _const;
		return true;
	}

	bool valid() const
	{
		return _const >= _MinValid && _const < _MaxValid;
	}

	auto&& value () const
	{
		return _const;
	}

private:
	word _value{0};
	word _const{0};
};