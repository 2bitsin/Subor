#pragma once

#include <cmath>

namespace
{
	template <typename T>
	constexpr inline T pi = T(3.1415926535897932384626433832795);
	template <typename T>
	constexpr inline T sqrt2over2 = T(0.707106781186547524401);
	template <typename T>
	constexpr inline T sqrt2 = T(2.0 * sqrt2over2<T>);
}

template <typename _Float, typename _Mixin>
struct FilterCommon
{

	friend _Mixin;
	
	template <typename... Args>
	FilterCommon(Args&& ... args)
	: 	xnz1{_0},
		xnz2{_0},
		ynz1{_0},
		ynz2{_0},
		a0{_0},
		a1{_0},
		a2{_0},
		b1{_0},
		b2{_0}
	{
		_Mixin::init(*this, std::forward<Args>(args)...);
	}
	
	auto apply(_Float value)
	{
		auto xn = value;
		auto yn = a0*xn + a1*xnz1 + a2*xnz2 - b1*ynz1 - b2*ynz2;
		
		assert(!isnan(xn) && !isinf(xn));
		assert(!isnan(yn) && !isinf(yn));
		
		xnz2 = xnz1; xnz1 = xn;
		assert(!isnan(xnz2) && !isinf(xnz2));
		assert(!isnan(xnz1) && !isinf(xnz1));
		
		ynz2 = ynz1; ynz1 = yn;
		assert(!isnan(ynz2) && !isinf(ynz2));
		assert(!isnan(ynz1) && !isinf(ynz1));
		return yn;
	}
	
protected:
	static inline constexpr auto _2 = _Float(2.0);
	static inline constexpr auto _1 = _Float(1.0);
	static inline constexpr auto _05 = _Float(0.5);
	static inline constexpr auto _0 = _Float(0.0);
	static inline constexpr auto _pi = pi<_Float>;

	_Float xnz1, xnz2, ynz1, ynz2;
	_Float a0, a1, a2, b1, b2;
};
