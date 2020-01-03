#pragma once

#include <utils/types.hpp>

template <typename _Color, dword _SizeX, dword _SizeH>
struct Image
{
	using color_type = _Color;
	static constexpr const auto width		= _SizeX;
	static constexpr const auto height	= _SizeH;
	color_type data [_SizeX*_SizeH];

	constexpr void set (dword x, dword y, color_type color)
	{
		if (x >= width || y >= height)
			return;
		data [x + y*width] = color;
	}

	constexpr auto value (dword x, dword y) const
	{
		if (x >= width || y >= height)
			return color_type{0};
		return data [x + y*width];
	}
};
