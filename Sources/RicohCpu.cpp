// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "RicohCpu.hpp"

void RicohCPU::raise (byte bits)
{
	static constexpr auto m = ResetBit|InterruptBit|NonMaskableBit;
	assert (!(bits & ~m));
	q.mode.bits |= (bits & m);
}
