#pragma once

#include <utils/Options.hpp>
#include <core/Console.hpp>
#include <core/AudioVideoFrame.hpp>

#include <thread>
#include <atomic>
#include <mutex>
#include <array>
#include <tuple>

struct Frontend;

struct Backend
{

	inline static constexpr auto ctFrameCount = 2u;

	Backend (const Options&, Frontend&);
	~Backend ();

	template <typename... _Args>
	void input(_Args&& ... args)
	{
		_console->input(args...);
	}

protected:
	void emulate ();

private:
	Frontend& _frontend;
	std::unique_ptr<Console> _console;

	std::atomic<bool>	_quit;
	std::thread	_emuThread;

	std::array<AudioVideoFrame, ctFrameCount> _frame;
	std::atomic<dword> _index{0};
};