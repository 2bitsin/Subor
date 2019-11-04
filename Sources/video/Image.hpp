// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once


template <typename U, std::size_t W, std::size_t H>
struct Image
{
	static constexpr const auto width = W;
	static constexpr const auto height = H;
	U data [W*H];

	void set (std::size_t x, std::size_t y, U c)
	{
		if (x < 0 || x >= W || y < 0 || y >= H)
			return;
		data [x+y*W] = c;
	}

	auto value (std::size_t x, std::size_t y) const
	{
		if (x < 0 || x >= W || y < 0 || y >= H)
			return U{0};
		return data[x+y*W];
	}
};
