// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Ines.hpp"
#include <fstream>
#include <cassert>

bool INes::load (INes& iNes, const std::filesystem::path& filePath)
{
	using namespace std::filesystem;
	if (!exists (filePath))
		return false;
	std::ifstream fst (filePath, std::ios::binary);
	if (!fst.is_open ())
		return false;
	char header [16u];
	fst.read (header, sizeof (header));
	if (fst.gcount () != sizeof (header))
		return false;
	if (std::memcmp (header, "NES\x1A", 4u))
		return false;
	INes tmp = { };

	tmp.trainerPresent = !!(header [6u] & 0x04u);

	const auto prgSize = 0x4000u * header [4u];
	const auto chrSize = 0x2000u * header [5u];

	tmp.prgBits.resize (prgSize);
	tmp.chrBits.resize (chrSize);

	if (tmp.trainerPresent)
	{
		tmp.trainer.resize (0x200);
		fst.read ((char*)tmp.trainer.data (), 0x200u);
		if (fst.gcount () != 0x200u)
			return false;
	}

	fst.read ((char*)tmp.prgBits.data (), prgSize);
	if (fst.gcount () != prgSize)
		return false;

	fst.read ((char*)tmp.chrBits.data (), chrSize);
	if (fst.gcount () != chrSize)
		return false;

	tmp.mapperId = (header [6u] >> 4u)|(header [7u] & 0xf0u);

	switch (header [6u] & 0x9u)
	{
	case 0x0u:
		tmp.mirroring = Mirroring::kHorizontal;
		break;
	case 0x1u:
		tmp.mirroring = Mirroring::kVertical;
		break;
	case 0x8u:
	case 0x9u:
	default:
		tmp.mirroring = Mirroring::kFourScreen;
		break;
	}

	tmp.batteryBacked = !!(header [6u] & 0x02u);	
	tmp.prgRamSize = header [8u] * 0x2000u;

	new (&iNes) INes (std::move (tmp));

	return true;
}
