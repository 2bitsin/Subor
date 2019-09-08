#pragma once
#include "AudioChannel.hpp"

struct NoiseChannel
: public AudioChannel<byte, 0, 15> 
{
};
