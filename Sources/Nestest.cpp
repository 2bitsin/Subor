// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Nestest.hpp"
#include "StaticMemory.hpp"
#include <iterator>

using namespace nestest;

extern const StaticMemory<kReadOnlyMemory, 0x8000u, 0x10000u, 16384u> PRG_ROM;
extern const StaticMemory<kReadOnlyMemory, 0u, 8192u> CHR_ROM;
StaticMemory<kReadWriteMemory, 0u, 0x2000u, 0x800u> RAM;
StaticMemory<kReadWriteMemory, 0x6000u, 0x8000u> MPR_RAM;

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
