#pragma once

#include <core/CoreConfig.hpp>
#include <utils/Types.hpp>
#include <utils/SDL2.hpp>
#include <utils/NonCopyable.hpp>
#include <utils/NonMovable.hpp>
#include <utils/Types.hpp>
#include <utils/ArrayView.hpp>

#include <mutex>

template
<	typename _Host,
	typename _Config = CoreConfig>
struct AudioDevice
: NonCopyable, NonMovable
{
	AudioDevice (_Host& host)
	: host_ { host },
		config_
		{
			.freq			= _Config::ctSamplingRate,
			.format		= AUDIO_F32,
			.channels = _Config::ctSampleChannels,
			.samples	= _Config::ctSamplesPerFrame,
			.callback = (SDL_AudioCallback)&AudioDevice::callback,
			.userdata = this
		},
		device_ { SDL_OpenAudioDevice (nullptr, 0, &config_, &config_, 0) }
	{}

	static void callback (AudioDevice* that, byte* data, int size)
	{
		if (!that->join_.try_lock ())
			return;
		size = (size + sizeof (float) - 1) / sizeof (float);		
		auto& host = that->host_;
		std::lock_guard g { that->join_, std::adopt_lock };
		host.audio_callback (array_view<float>{
			(float*)data, (dword)size
		});
	}

	~AudioDevice ()
	{
		std::lock_guard g { join_ };
		if (device_ > 0)
		{
			stop ();
			SDL_CloseAudioDevice (device_);
		}
	}

	void stop () const
	{
		SDL_PauseAudioDevice (device_, 1);
	}

	void start () const
	{
		SDL_PauseAudioDevice (device_, 0);
	}

private:
	_Host& host_;
	SDL_AudioSpec config_;
	SDL_AudioDeviceID device_;
	std::mutex join_;
};

#include <utils/Types.hpp>


