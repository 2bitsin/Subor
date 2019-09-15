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
:	public CoreConfig
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
		long long time{ 0 };
		Word pc{ 0xC000 };
		byte a{ 0 }, x{ 0 }, y{ 0 }, s{ 0 };
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
		p{ 0x24 };

		union
		{
			byte bits;
			Bitfield<0, 1> nonMaskable;
			Bitfield<1, 1> reset;
			Bitfield<2, 1> interrupt;
			Bitfield<3, 1> breakInsruction;
			Bitfield<4, 1> dmaStart;
			Bitfield<5, 1> dmaCycle;
		}
		mode{ 0x02u };

		Word addr{ 0 };
		Word tmp0{ 0 };
		Word tmp1{ 0 };
		Word rDma{ 0 };		
	};

	static constexpr const State kTestState = State{ 7, { 0xC000 }, 0, 0, 0, 0xfd, 0x24, 0 };

	template <BusOperation _Operation, typename _Host, typename _Value>
	auto tick (_Host&& master, word addr, _Value&& data);

	template<typename _Host>
	qword runUntil (_Host&& master, i64 ticks_);

	template <typename _Host>
	bool step (_Host&& tick_, std::size_t count_ = 1u);

	auto&& state () { return q; }
	auto&& state () const { return q; }

	void setSignal (byte bits);
	void clrSignal (byte bits);

	template <ResetType>
	void reset ();

	RicohCPU (State state = { })
	: q (std::move (state))
	{ }

private:
	State q;
};

#include "RicohCpuImpl.hpp"