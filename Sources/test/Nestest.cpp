// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <iterator>

#include <core/StaticMemory.hpp>
#include <utils/Literals.hpp>
#include <test/Nestest.hpp>
#include <test/NestestROM.hpp>

using namespace nestest;

StaticMemory<kReadWriteMemory, 0_K, 8_K, 2_K> RAM;
StaticMemory<kReadWriteMemory, 24_K, 32_K> MPR_RAM;

byte nestest::CPU_Peek (word addr)
{	
	struct {} s;
	byte data = 0xff;
	if (RAM.tick<kPeek>(s, addr, data) != kOpenBus)
		return data;
	if (MPR_RAM.tick<kPeek>(s, addr, data) != kOpenBus)
		return data;
	if (_prg.tick<kPeek>(s, addr, data) != kOpenBus)
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
	if (_prg.tick<kPoke>(s, addr, data) != kOpenBus)
		return;
	return;
}


ProgramROM nestest::AsProgramROM ()
{
	return ProgramROM(_hdr, _prg.bits.data(), _chr.bits.data(), nullptr);
}


