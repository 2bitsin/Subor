#pragma once

#include <utils/Types.hpp>
#include <fileio/ProgramROM.hpp>

namespace nestest
{
	struct LogLine
	{  
		word addr;
		struct { byte a, x, y, p, sp; } regs;
	
		byte nbytes;
		byte opbytes[3u];    
	
		qword cpuclock;
		qword ppuclock;
		qword column;
		qword scanline;
	
		bool unofficial;
		const char* instruction;
		const char* disassembly;
	};


	byte CPU_Peek(word);
	void CPU_Poke(word, byte);

	auto LOG_Get(std::size_t index) -> const LogLine&;
	auto LOG_Len() -> std::size_t;

	int self_test ();

	ProgramROM get_rom();
}