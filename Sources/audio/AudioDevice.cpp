#include "AudioDevice.hpp"

AudioDevice::AudioDevice()
:	_config 
	{
		.freq			= CoreConfig::ctSamplingRate,
		.format		= AUDIO_F32,
		.channels = CoreConfig::ctSampleChannels,
		.samples	= CoreConfig::ctSamplesPerFrame,
		.callback = nullptr,
		.userdata = nullptr
	},
	_device { SDL_OpenAudioDevice (nullptr, 0, &_config, &_config, 0) }
{}

AudioDevice::~AudioDevice ()
{
	if (_device > 0)
	{
		SDL_CloseAudioDevice (_device);
	}
}

void AudioDevice::queue (const AudioBuffer& buff) const
{
	SDL_QueueAudio (_device, buff.data (), buff.size ());
}

void AudioDevice::stop () const
{
	SDL_PauseAudioDevice (_device, 1);
}

void AudioDevice::start () const
{
	SDL_PauseAudioDevice (_device, 0);
}
