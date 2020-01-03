#pragma once

#include <cstdint>
#include <cstddef>

using byte = std::uint8_t;
using word = std::uint16_t;
using dword = std::uint32_t;
using qword = std::uint64_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

union Word
{
	word w;
	struct { byte l, h; };
	byte b[2u];
};
