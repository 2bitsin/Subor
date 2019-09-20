#pragma once

#include "FilterCommon.hpp"

template <typename _Float, auto _Order = 1>
struct HighPassFilter
: 	public FilterCommon<_Float, HighPassFilter<_Float, _Order>>
{
protected:
	using base = FilterCommon<_Float, HighPassFilter<_Float, _Order>>;
	friend base;
	using base::base;
	using base::_pi;
	using base::_0;
	using base::_05;
	using base::_1;
	using base::_2;

	template <typename _Filter>
	static auto init(_Filter& base, int fc, int fs, _Float Q = _Float(1.0))
	{
		if constexpr (_Order == 1)
		{
			auto th = _2 * _pi * fc / fs;
			auto g = std::cos(th) / (_1 + std::sin(th));
			base.a0 = (_1 + g) / _2;
			base.a1 = -base.a0;
			base.a2 = _0;
			base.b1 = -g;
			base.b2 = _0;
		}
		else if constexpr (_Order == 2)
		{
			const auto w = _2 * _pi * fc / fs;
			const auto d = _1 / Q;
			const auto b = _05*(_1 - (d / _2)*std::sin(w)) / (_1 + (d / _2)*std::sin(w));
			const auto g = (_05 + b)*std::cos(w);
			base.a0 = (_05 + b + g) / _2;
			base.a1 = -(_05 + b + g);
			base.a2 = base.a0;
			base.b1 = -_2 * g;
			base.b2 = _2 * b;
		}
	}
};
