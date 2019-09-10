#pragma once

#include "utils/Types.hpp"

#include <chrono>

namespace __detail__
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using std::chrono::seconds;
	static constexpr auto _1s2ns = duration_cast<nanoseconds>(seconds(1));
}

struct CoreConfig
{
	static constexpr const auto ctFramesPerSecond = 60ul;
	static constexpr const auto ctFrameLengthInNs = __detail__::_1s2ns / ctFramesPerSecond;
	static constexpr const auto ctHorizontalTicks = 341u;
	static constexpr const auto ctVerticalTicks = 262u;
	static constexpr const auto ctVblankScanline = 241u;
	static constexpr const auto ctPPUTicksPerCPUTick = 3u;
	static constexpr const auto ctTotalTicks = ctHorizontalTicks * ctVerticalTicks;
	static constexpr const auto ctPPUTicksPerSecond = qword((ctTotalTicks - 0.5)*60.0);
	static constexpr const auto ctCPUTicksPerSecond = qword(ctPPUTicksPerSecond/ctPPUTicksPerCPUTick);
	static constexpr const auto ctHorizontalPixels = 256u;
	static constexpr const auto ctVerticalPixels = 240u;
	static constexpr const auto ctNmiTimeout = 15u;
	static constexpr const auto ctSamplingRate = 48000ul;
	static constexpr const auto ctSampleChannels = 2u;
	static constexpr const auto ctSamplesPerFrame = ctSamplingRate / ctFramesPerSecond;
	static constexpr const auto ctCPUTicksPerFrame = (ctTotalTicks - 0.5) / 3.0;
	static constexpr const auto ctSampleTimeDelta = 1.0 / ctSamplingRate;
};