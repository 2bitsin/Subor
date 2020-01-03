#pragma once

#include <core/RicohCpu.hpp>
#include <core/StaticMemory.hpp>
#include <video/RicohPPU.hpp>
#include <audio/RicohApu.hpp>
#include <mapper/Mapper.hpp>
#include <utils/Literals.hpp>
#include <utils/ArrayView.hpp>

struct Console
{
	struct AudioBuffer 
	{
		array_view<float> buff;
		dword index { 0 };
		
		void lock()
		{}

		void unlock()
		{}

		auto append(float l, float r)
		{
			assert(buff.length() >= index + 2u);
			buff[index++] = l;
			buff[index++] = r;			
		}

		auto append(float l)
		{
			return append(l, l);
		}

	};

private:
	using Memory = StaticMemory<kReadWriteMemory, 0_K, 8_K, 2_K>;
	using VideoMemory = StaticMemory<kReadWriteMemory, 8_K, 12_K>;


	static constexpr auto _CPU_Cps = 1789773;
	static constexpr auto _PPU_Cps = 3 * _CPU_Cps;


	qword cnt_clock {0};
	qword cnt_frame {0};
	RicohCPU cpu;
	RicohPPU ppu;
	RicohAPU<AudioBuffer> apu;
	Memory mem;
	VideoMemory vmm;
	Mapper mmc;

public:
	Console ()
	{}

	void load (const ProgramROM& r);

	template <BusOperation _Operation, typename _Slave, typename _Value>
	auto tick (_Slave&& slave, word addr, _Value&& data)
	{
		using slave_type = std::remove_reference_t<_Slave>;
		mem.tick<_Operation> (*this, addr, data);
		ppu.tick<_Operation> (*this, addr, data);
		apu.tick<_Operation> (*this, addr, data);
		mmc.tick<_Operation> (*this, addr, data);
		++cnt_clock;
		return kSuccess;
	}

	template <BusOperation _Operation, typename _Value>
	auto ppuTick (word addr, _Value&& data)
	{
		vmm.tick<_Operation> (*this, ppuMirror (addr), data);
		mmc.ppuTick<_Operation> (*this, addr, data);
	}
	
	template <typename _Oaudio, typename _Ovideo, typename _Input>
	void emulate (_Oaudio& audio, _Ovideo& video, _Input& input_proxy)
	{		
		video.lock();		
		audio.lock();

		ppu.assign(video);
		apu.assign(audio);

		apu.push_input(input_proxy.read(*this));
		cpu.stepUntil (*this, [this] (auto&&...) 
			{ return ppu.ready (); });
		
		ppu.clearReady ();

		ppu.unassign();
		apu.unassign();
		
		audio.unlock();
		video.unlock();

		++cnt_frame;
	}

	bool is_odd_frame() const { return cnt_frame & 1u ; }
	bool is_even_frame() const { return !is_odd_frame() ; }
	auto count_frames () const { return cnt_frame ; }
	auto count_clocks () const { return cnt_clock ; }

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


	word width () const;
	word height () const;
	void nmi ();
	void irq (bool q = true);

	bool isOamDma () const;

};
