// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "MapperBase.hpp"
#include "utils/Literals.hpp"

struct MapperNROM
	: public MapperBase<MapperNROM>
{
	MapperNROM (const ProgramROM& iNes);

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& host, word addr, _Value&& data)
	{
		if (addr < 24_K)
			return;
		if (addr < 32_K)
		{
			if (_hasTrainer && addr >= 31.5_K)
			{
				if constexpr (_Operation == kPeek)
					data = _trainer [addr - 31.5_K];
				return;
			}
			if constexpr (_Operation == kPeek)
				data = _ramBits [addr - 24_K];
			if constexpr (_Operation == kPoke)
				_ramBits [addr - 24_K] = byte(data);
			return;
		}
		if constexpr (_Operation == kPeek)
			data = _prgBits [addr - 32_K];
		return;
	}

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&&, word addr, _Value&& data)
	{
		if (addr < 8_K)
		{
			if constexpr (_Operation == kPeek)
				data = _chrBits [addr];
		}
	}

	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset ()
	{
		if constexpr (_Type == kHardReset)
		{
			std::fill (std::begin (_ramBits), std::end (_ramBits), 0x00u);
		}
	}

private:
	dword				_ramSize;
	bool				_hasTrainer;
	Mirroring		_mirroring;
	byte				_ramBits [8_K];
	byte				_prgBits [32_K];
	byte				_chrBits [8_K];
	byte				_trainer [0.5_K];

};