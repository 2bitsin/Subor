#pragma once

#include <complex>

static constexpr auto pi = 3.14159265359;

template 
<	typename _Value_type,
	_Value_type _Min_value,
	_Value_type _Max_value>
struct AudioChannel
{
	using value_type = _Value_type;

	static constexpr auto value_min = _Min_value;
	static constexpr auto value_max = _Max_value;
	static constexpr auto value_dif = _Max_value - _Min_value;

	AudioChannel()
	{}

	template <typename _Delta>
	auto value(_Delta dt)
	{
		q *= std::polar(1.0, 440.0 * 2.0 * pi * dt);

		auto s = (float)(q.real() * 0.5 + 0.5);
		return value_type(s * value_dif + value_min);
	}

	void reset()
	{}

private:
	std::complex<double> q{ std::polar(1.0, 0.0) };
};

