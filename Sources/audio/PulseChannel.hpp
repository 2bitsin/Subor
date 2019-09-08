#pragma once
#include "AudioChannel.hpp"

template <word _BaseAddress>
struct PulseChannel
:	public AudioChannel
{
	auto lcEnable (bool)
	{}

	auto lcStatus () const
	{
		return 0u;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{
		switch (addr)
		{
		case _BaseAddress + 0u:
		case _BaseAddress + 1u:
		case _BaseAddress + 2u:
		case _BaseAddress + 3u:
			break;
		}
	}

	void reset ()
	{}

private:
	word _lenctr = 0u;
	byte _lenctr_halt= 0u;
};
