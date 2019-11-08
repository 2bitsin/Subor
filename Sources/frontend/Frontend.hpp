#pragma once

#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <core/CoreConfig.hpp>
#include <core/AudioVideoFrame.hpp>

#include <vector>
#include <string>
#include <mutex>

struct Backend;

struct Frontend
{

	Frontend (const Options& options);
	~Frontend ();

	SDL_Window* handle () const;
	int mainThread();

	bool notifyFrame(const AudioVideoFrame&);

	void frameConsume ();

private:
	int dispatch (const SDL_Event& ev);

private:
	SDL_Window* _window{nullptr};
	const AudioVideoFrame* _frame{nullptr};
	std::mutex _lockFrame;
};
