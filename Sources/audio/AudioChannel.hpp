#pragma once

#include <complex>

static constexpr auto pi = 3.14159265359;

struct AudioChannel
{
	AudioChannel ()
	{}

	auto lcEnable (bool)
	{}

	auto lcStatus () const
	{
		return 0u;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{}

	void reset ()
	{}

	auto value () const
	{
		return _value;
	}

protected:
	float _value = 0.0f;
};

