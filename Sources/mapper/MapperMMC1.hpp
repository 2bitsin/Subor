#pragma once

#include "MapperBase.hpp"
#include "utils/Bitarray.hpp"

struct MapperMMC1
	: public MapperBase<MapperMMC1>
{
	MapperMMC1 (const ProgramROM& iNes);

	void write_control (byte reg);
	void set_mirroring (byte m);

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& host, word addr, _Value&& data)
	{
		if constexpr (_Operation == kPoke)
		{
			if (addr < 0x6000u)
			{
			}
			else if (addr < 0x8000u)
			{
				if (!_ramDisable)
				{
					addr -= 0x6000u;
					_ramBits [_ramBank][addr] = byte (data);
				}
			}
			else
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
		}

		if constexpr (_Operation == kPeek)
		{
			if (addr < 0x6000u)
			{
			}
			else if (addr < 0x8000u)
			{
				if (!_ramDisable)
				{
					addr -= 0x6000u;
					data = _ramBits [_ramBank][addr];
				}
			}
			else if (addr < 0xC000u)
			{
				addr -= 0x8000u;
				data = _prgBits [_prgBank0][addr];
			}
			else
			{
				addr -= 0xC000u;
				data = _prgBits [_prgBank1][addr];
			}
		}
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{
		if constexpr (_Operation == kPeek)
		{
			if (addr < 0x1000u)
			{
				data = _chrBits [_chrBank0][addr];
			}
			else if (addr < 0x2000u)
			{
				addr -= 0x1000u;
				data = _chrBits [_chrBank1][addr];
			}
		}
	}

	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset ()
	{
		_prgBank0 = 0;
		_prgBank1 = 0;
		_chrBank0 = 0;
		_chrBank1 = 0;
		_prgMode = 0;
		_chrMode = 0;
		_ramDisable = false;
		_ramBank = 0;
		_load.bits = 0;
		_bits = 0;

		if (_Type == kHardReset)
		{
			for (auto&& bank : _ramBits)
				for (auto&& cell : bank)
					cell = 0u;
		}
	}

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
	byte _ramBank = 0;

	Bitarray<1, 5> _load{0};
	byte _bits{0};

	byte _prgBits [32][16_K];
	byte _chrBits [32][4_K];
	byte _ramBits [4][8_K];
};
