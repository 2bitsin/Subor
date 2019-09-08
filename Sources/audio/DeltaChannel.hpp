#pragma once

#include "AudioChannel.hpp"

struct DeltaChannel
: public AudioChannel<byte, 0, 127>
{
};
