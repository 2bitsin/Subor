#pragma once

#include <video/PixelBuffer.hpp>
#include <audio/AudioBuffer.hpp>

struct AudioVideoFrame
{
	AudioVideoFrame()
	:	audio {
			CoreConfig::ctSamplesPerFrame},
		video {
			CoreConfig::ctHorizontalPixels, 
			CoreConfig::ctVerticalPixels}
	{}

	AudioBuffer audio;
	PixelBuffer video;
};
