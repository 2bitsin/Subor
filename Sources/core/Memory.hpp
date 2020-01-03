#pragma once

#include "utils/Types.hpp"

enum MemoryType: byte
{
	kReadableMemory = 0x1,
	kWritableMemory = 0x2,
	kReadOnlyMemory = kReadableMemory,
	kReadWriteMemory = kReadableMemory|kWritableMemory,
};

enum BusOperation: byte
{
	kNoOp = 0x0,
	kPeek = 0x1,
	kPoke = 0x2,
	kDummyPeek = 0x4|kPeek,
	kDummyPoke = 0x4|kPoke
};

enum MemoryStatus: byte
{
	kOpenBus,
	kSuccess
};

enum ResetType: byte
{
	kSoftReset,
	kHardReset
};


