// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "input/InputPort.hpp"
#include "video/RicohPPU.hpp"
#include "audio/AudioBuffer.hpp"

#include <vector>
#include <tuple>

struct RicohAPU
	: public CoreConfig
{

	RicohAPU ()
	:	_buffer(ctSamplesPerFrame)
	{}

	template <MemoryOperation _Operation, typename _Host, typename _Data>
	auto tick (_Host& host, word addr, _Data&& data)
	{
		if (addr >= 0x4000u && addr < 0x4200u)
		{
			addr -= 0x4000u;
			addr &= 0x1fu;

			if constexpr (_Operation == kPeek)
				data = latch;
			if constexpr (_Operation == kPoke)
				latch = byte (data);

			switch (addr)
			{
			case 0x16:
				if constexpr (_Operation == kPoke)
					_input.write (data);
				if constexpr (_Operation == kPeek)
					_input.read<0> (data);
				break;
			case 0x17:
				if constexpr (_Operation == kPeek)
					_input.read<1> (data);
				break;
			}
		}
		return kSuccess;
	}

	template <ResetType _Type>
	void reset ()
	{}

	template <typename _Sink>
	void grabFrame (_Sink&& sink)
	{
		sink (_buffer);
		_buffer.clear ();
	}

	template <typename... _Args>
	auto input(_Args&& ... args)
	{
		return _input.set(std::forward<_Args>(args)...);
	}

private:

	float mix (float sq0, float sq1, float tri, float noi, float dmc)
	{
		auto _sq0 = 15.0f * sq0;
		auto _sq1 = 15.0f * sq1;
		auto _tri = 15.0f * tri;
		auto _noi = 15.0f * noi;
		auto _dmc = 127.0f * dmc;

		float g0 = 0.0f;
		if (_tri || _noi || _dmc)
		{
			g0 = (_tri / 8227.0f) + (_noi / 12241.0f) + (_dmc / 22638.0f);
			g0 = 1.0f / g0 + 100.0f;
			g0 = 159.79f / g0;
		}

		float g1 = 0.0f;
		if (_sq0 || _sq1)
		{
			g1 = (8128.0f / (_sq0 + _sq1)) + 100.0f;
			g1 = 95.88f / g1;
		}

		return g0 + g1;
	}


private:

	byte latch{0u};
	InputPort	_input;
	AudioBuffer<float> _buffer;
};