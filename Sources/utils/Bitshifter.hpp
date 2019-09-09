// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "Bitcommon.hpp"
#include <cassert>

template <std::size_t _Size>
struct Bitshifter
{
	static constexpr const auto Mask = __bits_hidden__::mask<0, _Size> ();
	using value_type = std::remove_const_t<decltype(Mask)>;

	Bitshifter (value_type q = 0)
	: bits (q & Mask)
	{ }

	template <std::size_t _Bits = 1u>
	value_type left (value_type q = 0)
	{
		static_assert (_Bits <= _Size);
		using namespace __bits_hidden__;
		static constexpr const auto m = mask<0, _Bits>();
		const value_type g = (bits >> (_Size - _Bits)) & m;
		bits = ((bits << _Bits) | (q & m)) & Mask;
		return g;
	}

	template <std::size_t _Bits = 1u>
	value_type rot_left (value_type q = 0)
	{
		return left<_Bits>(extract(_Size - _Bits, _Bits));
	}

	value_type left (value_type q, std::size_t _Bits)
	{
		assert (_Bits <= _Size);
		using namespace __bits_hidden__;
		const auto m = (value_type(1u) << _Bits) - value_type(1u);
		const value_type g = (bits >> (_Size - _Bits)) & m;
		bits = ((bits << _Bits) | (q & m)) & Mask;
		return g;
	}

	template <std::size_t _Bits = 1u>
	value_type right (value_type q = 0)
	{
		static_assert (_Bits <= _Size);
		using namespace __bits_hidden__;
		static constexpr const auto m = mask<0, _Bits>();
		const value_type g = bits & m;
		bits = ((bits >> _Bits) | ((q & m) << (_Size - _Bits))) & Mask;
		return g;
	}

	template <std::size_t _Bits = 1u>
	value_type rot_right (value_type q = 0)
	{
		return right<_Bits>(extract(0u, _Bits));
	}

	value_type right (value_type q, std::size_t _Bits)
	{
		assert (_Bits <= _Size);
		using namespace __bits_hidden__;
		const auto m = (value_type(1u) << _Bits) - value_type(1u);
		const value_type g = bits & m;
		bits = ((bits >> _Bits) | ((q & m) << (_Size - _Bits))) & Mask;
		return g;
	}

	auto&& value (value_type q)
	{
		bits = q & Mask;
		return *this;
	}

	value_type value () const
	{
		return bits & Mask;
	}

	template <typename U = value_type>
	U extract(std::size_t o, std::size_t s) const
	{
		return U(((bits & Mask) >> o) & ((value_type(1u) << s) - value_type(1u)));
	}

	auto replace(value_type v, std::size_t o, std::size_t s)
	{
		const auto m = ((value_type(1u) << s) - 1u) << o;
		v <<= o;
		bits = (bits & ~m) | (v & m);
	}

	value_type bits;
};
