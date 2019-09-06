// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

enum MemoryType: byte
{
	kReadableMemory = 0x1,
	kWritableMemory = 0x2,
	kReadOnlyMemory = kReadableMemory,
	kReadWriteMemory = kReadableMemory|kWritableMemory,
};

enum MemoryOperation: byte
{
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


