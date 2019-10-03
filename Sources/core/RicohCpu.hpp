// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "core/Memory.hpp"
#include "core/CoreConfig.hpp"
#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"
#include <cstdio>
#include <cassert>
#include <functional>

struct RicohCPU
	: public CoreConfig
{
	static constexpr byte NonMaskableBit = 0x01u;
	static constexpr byte ResetBit = 0x02u;
	static constexpr byte InterruptBit = 0x04u;

	static constexpr word NonMaskableVec = 0xfffau;
	static constexpr word ResetVec = 0xfffcu;
	static constexpr word InterruptVec = 0xfffeu;

	static constexpr byte NegativeFlag = 0x80u;
	static constexpr byte OverflowFlag = 0x40u;
	static constexpr byte ExpansionFlag = 0x20u;
	static constexpr byte BreakFlag = 0x10u;
	static constexpr byte BcdFlag = 0x08u;
	static constexpr byte InterruptFlag = 0x04u;
	static constexpr byte ZeroFlag = 0x02u;
	static constexpr byte CarryFlag = 0x01u;
	static constexpr byte DefaultFlags = ExpansionFlag | InterruptFlag;

	struct State
	{
		constexpr State (
			i64 time = 0,
			word pc = 0xc000,
			byte a = 0,
			byte x = 0,
			byte y = 0,
			byte s = 0,
			byte p = 0x24u,
			byte mode = 0x2u)
		: time{time},
			pc{pc},
			a{a},
			x{x},
			y{y},
			s{s},
			p{p},
			mode{mode},
			addr{0},
			tmp0{0},
			tmp1{0},
			rDma{0}
		{}

		long long time;
		Word pc;
		byte a, x, y, s;
		union
		{
			byte bits;
			Bitfield<0, 1> c;
			Bitfield<1, 1> z;
			Bitfield<2, 1> i;
			Bitfield<3, 1> d;
			Bitfield<4, 1> b;
			Bitfield<5, 1> e;
			Bitfield<6, 1> v;
			Bitfield<7, 1> n;
		}
		p;

		union
		{
			byte bits;
			Bitfield<0, 1> nmi;
			Bitfield<1, 1> rst;
			Bitfield<2, 1> irq;
			Bitfield<3, 1> stall;
			Bitfield<4, 1> dmaStart;
			Bitfield<5, 1> dmaCycle;
		}
		mode;

		Word addr;
		Word tmp0;
		Word tmp1;
		Word rDma;
	};

	static inline const State kTestState = State{7, 0xC000, 0, 0, 0, 0xfd, 0x24, 0};

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& master, word addr, _Value&& data);

	template <typename _Host>
	bool step (_Host&& tick_, std::size_t count_ = 1u);

	auto&& state ()
	{
		return q;
	}
	auto&& state () const
	{
		return q;
	}

	void setSignal (byte bits);
	void clrSignal (byte bits);

	template <ResetType>
	void reset ();

	RicohCPU (State state);
	RicohCPU ():RicohCPU{State{}}
	{};

	bool inDmaMode () const
	{
		return q.mode.dmaCycle || q.mode.dmaStart;
	}


private:
	State q;

	static inline const constexpr word vectors [] =
	{
		InterruptVec,
		InterruptVec,
		NonMaskableVec,
		ResetVec,
	};

};

#include "RicohCpuImpl.hpp"
