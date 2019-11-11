// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include <core/RicohCpu.hpp>
#include <core/StaticMemory.hpp>
#include <video/RicohPPU.hpp>
#include <audio/RicohApu.hpp>
#include <mapper/Mapper.hpp>
#include <utils/Literals.hpp>
#include <core/AudioVideoFrame.hpp>

struct Console
{
	using Memory = StaticMemory<kReadWriteMemory, 0_K, 8_K, 2_K>;
	using VideoMemory = StaticMemory<kReadWriteMemory, 8_K, 12_K>;
	using AudioFrame = AudioBuffer;

	static constexpr auto _CPU_Cps = 1789773;
	static constexpr auto _PPU_Cps = 3 * _CPU_Cps;

	double time{0.0};
	RicohCPU cpu;
	RicohPPU ppu;
	RicohAPU apu;
	Memory mem;
	VideoMemory vmm;
	Mapper mmc;

	Console ()
	: cpu{}
	, ppu{}
	, apu{}
	, mem{}
	, vmm{}
	, mmc{}
	{}

	void load (const ProgramROM& r);

	template <BusOperation _Operation, typename _Slave, typename _Value>
	auto tick (_Slave&& slave, word addr, _Value&& data)
	{
		using slave_type = std::remove_reference_t<_Slave>;

		++time;
		mem.tick<_Operation> (*this, addr, data);
		ppu.tick<_Operation> (*this, addr, data);
		apu.tick<_Operation> (*this, addr, data);
		mmc.tick<_Operation> (*this, addr, data);
		return kSuccess;
	}

	template <BusOperation _Operation, typename _Value>
	auto ppuTick (word addr, _Value&& data)
	{
		vmm.tick<_Operation> (*this, ppuMirror (addr), data);
		mmc.ppuTick<_Operation> (*this, addr, data);
	}

	
	void emulate (AudioVideoFrame& frame)
	{
		frame.audio.lock();
		frame.video.lock();		

		ppu.assign(frame.video);
		apu.assign(frame.audio);

		cpu.stepUntil (*this, [this] (auto&&...)
		{
			return ppu.ready ();
		});

		ppu.clearReady ();

		ppu.unassign();
		apu.unassign();

		frame.audio.unlock();
		frame.video.unlock();
	}


	word ppuMirror (word addr) const;

	template <ResetType _Type>
	void reset ()
	{
		cpu.reset<_Type> ();
		mem.reset<_Type> ();
		vmm.reset<_Type> ();
		ppu.reset<_Type> ();
		apu.reset<_Type> ();
		mmc.reset<_Type> ();
	}

	template <typename... Args>
	void input (Args&& ...new_state)
	{
		apu.input (new_state...);
	}

	word width () const;
	word height () const;
	void nmi ();
	void irq (bool q = true);

	bool isOamDma () const;

};
