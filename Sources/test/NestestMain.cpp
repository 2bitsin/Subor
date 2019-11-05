// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "test/Nestest.hpp"
#include "core/RicohCpu.hpp"
#include <string>
#include <iostream>


template <typename Q, typename U>
static inline void AssertState (Q&& statelog, U&& state)
{
	bool isok;
	isok=(statelog.regs.a		== state.a)
		&& (statelog.regs.x	  == state.x)
		&& (statelog.regs.y   == state.y)
		&& (statelog.regs.p	  == state.p.bits)
		&& (statelog.regs.sp  == state.s)
		&& (statelog.addr			== state.pc.w)
		&& (statelog.cpuclock == state.time)
	;
	if (isok != true)
	{
		printf ("\nEXPECTATION : PC = %04X, A = %02X, X = %02X, Y = %02X, P = %02X, S = %02X, T = %lld"
			,statelog.addr	  
			,statelog.regs.a	  
			,statelog.regs.x	  
			,statelog.regs.y   
			,statelog.regs.p	  
			,statelog.regs.sp  
			,statelog.cpuclock 
		);
		printf ("\n    REALITY : PC = %04X, A = %02X, X = %02X, Y = %02X, P = %02X, S = %02X, T = %lld"
			,state.pc.w
			,state.a
			,state.x  
			,state.y  
			,state.p.bits
			,state.s
			,state.time
		);
		throw std::runtime_error("Failed nestest");
	}
}

struct Test
{
	template <BusOperation _Operation, typename _Slave, typename _Addr, typename _Value>
	auto tick (_Slave&& slave, _Addr addr, _Value&& data)
	{
		using namespace nestest;
		switch (_Operation)
		{
		case BusOperation::kPoke:
			CPU_Poke (addr, (byte)data);
			break;
		case BusOperation::kPeek:
			data = CPU_Peek (addr);
			break;
		case BusOperation::kDummyPeek:
		default:			
			CPU_Peek (addr);
			break;
		}
		return kSuccess;
	}
};

int nestest::NestestMain ()
{
	using namespace nestest;
	auto cpu = RicohCPU (RicohCPU::kTestState);
	auto test = Test{};

	std::printf ("INIT : ");
	for (auto i = 0ull; i < LOG_Len (); ++i)
	{
		auto&& logState = LOG_Get (i);
		auto&& cpuState = cpu.state ();
		AssertState (logState, cpuState);
		std::printf ("PASS\nNEXT: (%02X) %s ... ", logState.opbytes [0], logState.instruction);
		cpu.stepUntil (test, [i = 0] (auto&&... ) mutable { return i++ > 0; });
	}
	
	return 0;
}