#pragma once

#include <complex>

static constexpr auto pi = 3.14159265359;

struct AudioChannel
{
	AudioChannel()
	{
		
	}

	template <typename _Dt>
	float update(_Dt dt)
	{
		q *= std::polar(1.0, 440.0 * 2.0 * pi * dt);;
		return (float)q.real();
	}

	void reset()
	{}

private:
	std::complex<double> q{ std::polar(1.0, 0.0) };
};

