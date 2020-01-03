#pragma once
#include <utils/Types.hpp>
#include <utils/BitCommon.hpp>
#include <atomic>

struct InputPort
{
	template <int _Port, typename _Host, typename _Value>
	void read (_Host&& host, _Value&& data)
	{
		data = (data & 0xe0) | (input_shift [_Port] & 1u);
		if (!(input_latch & 1u))
			input_shift [_Port] >>= 1u;
	}

	template<typename _Host, typename _Value>
	void write (_Host&& host, _Value&& data)
	{
		auto w = input_state.load (
			std::memory_order_acquire);
		for (auto i = 0; i < std::size (input_shift); ++i)
		{
			input_latch = (byte)data;
			if (input_latch & 1u)
				input_shift [i] = ((w >> (i * 8u)) & 0xffu);
		}
	}

	auto set (const std::array<byte, 4> &input_bits)
	{		
		input_state.store (
			bits::pack<8, 8, 8, 8> (
				input_bits [0u], 
				input_bits [1u], 
				input_bits [2u], 
				input_bits [3u]),
			std::memory_order_release);
	}

	struct buttons
	{
		bool a: 1;	// A
		bool b: 1;	// B
		bool s: 1;	// select
		bool t: 1;	// start
		bool u: 1;	// up
		bool d: 1;	// down
		bool l: 1;	// left
		bool r: 1;	// right
	};

	static constexpr auto pack(const buttons f)
	{
		return bits::pack<1,1,1,1,1,1,1,1>(
			f.a, f.b, f.s, f.t, f.u, f.d, f.l, f.r);
	}

private:
	std::atomic<dword> input_state{0};
	byte input_shift [4u] = {0, 0, 0, 0};
	byte input_latch{0};
	qword last_frame{0};
};