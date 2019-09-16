#pragma once

#include "MapperBase.hpp"


struct MapperUxROM
:	public MapperBase<MapperUxROM>
{
	MapperUxROM (const ProgramROM& iNes);

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& host, word addr, _Value&& data)
	{
		if (addr < 32_K)
			return;	
		if constexpr (_Operation == kPeek)
		{
			if (addr < 48_K)			
				data = _prgBits[_regBank] [(addr - 32_K) % 16_K];
			else
				data = _prgBits[_banks - 1] [(addr - 48_K) % 16_K];
		}
		if constexpr (_Operation == kPoke)
		{
			_regBank = byte(data) % _banks;
		}
		return;
	}	

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{
		if (addr < 8_K)
		{
			if constexpr (_Operation == kPeek)
				data = _chrBits [addr];
			if constexpr (_Operation == kPoke)
				_chrBits [addr] = byte(data);
		}
	}

	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset ()
	{
		_regBank = 0;
	}

private:
	byte			_regBank;
	byte			_banks;
	bool			_hasTrainer;
	Mirroring _mirroring;
	byte			_prgBits[16][16_K];
	byte			_chrBits[8_K];
	byte			_trainer[0.5_K];
};