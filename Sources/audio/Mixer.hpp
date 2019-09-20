#pragma once

#include "utils/Types.hpp"
#include "audio/HighPassFilter.hpp"
#include "audio/LowPassFilter.hpp"
#include "core/CoreConfig.hpp"

struct Mixer
: 	public CoreConfig
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

	template <typename... _Channel>
	void step (_Channel&&... ch)
	{
		auto s = mix((ch.value())...);
		s = _hpf0.apply(s);
		s = _hpf1.apply(s);
		s = _lpf0.apply(s);
		_buffer.emplace_back (s);
	}

	template <typename _Sink>
	void grab (_Sink&& sink)
	{
		sink (_buffer);
		_buffer.clear ();
	}

private:
	AudioBuffer<float> _buffer{ctSamplesPerFrame};
	HighPassFilter<float, 2> _hpf0{90, ctSamplingRate, 1.0f};
	HighPassFilter<float, 2> _hpf1{440, ctSamplingRate, 1.0f};
	LowPassFilter<float, 2> _lpf0{14000, ctSamplingRate, 0.25f};


};
