// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "test/Nestest.hpp"
#include "core/StaticMemory.hpp"
#include <iterator>


#include "utils/Literals.hpp"

using namespace nestest;

extern const StaticMemory<kReadOnlyMemory, 32_K, 64_K, 16_K> PRG_ROM;
extern const StaticMemory<kReadOnlyMemory, 0_K, 8_K> CHR_ROM;
StaticMemory<kReadWriteMemory, 0_K, 8_K, 2_K> RAM;
StaticMemory<kReadWriteMemory, 24_K, 32_K> MPR_RAM;

const byte header [16] = 
{
	0x4E, 0x45, 0x53, 0x1A, 
	0x01, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
};


byte nestest::CPU_Peek (word addr)
{	
	struct {} s;
	byte data = 0xff;
	if (RAM.tick<kPeek>(s, addr, data) != kOpenBus)
		return data;
	if (MPR_RAM.tick<kPeek>(s, addr, data) != kOpenBus)
		return data;
	if (PRG_ROM.tick<kPeek>(s, addr, data) != kOpenBus)
		return data;
	return data;
}

void nestest::CPU_Poke (word addr, byte data)
{
	struct {} s;
	if (RAM.tick<kPoke>(s, addr, data) != kOpenBus)
		return;
	if (MPR_RAM.tick<kPoke>(s, addr, data) != kOpenBus)
		return;
	if (PRG_ROM.tick<kPoke>(s, addr, data) != kOpenBus)
		return;
	return;
}
