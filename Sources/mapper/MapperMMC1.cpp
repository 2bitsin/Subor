#include "MapperMMC1.hpp"

MapperMMC1::MapperMMC1(const ProgramROM& iNes)
:	_mirroring{iNes.mirroring()}
{
	iNes.prgTransfer(_prgBits);
	iNes.chrTransfer(_chrBits);
}

word MapperMMC1::ppuMirror (word addr) const
{
	return basicMirror (_mirroring, addr);
}
