#pragma once

#include <cassert>
#include <iterator>

template <typename T>
struct array_view
{
	using zword = std::size_t;

	constexpr array_view ()
	: array_view (nullptr, nullptr)
	{}

	template <typename _Ctype, typename T = std::void_t<
		decltype(std::data (std::declval<_Ctype> ())),
		decltype(std::size (std::declval<_Ctype> ()))
	>>
	constexpr array_view (_Ctype&& cont)
	: array_view (std::data (cont), std::size (cont))
	{}

	constexpr array_view (T* lhs)
	: _lhs{lhs},
		_rhs{_locate_null (lhs)}
	{
//		#pragma message(_PhDL_FileLine() " -> Potentially unsafe, you must make sure there is null at the end!")
	}

	constexpr array_view (T* lhs, T* rhs)
	: _lhs{lhs},
		_rhs{rhs}
	{
		assert (_lhs < _rhs);
	}

	constexpr array_view (T* lhs, std::size_t len)
	: _lhs{lhs},
		_rhs{lhs + len}
	{
		assert (_lhs < _rhs);
	}

	inline constexpr auto length () const
	{		
		assert (_lhs < _rhs);
		return zword (_rhs - _lhs);
	}

	inline constexpr auto size () const
	{
		return length () * sizeof (T);
	}

	inline constexpr auto data () const
	{
		return begin ();
	}

	inline constexpr auto begin () const
	{
		return _lhs;
	}

	inline constexpr auto end () const
	{
		return _rhs;
	}

	inline constexpr auto cbegin () const
	{
		return static_cast<const T*>(begin ());
	}

	inline constexpr auto cend () const
	{
		return static_cast<const T*>(end ());
	}

	inline constexpr auto rbegin () const
	{
		return std::reverse_iterator{begin ()};
	}

	inline constexpr auto rend () const
	{
		return std::reverse_iterator{end ()};
	}

	inline constexpr auto crbegin () const
	{
		return std::reverse_iterator{cbegin ()};
	}

	inline constexpr auto crend () const
	{
		return std::reverse_iterator{cend ()};
	}

	inline constexpr auto&& operator [] (std::size_t idx) const
	{
		assert (idx < length ());
		return _lhs [idx];
	}

	array_view subview (std::size_t start, std::size_t leng)
	{
		assert (start + leng <= length ());
		assert (start < length ());
		return array_view{&_lhs [start], leng};
	}

	array_view subview (std::intptr_t start)
	{
		if (start < 0)
			return subview (length () + start, -start);
		return subview ((std::size_t)start, length () - start);
	}

public:
	static inline constexpr auto _locate_null (T* s)
	{
		while (*s) ++s;
		return s;
	}
private:
	T* _lhs{nullptr};
	T* _rhs{nullptr};
};

