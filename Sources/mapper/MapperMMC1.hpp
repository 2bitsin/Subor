#pragma once

#include "MapperBase.hpp"
#include "utils/Bitarray.hpp"

struct MapperMMC1
	: public MapperBase<MapperMMC1>
{
	MapperMMC1 (const ProgramROM& iNes);

	void write_control (const byte reg)
	{
		const auto data = _load.value ();
		_load.value (0);
		_bits = 0u;
		switch (reg)
		{
		case 0:
			{
				const auto [m, p, c] = bits::unpack_as_tuple<2, 2, 1> (data);
				set_mirroring (m);
				_chrMode = c;
				_prgMode = p;
				break;
			}
		case 1:
			if (!_chrMode)
			{
				_chrBank0 = (data & 0b11110);
				_chrBank1 = (data & 0b11110) + 1u;
			}
			else
				_chrBank0 = data;
			break;
		case 2:
			if (_chrMode)
				_chrBank1 = data;
			break;
		case 3:
			{
				const auto [bank, ramd] = bits::unpack_as_tuple<4, 1>(data);
				if (_prgMode & 0b10)
				{
					if (_prgMode & 1u)
					{
						_prgBank0 = bank;
						_prgBank1 = _prgCount - 1u;
					}
					else
					{
						_prgBank0 = 0;
						_prgBank1 = bank;
					}
				}
				else
				{
					_prgBank0 = (bank & 0b1110);
					_prgBank1 = (bank & 0b1110) + 1u;
				}
				_ramDisable = !!ramd;
				break;
			}
		}
	}

	void set_mirroring (byte m)
	{
		switch (m)
		{
		case 0u: _mirroring = kSingleLow; break;
		case 1u: _mirroring = kSingleHigh; break;
		case 2u: _mirroring = kVertical; break;
		case 3u: _mirroring = kHorizontal; break;
		}
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& host, word addr, _Value&& data)
	{
		if constexpr (_Operation == kPoke)
		{
			const auto [reset_bit, _, data_bit] = bits::unpack_as_tuple<1, 6, 1> (data);
			const auto reg = bits::extract<2, 13> (addr);
			if (reset_bit)
				write_control (reg);
			else
			{
				_load.left (data_bit);
				if (++_bits >= 5u)
					write_control (reg);
			}
		}

		if constexpr (_Operation == kPeek)
		{
		}
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{}

	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset ()
	{}

private:
	Mirroring _mirroring{kFourScreen};
	byte _chrBank0{0};
	byte _chrBank1{0};
	byte _prgBank0{0};
	byte _prgBank1{0};
	byte _prgCount{0};
	byte _prgMode{0};
	byte _chrMode{0};
	bool _ramDisable{false};

	Bitarray<1, 5> _load{0};
	byte _bits{0};

	byte _prgBits [32][16_K];
	byte _chrBits [32][4_K];
	byte _ramBits [4][8_K];
};
