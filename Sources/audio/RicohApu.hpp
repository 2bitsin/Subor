// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "core/Memory.hpp"
#include "video/RicohPPU.hpp"
#include "audio/TriangleChannel.hpp"
#include "audio/PulseChannel.hpp"
#include "audio/NoiseChannel.hpp"
#include "audio/DeltaChannel.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
{
	static constexpr auto ctSamplingRate = 48000ul;
	static constexpr auto ctSampleChannels = 2u;
	static constexpr auto ctFramesPerSecond = 60ul;
	static constexpr auto ctSamplesPerFrame = ctSamplingRate / ctFramesPerSecond;
	static constexpr auto ctCPUClksPerFrame = (RicohPPU::ctTotalTicks - 0.5) / 3.0;
	static constexpr auto ctCPUClksPerSample = ctCPUClksPerFrame/ctSamplesPerFrame;
	static constexpr auto ctCPUClkFracDivider = qword((1.0/ctCPUClksPerSample) * 0x100000000ull);
	static constexpr auto ctSampleTimeDelta = 1.0 / ctSamplingRate;

	using AudioBuffer = std::vector<std::pair<float, float>>;

	RicohAPU()
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host& master, word addr, _Value&& data)
	{
		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = 0u;
			if constexpr (_Operation == kPoke);

			switch (addr)
			{
			case 0x00: 
			case 0x01: 
			case 0x02: 
			case 0x03: 
			case 0x04: 
			case 0x05: 
			case 0x06:
			case 0x07:
			case 0x08: 
			case 0x09: 
			case 0x0A: 
			case 0x0B: 
			case 0x0C: 
			case 0x0D: 
			case 0x0E: 
			case 0x0F:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x18:
			case 0x19:
			case 0x1A:
			case 0x1B:
			case 0x1C:
			case 0x1D:
			case 0x1E:
			case 0x1F:
				break;
			case 0x16:
				if constexpr (_Operation == kPoke)
					inputStrobe<_Operation, _Host>(data);
				if constexpr (_Operation == kPeek)
					inputRead<_Operation, _Host>(addr, data);
				break;
			case 0x17:
				if constexpr (_Operation == kPeek)
					inputRead<_Operation, _Host>(addr, data);
				break;
			}			
		}		
		tickInternal();
		return kSuccess;
	}

	template <ResetType _Type>
	void reset()
	{
		triangle.reset();
		pulse0.reset();
		pulse1.reset();
		noise.reset();		
	}

	template <typename... Args>
	void input(Args&& ... values)
	{
		byte new_state [4u] = {values...};
		input_state [0] = new_state [0];
		input_state [1] = new_state [1];
		input_state [2] = new_state [2];
		input_state [3] = new_state [3];
	}

	template <typename _Sink>
	void grabFrame(_Sink&& sink)
	{
		sink(buffer);
	#ifdef _DEBUG
		std::printf("size = %zu, clk = %llu, sindex = %llu\n", buffer.size(), cpuclk, sindex);
	#endif
		buffer.clear();
		cpuclk = 0;
		sindex = 0;
	}

private:

	void mixAudio()
	{
			auto _pl0 = pulse0.value(ctSampleTimeDelta);
			auto _pl1 = pulse1.value(ctSampleTimeDelta);
			auto _tri = triangle.value(ctSampleTimeDelta);
			auto _noi = noise.value(ctSampleTimeDelta);
			auto _dmc = delta.value(ctSampleTimeDelta);

			float g0 = 0.0f;
			if (_tri || _noi || _dmc)
			{				                             
				g0 = (_tri / 8227.0f) + (_noi / 12241.0f) + (_dmc / 22638.0f);
				g0 = 1.0f/g0 + 100.0f;
        g0 = 159.79f / g0;
			}
			
			float g1 = 0.0f;
			if (_pl0 || _pl1)
			{
				g1 = (8128.0f / (_pl0 + _pl1)) + 100.0f;
				g1 = 95.88f / g1;
			}
			
			g0 = g0 + g1;
			buffer.emplace_back(g0, g0);
	}

	void tickInternal()
	{
		++cpuclk;
		auto last_state = (clkdiv >> 32u) & 1u;
		clkdiv += ctCPUClkFracDivider;
		auto state = (clkdiv >> 32u) & 1u;
		if (state != last_state)
		{
			mixAudio();
			++sindex;
		}
	}

	template<MemoryOperation _Operation, typename _Host, typename _Value>
	void inputRead(const word& addr, _Value&& data)
	{
			auto i = (addr - 0x16) & 1u;
			data = (data & 0xe0) | (input_shift[i] & 1u);
			if (!(input_latch & 1u))
				input_shift[i] >>= 1u;
	}

	template<MemoryOperation _Operation, typename _Host, typename _Value>
	void inputStrobe(_Value&& data)
	{
			for (auto i = 0; i < std::size(input_shift); ++i)
			{
				input_latch = (byte)data;
				if (input_latch & 1u)
					input_shift[i] = input_state[i];
			}
	}

private:
	byte						input_state[4u] = {0, 0, 0, 0};
	byte						input_shift[4u] = {0, 0, 0, 0};
	byte						input_latch{0};

	qword						cpuclk { 0ull };
	qword						clkdiv { 0ull };
	qword						sindex { 0ull };
	AudioBuffer			buffer;
	TriangleChannel triangle;
	PulseChannel		pulse0;
	PulseChannel		pulse1;
	NoiseChannel		noise;
	DeltaChannel	  delta;
};