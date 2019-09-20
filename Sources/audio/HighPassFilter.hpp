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

	template <typename _Filter>
	static auto init(_Filter& base, int fc, int fs)
	{
		constexpr auto _2 = _Float(2.0);
		constexpr auto _1 = _Float(1.0);
		constexpr auto _0 = _Float(0.0);
		constexpr auto _pi = pi<_Float>;
		
		auto th = _2 * _pi * fc / fs;
		auto g = std::cos(th) / (_1 + std::sin(th));
		base.a0 = (_1 + g) / _2;
		base.a1 = -((_1 + g) / _2);
		base.a2 = _0;
		base.b1 = -g;
		base.b2 = _0;
	}
};
