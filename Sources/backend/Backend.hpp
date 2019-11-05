#pragma once

#include <thread>
#include <atomic>

struct Backend
{
	Backend();
	~Backend();

private:
	void emulate();

private:
	std::atomic<bool> _quit{false};
	std::thread _bgThread;
	
};