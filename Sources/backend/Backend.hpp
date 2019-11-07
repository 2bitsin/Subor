#pragma once

#include <utils/Options.hpp>
#include <core/Console.hpp>
#include <audio/AudioBuffer.hpp>
#include <video/PixelBuffer.hpp>

#include <thread>
#include <atomic>
#include <mutex>
#include <array>
#include <tuple>

struct Frontend;

struct Backend
{
	inline static constexpr auto ct_buffer_count = 2u;

	Backend (const Options&, Frontend&);
	~Backend ();

private:
	void emulate ();

private:
	Frontend& _frontend;

	std::unique_ptr<Console> _console;
	std::array<AudioBuffer<float>, ct_buffer_count> _audio_buff;
	std::array<PixelBuffer<dword>, ct_buffer_count> _pixel_buff;
	byte _buff_index;

	std::atomic<bool>	_quit;
	std::thread	_emu_thread;

};