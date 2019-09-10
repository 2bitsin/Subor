#pragma once
#include "utils/Types.hpp"

struct InputPort
{
	template <typename... Args>
	void set (Args&& ... values)
	{
		byte new_state [4u] = {values...};
		for (auto i = 0; i < 4; ++i)
			input_state [i] = new_state [i];
	}

	template<int _Port, typename _Value>
	void read (_Value&& data)
	{
		data = (data & 0xe0) | (input_shift [_Port] & 1u);
		if (!(input_latch & 1u))
			input_shift [_Port] >>= 1u;
	}

	template<typename _Value>
	void write (_Value&& data)
	{
		for (auto i = 0; i < std::size (input_shift); ++i)
		{
			input_latch = (byte)data;
			if (input_latch & 1u)
				input_shift [i] = input_state [i];
		}
	}
private:
	byte input_state [4u] = {0, 0, 0, 0};
	byte input_shift [4u] = {0, 0, 0, 0};
	byte input_latch{0};
};