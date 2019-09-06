// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "Types.hpp"
#include <memory>
#include <filesystem>

enum Mirroring: byte
{
	kFourScreen,
	kHorizontal,
	kVertical
};

struct INes
{
	INes () 
	{}

	INes (const std::filesystem::path& filePath)
	{
		load(*this, filePath);
	}

	byte mapperId {};
	std::vector<byte> prgBits {};
	std::vector<byte> chrBits {};
	std::vector<byte> trainer {};
	dword prgRamSize {};
	Mirroring mirroring {};
	bool batteryBacked {};
	bool trainerPresent {};

	static bool load(INes& iNes, const std::filesystem::path& filePath);
};
