// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include <memory>
#include <cassert>
#include <string>
#include <vector>

enum Mirroring: byte
{
	kFourScreen,
	kHorizontal,
	kVertical
};

struct ProgramROM
{
private:
	byte _mapperId{};
	std::vector<byte> _prgBits{};
	std::vector<byte> _chrBits{};
	std::vector<byte> _trainer{};
	dword _ramSize{};
	Mirroring _mirroring{};
	bool _batteryBacked{};
	bool _trainerPresent{};
public:

	ProgramROM ()
	{}

	ProgramROM (const std::string& filePath)
	{
		load (*this, filePath);
	}

	dword prgSize () const
	{
		return dword(_prgBits.size ());
	}

	dword chrSize () const
	{
		return dword(_chrBits.size ());
	}

	dword ramSize () const
	{
		return _ramSize;
	}

	bool hasTrainer () const
	{
		return _trainerPresent;
	}

	bool hasBattery () const
	{
		return _batteryBacked;
	}

	Mirroring mirroring () const
	{
		return _mirroring;
	}

	auto&& chr (std::size_t i) const
	{
		assert (chrSize () > 0);
		return _chrBits [i % chrSize ()];
	}

	auto&& prg (std::size_t i) const
	{
		assert (prgSize () > 0);
		return _prgBits [i % prgSize ()];
	}

	auto&& trainer (std::size_t i) const
	{
		assert (_trainer.size () > 0);
		return _trainer [i % _trainer.size ()];
	}

	template <typename _Target>
	auto prgTransfer(_Target& dst) const
	{
		if (prgSize() > 0u)
			copy(dst, _prgBits, _prgBits.size());
	}

	template <typename _Target>
	auto chrTransfer(_Target& dst) const
	{
		if (chrSize() > 0u)
			copy(dst, _chrBits, _chrBits.size());
	}

	template <typename _Target>
	auto trainerTransfer(_Target& dst) const
	{
		if (hasTrainer ())
			copy(dst, _trainer, _trainer.size());
	}

	auto idMapper() const
	{
		return _mapperId;
	}

	static bool load (ProgramROM& iNes, const std::string& filePath);

protected:
	template <std::size_t _Size, typename _Source, typename _Length>
	auto copy (byte (&dst) [_Size], _Source&& src, _Length&& len) const
	{
		for (auto indx = 0u; indx < _Size; ++indx)
			dst[indx] = src[indx%len];
	}


	template <std::size_t _Banks, std::size_t _Size, typename _Source, typename _Length>
	auto copy (byte (&dst) [_Banks][_Size], _Source&& src, _Length&& len) const
	{
		for (auto bank = 0u; bank < _Banks; ++bank)
			for (auto indx = 0u; indx < _Size; ++indx)
				dst[bank][indx] = src[(_Size*bank + indx)%len];
	}
};
