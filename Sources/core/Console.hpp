// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/RicohCpu.hpp"
#include "core/StaticMemory.hpp"
#include "video/RicohPPU.hpp"
#include "audio/RicohApu.hpp"
#include "mapper/MapperNROM.hpp"
#include "utils/Literals.hpp"


struct Console
{
	using Memory = StaticMemory<kReadWriteMemory, 0_K, 8_K, 2_K>;
	using VideoMemory = StaticMemory<kReadWriteMemory, 8_K, 12_K>;

	static constexpr auto _CPU_Cps = 1789773;
	static constexpr auto _PPU_Cps = 3 * _CPU_Cps;

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

	template <BusOperation _Operation, typename _Slave, typename _Value>
	auto tick (_Slave&& slave, word addr, _Value&& data)
	{
		++time;
		ppu.tick<_Operation> (*this, addr, data);
		apu.tick<_Operation> (*this, addr, data);
		mem.tick<_Operation> (*this, addr, data);			
		mmc.tick<_Operation> (*this, addr, data);
		return kSuccess;
	}

	template <BusOperation _Operation, typename _Value>
	auto ppuTick (word addr, _Value&& data)
	{
		mmc.ppuTick<_Operation> (*this, addr, data);			
		vmm.tick<_Operation> (*this, ppuMirror (addr), data);
	}

	template <typename _VideoSink, typename _AudioSink>
	void frame (_VideoSink&& video, _AudioSink&& audio)
	{
		while (!ppu.ready ())
			cpu.step (*this, 1u);
		ppu.grabFrame (std::forward<_VideoSink>(video));
		apu.grabFrame (std::forward<_AudioSink>(audio));
	}

	template <typename _Sink>
	auto audio(_Sink&& sink)
	{
		return apu.grabFrame(std::forward<_Sink>(sink));
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

	void nmi () { cpu.setSignal(cpu.NonMaskableBit); }

	void irq (bool q=true) 
	{
		if (q)
			cpu.setSignal(cpu.InterruptBit); 
		else
			cpu.clrSignal(cpu.InterruptBit); 
	}
	
}; 
