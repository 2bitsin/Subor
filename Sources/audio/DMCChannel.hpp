#pragma once

#include "AudioChannel.hpp"
#include "utils/Bitarray.hpp"

struct DMCChannel
	: public AudioChannel
{
	using super = AudioChannel;

	template <BusOperation _Operation, typename _Host, typename _Data>
	void tick (_Host&& host, word addr, _Data&& data)
	{
		if constexpr (_Operation == kPeek)
			return;
		if constexpr (_Operation == kPoke)
		{
			switch (addr)
			{
			case 0x10:
				
				break;
			case 0x11:
				break;
			case 0x12:
				_waddr = bits::pack<6, 8, 2>(0b000000, data, 0b11);
				break;
			case 0x13:
				_wleng = bits::pack<4, 8, 4>(0b0001, data, 0b0000);
				break;
			}
		}
	}

	template <byte _Clk, typename _Host>
	auto step (_Host&& host)
	{
		if constexpr (_Clk & kStepChannel)
		{
			
		}

		super::step<_Clk> (host);
	}

	byte value () const
	{
		if (!status ())
			return 0;
		super::value ();
	}

	void reset()
	{
		_caddr = _waddr;
		_cleng = _wleng;
	}

private:
	word _caddr{0};
	word _cleng{0};
	word _waddr{0};
	word _wleng{0};
	Bitarray<1, 8> _sbuff;
	PeriodCounter<0, 0x7ff> _timer; // 11-bit period timer
};