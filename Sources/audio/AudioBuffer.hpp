#pragma once

#include "utils/Types.hpp"

#include <tuple>
#include <vector>

namespace detail
{
	template <typename _Value_type>
	struct sample
	:	protected std::tuple<_Value_type, _Value_type>
	{
	private:
		using v = _Value_type;
	public:

		sample(_Value_type m = _Value_type{0})
		:	std::tuple<v, v>(m, m)
		{}

		sample(_Value_type l, _Value_type r)
		:	std::tuple<v, v>(l, r)
		{}

		auto&& lhs() const { return std::get<0>(*this); }
		auto&& rhs() const { return std::get<1>(*this); }
	};
}

template <typename _Value_type>
struct AudioBuffer
:	public std::vector<detail::sample<_Value_type>>
{
	using value_type = _Value_type;

	AudioBuffer(std::size_t _leng_hint)
	{
		if (_leng_hint > 0ul)
			this->reserve(_leng_hint);
	}

	void lock() {}
	void unlock() {}
};
