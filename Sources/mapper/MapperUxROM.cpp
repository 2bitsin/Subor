#include "MapperUxROM.hpp"

#include <algorithm>

MapperUxROM::MapperUxROM (const ProgramROM& iNes)
:	_regBank{0},
	_banks{MapperBase::calcBanks<byte>(iNes.prgSize(), std::size(_prgBits[0]))},
	_hasTrainer{iNes.hasTrainer()},
	_mirroring{iNes.mirroring()}
{
	assert(!iNes.ramSize());

	iNes.prgTransfer(_prgBits);
	iNes.chrTransfer(_chrBits);
	iNes.trainerTransfer(_trainer);
}

word MapperUxROM::ppuMirror (word addr) const
{
	return basicMirror (_mirroring, addr);
}
