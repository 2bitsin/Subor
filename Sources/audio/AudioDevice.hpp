#pragma once

#include <core/CoreConfig.hpp>
#include <utils/Types.hpp>
#include <utils/SDL2.hpp>
#include <audio/AudioBuffer.hpp>
#include <utils/NonCopyable.hpp>
#include <utils/NonMovable.hpp>

struct AudioDevice
:	NonCopyable, NonMovable
{
	AudioDevice ();
	~AudioDevice ();

	void queue (const AudioBuffer& buff) const;
	void stop () const;	
	void start () const;
private:
	SDL_AudioSpec _config;
	SDL_AudioDeviceID _device;
};