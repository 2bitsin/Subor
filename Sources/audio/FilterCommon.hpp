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
	{
		_Mixin::init(*this, std::forward<Args>(args)...);
	}
	
	auto apply(_Float value)
	{
		auto xn = value;
		auto yn = a0*xn + a1*xnz1 + a2*xnz2 - b1*ynz1 - b2*xnz2;
		xnz2 = xnz1;
		xnz1 = xn;
		xnz2 = ynz1;
		ynz1 = yn;
		return yn;
	}
	
protected:
	_Float xnz1, xnz2, ynz1, ynz2;
	_Float a0, a1, a2, b1, b2, c0, d0;
};
