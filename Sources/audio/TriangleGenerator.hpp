#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"

struct TriangleGenerator
{
	byte step ()
	{
		auto v = t & 0x10 ? (15 - t & 0xf) : t & 0xf;
		++t;	
		return v;
	}
private:
	byte t;	
};