#pragma once

#include <string>
#include <iostream>

#include <utils/Types.hpp>
#include <utils/Bitfield.hpp>

#pragma pack(push, 1)

union InputFrame
{
	enum Button 
	{ 
		A				= 0, 
		B				= 1, 
		Select	= 2, 
		Start		= 3, 
		Up			= 4, 
		Down		= 5, 
		Left		= 6, 
		Right		= 7
	};

	inline static constexpr const auto kNumButtons = 8;

	byte bits;
	Bitfield<0, 1> a;
	Bitfield<1, 1> b;
	Bitfield<2, 1> select;
	Bitfield<3, 1> start;
	Bitfield<4, 1> up;
	Bitfield<5, 1> down;
	Bitfield<6, 1> left;
	Bitfield<7, 1> right;

	InputFrame ()
	: bits{0}
	{}

	inline static const constexpr char kButtonValue [9u] = {'A', 'B', '@', '#', '^', 'v', '<', '>', '-'};

	std::string to_string () const
	{
		auto& v = kButtonValue;
		char keys [9] =
		{
			/*******/a ? v [0] : v [8],
			/*******/b ? v [1] : v [8],
			/**/select ? v [2] : v [8],
			/***/start ? v [3] : v [8],
			/******/up ? v [4] : v [8],
			/****/down ? v [5] : v [8],
			/****/left ? v [6] : v [8],
			/***/right ? v [7] : v [8],
		  '\0'
		};
		return keys;
	}

	bool from_string (const std::string& str)
	{
		auto& v = kButtonValue;
		constexpr auto n = sizeof(kButtonValue) - 1;
		if (str.length() != n)
			return false;
		for (auto i = 0u; i < n; ++i)
		{
			if (str [i] == v [i])
				bits |= (1u << i);
			else if (str [i] == v [n])
				bits &= ~(1u << i);
			else
				return false;
		}
		return true;
	}

	friend auto& operator << (std::ostream& cout, const InputFrame& data)
	{
		cout << data.to_string ();
		return cout;
	}

	friend auto& operator >> (std::istream& cin, InputFrame& data)
	{
		char keys [sizeof(kButtonValue)];
		cin.read (keys, sizeof (keys) - 1);
		if (cin.gcount () != sizeof (keys) - 1)
			cin.setstate (std::ios_base::failbit);
		keys [sizeof (keys) - 1] = 0;
		if (!data.from_string (keys))
			cin.setstate (std::ios_base::failbit);
		return cin;
	}
};

#pragma pack(pop)