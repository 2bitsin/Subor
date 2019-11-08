#pragma once

#include <utils/Types.hpp>

#include <tuple>
#include <vector>
#include <cassert>

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
struct AudioBufferT
{
	using value_type = _Value_type;
	using sample_type = detail::sample<value_type>;

	AudioBufferT(std::size_t len)
	:	_buffer{std::make_unique<sample_type[]> (len)},
		_length{len},
		_index{0u}
	{}

	void lock () { clear(); }	
	void unlock	() {}

	void append(sample_type s)
	{
		assert(_index < _length);
		if (_index < _length)
			_buffer[_index++] = s;
	}

	void clear()
	{
		_index = 0;
	}

	const auto* data() const
	{
		return _buffer.get();
	}

	auto size() const
	{
		return _length;
	}

private:
	std::unique_ptr<sample_type[]> _buffer;
	std::size_t _length = 0u;
	std::size_t _index = 0u;
};

using AudioBuffer = AudioBufferT<float>;