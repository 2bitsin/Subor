#pragma once

#include <utils/Options.hpp>
#include <core/Console.hpp>

#include <thread>
#include <atomic>

struct Backend
{
	Backend(const Options&);
	~Backend();

private:
	void emulate();

private:
	std::unique_ptr<Console> _console;
	std::atomic<bool> _quit{false};
	std::thread _bgThread;
	
};