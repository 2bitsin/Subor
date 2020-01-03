#pragma once

#include "Bitcommon.hpp"
#include "Bitshifter.hpp"

template <std::size_t _Size, std::size_t _Count, std::size_t _TotalBits = _Count*_Size>
struct Bitarray: 
	Bitshifter<_TotalBits>
{
	static constexpr auto Mask = __bits_hidden__::mask<0, _Size>();
	static constexpr auto GMask = __bits_hidden__::mask<0, _TotalBits>();
	using value_type = std::remove_const_t<decltype(GMask)>;

	using super = Bitshifter<_TotalBits>;

	template <typename _Return>
	struct element
	{
		element(value_type& q, std::size_t j)
		:	g(q), i(j*_Size)
		{}

		auto&& operator = (value_type q)
		{
			auto m = value_type(Mask) << i;
			g = (g & ~m) | ((q << i) & m);
			return *this;
		}

		operator _Return () const
		{
			return _Return ((g >> i) & Mask);
		}

	private:
		value_type& g;
		std::size_t i;
	};

	auto operator [] (std::size_t i)
	{
		using return_type = decltype(__bits_hidden__::mask<0, _Size>());
		return element<return_type> {super::bits, i};
	}

	value_type operator [] (std::size_t i) const
	{
		using return_type = decltype(__bits_hidden__::mask<0, _Size>());
		return return_type ((super::bits >> i) & Mask);
	}

	using super::super;
		
	auto left(value_type value = 0)
	{
		return super::template left<_Size>(value);
	}

	auto right(value_type value = 0)
	{
		return super::template right<_Size>(value);
	}
		
	auto left(value_type value, std::size_t cnt)
	{
		return super::left(value, cnt*_Size);
	}

	auto right(value_type value, std::size_t cnt)
	{
		return super::right(value, cnt*_Size);
	}
};
