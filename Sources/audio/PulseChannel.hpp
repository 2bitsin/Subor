#pragma once
#include "AudioChannel.hpp"

template <word _BaseAddress>
struct PulseChannel
: public AudioChannel<PulseChannel<_BaseAddress>>
{
	using super = AudioChannel<PulseChannel<_BaseAddress>>;

	auto lcEnable (bool _enable)
	{
		if(!_enable)
		{
			_lenctr = 0;
			_lenctr_mask = 0u;
		}
		else
			_lenctr_mask = 0xffu;
	}

	auto lcStatus () const
	{
		return 0u;
	}

	void tickFrame()
	{
		if (_lenctr > 0u)
			--_lenctr;			
	}

	auto level() const
	{
		return _lenctr > 0u ? 1.0f : 0.0f;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data, bool active)
	{
		if (active == true)
			updateInternal ();

		switch (addr)
		{
		case _BaseAddress + 0u:
			{
				auto [vvvv, c, l, DD] = bits::unpack_as_tuple<4, 1, 1, 2> (data);
				switch (DD)
				{
				case 0: _duty_cycle = 0b01000000;	break;
				case 1: _duty_cycle = 0b01100000;	break;
				case 2: _duty_cycle = 0b01111000;	break;
				case 3: _duty_cycle = 0b10011111; break;
				}
			}
			break;
		case _BaseAddress + 1u:
			{
				auto [HHH, lllll] = bits::unpack_as_tuple<3, 5>(data);
				_timer_load = (_timer_load & 0x00ff) | (word(HHH) << 8u);
				_lenctr_load = super::_legth_table[lllll] & _lenctr_mask;
			}
			break;
		case _BaseAddress + 2u:
			_timer_load = (_timer_load & 0xff00) | byte(data);
			break;
		case _BaseAddress + 3u:
			break;
		}

	}

	void updateInternal ()
	{
		if (_timer_ticks)
			--_timer_ticks;
		else
		{
			_timer_ticks = _timer_load;
			super::_value = _duty_cycle.extract (0u, 1u);
			_duty_cycle.rot_right<1u> ();
		}
	}

	void reset ()
	{}

private:
	word _timer_ticks = 0u;
	word _timer_load = 0u;
	word _lenctr = 0u;
	byte _lenctr_halt = 0u;
	byte _lenctr_load = 0u;
	byte _lenctr_mask = 0u;
	byte _const_voume = 0u;
	Bitshifter<8> _duty_cycle = 0b01111000;
};
