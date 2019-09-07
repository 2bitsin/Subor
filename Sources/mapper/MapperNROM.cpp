// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "MapperNROM.hpp"

MapperNROM::MapperNROM (const INes& iNes)
: trainerPresent_ (iNes.trainerPresent)
, mirroring_ (iNes.mirroring)
, ramSize_ (iNes.prgRamSize)
, cpuBits_{ }
, ppuBits_{ }
{
	printf ("Mirroring = %d\n", iNes.mirroring);
	if (iNes.mapperId != 0)
		throw std::runtime_error ("Unsupported mapper");
	for (auto i = 0u; i < sizeof (ppuBits_); ++i)
		ppuBits_ [i] = iNes.chrBits [i % iNes.chrBits.size ()];
	for (auto i = 0u; i < 0x8000u; ++i)
		cpuBits_ [0x2000u + i] = iNes.prgBits [i % iNes.prgBits.size ()];
	if (!trainerPresent_)
		return;
	for (auto i = 0u; i < 0x200u; ++i)
		cpuBits_ [0x1e00u + i] = iNes.trainer [i % iNes.trainer.size ()];
}

MapperNROM::MapperNROM ()
: trainerPresent_{ false }
, mirroring_{ kHorizontal }
, ramSize_{ 0u }
, cpuBits_{ }
, ppuBits_{ }
{}

word MapperNROM::ppuMirror (word addr) const
{
	return basicMirror (mirroring_, addr);
}
