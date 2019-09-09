#pragma once

#include <complex>

static constexpr auto pi = 3.14159265359;

struct AudioChannelBase
{
	static inline constexpr const auto kTickLength		= 1u;
	static inline constexpr const auto kTickSweep			= 2u;
	static inline constexpr const auto kTickEnvelope	= 4u;
};

template <typename _Mixin>
struct AudioChannel
:	public AudioChannelBase
{

	AudioChannel ()
	{}

	auto lcEnable (bool)
	{}

	auto lcStatus () const
	{
		return 0u;
	}

	auto level () const
	{
		return 0.0f;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data, bool active)
	{}

	void reset ()
	{}

	auto value () const
	{
		return ((_Mixin&)*this).level() * _value;
	}

	void tickSequencer(byte mask)
	{}

protected:
	float _value = 0.0f;
	float _level = 0.0f;
	static inline const byte _legth_table[] = 
	{
		10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	};
};

