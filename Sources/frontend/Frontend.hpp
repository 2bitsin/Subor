#pragma once

#include <core/CoreConfig.hpp>
#include <utils/Options.hpp>
#include <utils/SDL2.hpp>
#include <video/PixelBuffer.hpp>
#include <audio/AudioBuffer.hpp>

#include <vector>
#include <string>
#include <mutex>

struct Frontend: CoreConfig
{
	Frontend (const Options& options);
	~Frontend ();

	void open ();
	void close ();

	auto handle() const { return _window; }

	void pushFrame(const AudioBuffer<float>&, const PixelBuffer<dword>&);

	int mainThread() ;

private:

	SDL_Window* _window{nullptr};
	std::mutex _mutex_buff;
	const AudioBuffer<float>* _audio_buff{nullptr};
	const PixelBuffer<dword>* _pixel_buff{nullptr};
};