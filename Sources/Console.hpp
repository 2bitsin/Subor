// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "RicohCpu.hpp"
#include "RicohPPU.hpp"
#include "StaticMemory.hpp"
#include "RicohApu.hpp"
#include "MapperNROM.hpp"


struct Console
{
	using Memory = StaticMemory<kReadWriteMemory, 0u, 0x2000u, 0x800u>;
	using VideoMemory = StaticMemory<kReadWriteMemory, 0x2000u, 0x3000u>;

	double time{ 0.0 };
	RicohCPU cpu;
	RicohPPU ppu;
	RicohAPU apu;
	Memory mem;
	VideoMemory vmm;
	MapperNROM mmc;

	Console (std::filesystem::path filePath)
	:	cpu{ }
	,	ppu{ }
	,	apu{ }
	,	mem{ }
	,	vmm{ }
	,	mmc{ INes{ filePath } }
	{}

	template <MemoryOperation _Operation, typename _Slave, typename _Value>
	auto tick (_Slave&& slave, word addr, _Value&& data)
	{
		++time;
		ppu.tick<_Operation> (*this, addr, data);
		apu.tick<_Operation> (*this, addr, data);
		mem.tick<_Operation> (*this, addr, data);			
		mmc.tick<_Operation> (*this, addr, data);
		return kSuccess;
	}

	template <MemoryOperation _Operation, typename _Value>
	auto ppuTick (word addr, _Value&& data)
	{
		mmc.ppuTick<_Operation> (*this, addr, data);			
		vmm.tick<_Operation> (*this, ppuMirror (addr), data);
	}

	template <typename _Writter>
	void frame (_Writter&& write)
	{
		while (!ppu.ready ())
			cpu.step (*this, 1u);
		ppu.grabFrame (std::forward<_Writter>(write));
	}

	word ppuMirror (word addr) const
	{
		return mmc.ppuMirror (addr);
	}

	template <ResetType _Type>
	void reset ()
	{
		vmm.reset<_Type> ();
		mem.reset<_Type> ();
		mmc.reset<_Type> ();
		cpu.reset<_Type> ();
		ppu.reset<_Type> ();
		apu.reset<_Type> ();
	}

	template <typename... Args>
	void input(Args&&...new_state)
	{
		apu.input(new_state...);
	}

	auto width () const { return ppu.width (); }
	auto height () const { return ppu.height (); }

	void nmi () { cpu.raise(cpu.NonMaskableBit); }
	void irq () { cpu.raise(cpu.InterruptBit); }
}; 
