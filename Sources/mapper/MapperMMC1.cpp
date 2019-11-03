#include "MapperMMC1.hpp"

MapperMMC1::MapperMMC1(const ProgramROM& iNes)
:	_mirroring{iNes.mirroring()}
{
	iNes.prgTransfer(_prgBits);
	iNes.chrTransfer(_chrBits);
}

void MapperMMC1::write_control (byte reg)
{
	const auto data = _load.value ();
	_load.value (0);
	_bits = 0u;
	switch (reg)
	{
	case 0:
		{
			const auto [m, p, c] = bits::unpack_as_tuple<2, 2, 1> (data);
			set_mirroring (m);
			_chrMode = c;
			_prgMode = p;
			break;
		}
	case 1:
		if (!_chrMode)
		{
			_chrBank0 = (data & 0b11110);
			_chrBank1 = (data & 0b11110) + 1u;
		}
		else
			_chrBank0 = data;
		break;
	case 2:
		if (_chrMode)
			_chrBank1 = data;
		break;
	case 3:
		{
			const auto [bank, ramd] = bits::unpack_as_tuple<4, 1> (data);
			if (_prgMode & 0b10)
			{
				if (_prgMode & 1u)
				{
					_prgBank0 = bank;
					_prgBank1 = _prgCount - 1u;
				}
				else
				{
					_prgBank0 = 0;
					_prgBank1 = bank;
				}
			}
			else
			{
				_prgBank0 = (bank & 0b1110);
				_prgBank1 = (bank & 0b1110) + 1u;
			}
			_ramDisable = !!ramd;
			break;
		}
	}
}

void MapperMMC1::set_mirroring (byte m)
{
	switch (m)
	{
	case 0u: _mirroring = kSingleLow; break;
	case 1u: _mirroring = kSingleHigh; break;
	case 2u: _mirroring = kVertical; break;
	case 3u: _mirroring = kHorizontal; break;
	}
}

word MapperMMC1::ppuMirror (word addr) const
{
	return basicMirror (_mirroring, addr);
}
