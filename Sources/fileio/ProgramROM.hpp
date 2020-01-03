#pragma once

#include "utils/Types.hpp"
#include <memory>
#include <cassert>
#include <string>
#include <vector>

enum Mirroring: byte
{
	kSingleLow,
	kSingleHigh,
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

	ProgramROM ();
	ProgramROM (const std::string& filePath);
	ProgramROM (const byte* header, const byte* prgbit, const byte* chrbit, const byte* trainr);

	dword prgSize () const;
	dword chrSize () const;
	dword ramSize () const;
	bool hasTrainer () const;
	bool hasBattery () const;
	Mirroring mirroring () const;
	byte chr (std::size_t i) const;
	byte prg (std::size_t i) const;
	byte trainer (std::size_t i) const;
	byte idMapper () const;

	template <typename _Target>
	auto prgTransfer (_Target& dst) const
	{
		if (prgSize () > 0u)
			copy (dst, _prgBits, _prgBits.size ());
	}

	template <typename _Target>
	auto chrTransfer (_Target& dst) const
	{
		if (chrSize () > 0u)
			copy (dst, _chrBits, _chrBits.size ());
	}

	template <typename _Target>
	auto trainerTransfer (_Target& dst) const
	{
		if (hasTrainer ())
			copy (dst, _trainer, _trainer.size ());
	}
	
	static bool load (ProgramROM& iNes, const std::string& filePath);
	static bool load (ProgramROM& iNes, const byte* header, const byte* prgbit, const byte* chrbit, const byte* trainr);

protected:
	template <std::size_t _Size, typename _Source, typename _Length>
	auto copy (byte (&dst) [_Size], _Source&& src, _Length&& len) const
	{
		for (auto indx = 0u; indx < _Size; ++indx)
			dst [indx] = src [indx % len];
	}

	template <std::size_t _Banks, std::size_t _Size, typename _Source, typename _Length>
	auto copy (byte (&dst) [_Banks][_Size], _Source&& src, _Length&& len) const
	{
		for (auto bank = 0u; bank < _Banks; ++bank)
			for (auto indx = 0u; indx < _Size; ++indx)
				dst [bank][indx] = src [(_Size * bank + indx) % len];
	}
};
