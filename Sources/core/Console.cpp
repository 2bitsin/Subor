#include "Console.hpp"

void Console::load (std::string p)
{
	mmc.load (ProgramROM{p});
}

word Console::ppuMirror (word addr) const
{
	return mmc.ppuMirror (addr);
}

word Console::width () const
{
	return ppu.width ();
}

word Console::height () const
{
	return ppu.height ();
}

void Console::nmi ()
{
	cpu.setSignal (cpu.NonMaskableBit);
}

void Console::irq (bool q)
{
	if (q)
		cpu.setSignal (cpu.InterruptBit);
	else
		cpu.clrSignal (cpu.InterruptBit);
}

bool Console::isOamDma () const
{
	return cpu.inDmaMode ();
}
