// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ProgramROM.hpp"
#include <fstream>
#include <cassert>

bool ProgramROM::load (ProgramROM& iNes, const std::string& filePath)
{
	std::ifstream fst (filePath, std::ios::binary);
	if (!fst.is_open ())
		return false;
	char header [16u];
	fst.read (header, sizeof (header));
	if (fst.gcount () != sizeof (header))
		return false;
	if (std::memcmp (header, "NES\x1A", 4u))
		return false;
	ProgramROM tmp = { };

	tmp._trainerPresent = !!(header [6u] & 0x04u);

	const auto prgSize = 0x4000u * header [4u];
	const auto chrSize = 0x2000u * header [5u];

	tmp._prgBits.resize (prgSize);
	tmp._chrBits.resize (chrSize);

	if (tmp._trainerPresent)
	{
		tmp._trainer.resize (0x200);
		fst.read ((char*)tmp._trainer.data (), 0x200u);
		if (fst.gcount () != 0x200u)
			return false;
	}

	fst.read ((char*)tmp._prgBits.data (), prgSize);
	if (fst.gcount () != prgSize)
		return false;

	fst.read ((char*)tmp._chrBits.data (), chrSize);
	if (fst.gcount () != chrSize)
		return false;

	tmp._mapperId = (header [6u] >> 4u)|(header [7u] & 0xf0u);

	switch (header [6u] & 0x9u)
	{
	case 0x0u:
		tmp._mirroring = Mirroring::kHorizontal;
		break;
	case 0x1u:
		tmp._mirroring = Mirroring::kVertical;
		break;
	case 0x8u:
	case 0x9u:
	default:
		tmp._mirroring = Mirroring::kFourScreen;
		break;
	}

	tmp._batteryBacked = !!(header [6u] & 0x02u);	
	tmp._ramSize = header [8u] * 0x2000u;

	new (&iNes) ProgramROM (std::move (tmp));

	return true;
}
