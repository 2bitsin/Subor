#pragma once

#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "AudioBuffer.hpp"
#include "HighPassFilter.hpp"
#include "LowPassFilter.hpp"

struct Mixer
: public CoreConfig
{
	template
	<	typename _SQ0,
		typename _SQ1,
		typename _TRI,
		typename _NOI,
		typename _DMC>
		float mix (_SQ0&& sq0, _SQ1&& sq1, _TRI&& tri, _NOI&& noi, _DMC&& dmc)
	{
		float g0 = 0.0f;
		if (tri || noi || dmc)
		{
			g0 = (tri / 8227.0f) + (noi / 12241.0f) + (dmc / 22638.0f);
			g0 = 1.0f / g0 + 100.0f;
			g0 = 159.79f / g0;
		}

		float g1 = 0.0f;
		if (sq0 || sq1)
		{
			g1 = (8128.0f / (sq0 * 1.0f + sq1 * 1.0f)) + 100.0f;
			g1 = 95.88f / g1;
		}
		return g0 + g1;
	}

	template <typename _Host,typename... _Channel>
	void step (_Host&& host, _Channel&&... ch)
	{
		auto s = mix((ch.value())...);
		s = _hpf0.apply(s);
		s = _hpf1.apply(s);
		s = _lpf0.apply(s);
		host.audioBuffer().emplace_back (s);
	}

private:
	HighPassFilter<float, 1> _hpf0{90, ctSamplingRate};
	HighPassFilter<float, 1> _hpf1{440, ctSamplingRate};
	LowPassFilter<float, 2> _lpf0{14000, ctSamplingRate, 1.2f};
};
