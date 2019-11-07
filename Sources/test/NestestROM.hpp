#pragma once

#include <core/Memory.hpp>

namespace nestest
{
	extern const StaticMemory<kReadOnlyMemory, 32_K, 64_K, 16_K> _prg;
	extern const StaticMemory<kReadOnlyMemory, 0_K, 8_K> _chr;
	extern const byte _hdr[16];
}