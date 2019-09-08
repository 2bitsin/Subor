// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "Mapper.hpp"

struct MapperNROM
:	public Mapper<MapperNROM>
{
	MapperNROM (const INes& iNes);
	MapperNROM ();

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tick(_Host&&, word addr, _Value&& data) 
	{
		if (addr < 0x6000u)
			return kSuccess;
		addr -= 0x6000u;
		if (addr < 0x2000u)
		{
			if constexpr (_Operation == kPeek)
				if (trainerPresent_ && addr >= 0x1e00u)
				{
					data = cpuBits_ [addr];
					return kSuccess;
				}

			if (ramSize_ < 1u)
				return kSuccess;
			if constexpr (_Operation == kPeek)
				data = cpuBits_ [addr % ramSize_];
			if constexpr (_Operation == kPoke)
				cpuBits_ [addr % ramSize_] = (byte)data;
			return kSuccess;
		}
		if constexpr (_Operation == kPeek)
			data = cpuBits_ [addr];	
		return kSuccess;
	}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto ppuTick(_Host&&, word addr, _Value&& data)
	{
		if (addr < 0x2000u)
		{
			if constexpr (_Operation == kPeek)
				data = ppuBits_[addr];
			return kSuccess;
		}
		return kOpenBus;
	}
	
	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset()
	{
		if constexpr (_Type == kHardReset)
		{
			auto l = 0x2000u;
			if (trainerPresent_)
				l -= 0x200;
			for(auto i = 0u; i < l; ++i)
				cpuBits_[i] = 0;
		}
	}

	
private:
	Mirroring mirroring_;
	word ramSize_;
	bool trainerPresent_;
	byte cpuBits_[0xA000u];
	byte ppuBits_[0x2000u];
};