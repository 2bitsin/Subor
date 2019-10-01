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
				{
					auto [p, _, l, i] = bits::unpack_as_tuple<4, 2, 1, 1> (data);
					_timer.load (_perio [p] >> 1u);
					_irqen = !!i;
					_loopf = !!l;
					if (!_irqen)
						_irqon = false;
					break;
				}
			case 0x11:
				_value = data;
				break;
			case 0x12:
				_waddr = bits::pack<6, 8, 2> (0b000000, data, 0b11);
				break;
			case 0x13:
				_wleng = bits::pack<4, 8, 4> (0b0001, data, 0b0000);
				break;
			}
		}
	}

	void enable (bool _enabl)
	{
		super::enable (_enabl);
		if (!_enabl)
			_cleng = 0u;
		else
			if (!_cleng)
				reset();
	}

	template <typename _Host>
	void step_dma (_Host&& host)
	{
		assert (_cleng > 0u);

		byte t{0};
		if (!host.isOamDma ())
		{
			host.tick<kDummyPeek> (*this, _caddr, t);
			host.tick<kDummyPeek> (*this, _caddr, t);
		}
		host.tick<kDummyPeek> (*this, _caddr, t);
		host.tick<kPeek> (*this, _caddr, t);

		_sbuff.value (t);
		++_caddr;
		--_cleng;
		if (!_caddr)
			_caddr = 0x8000u;
		_cbits = 8u;
		_irqon = _irqon || (_irqen && !_cleng);
	}

	template <typename _Host>
	void step_output (_Host&& host)
	{
		const auto dval = _sbuff.right () ? +2 : -2;
		_value = std::clamp ((int)_value + dval, 0, 127);
		--_cbits;
	}

	template <byte _Clk, typename _Host>
	void step (_Host&& host)
	{
		if constexpr (_Clk & kStepChannel)
		{
			if (_timer.step ())
			{
				if (!_cbits)
				{
					if (_cleng > 0u)
						step_dma (host);
					else
						if (_loopf)
							reset ();
				}
				else
					step_output (host);
			}
		}
		super::step<_Clk> (host);
	}

	void reset ()
	{
		_caddr = _waddr;
		_cleng = _wleng;
	}

	bool irq () const
	{
		return _irqon;
	}

	bool irq_rr ()
	{
		return std::exchange (_irqon, false);
	}

	auto status () const
	{
		return _cleng > 0u;
	}

private:
	word _waddr{0};
	word _wleng{0};
	word _caddr{0};
	word _cleng{0};
	byte _cbits{0};
	Bitarray<1, 8> _sbuff;
	bool _irqen{false};
	bool _irqon{false};
	bool _loopf{false};
	PeriodCounter<0, 0x7ff> _timer; // 11-bit period timer

	static inline const word _perio [] =
	{
		428u, 380u, 340u, 320u,
		286u, 254u, 226u, 214u,
		190u, 160u, 142u, 128u,
		106u, 84u, 72u, 54u
	};
};