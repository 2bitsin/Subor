// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "RicohCpu.hpp"

template <BusOperation _Operation, typename _Host, typename _Value>
inline auto RicohCPU::tick (_Host&& m, word addr, _Value&& data)
{
	byte discard = 0;
	if constexpr (_Operation == kDummyPeek)
		return tick<kPeek> (m, addr, discard);
	if constexpr (_Operation == kDummyPoke)
		return tick<kPoke> (m, addr, data);

	++q.time;
	if constexpr (_Operation == kPoke)
	{
		if (addr == 0x4014u)
		{
			q.rDma.h = (byte)data;
			q.mode.dmaStart = 1;
			return kSuccess;
		}
	}
	return m.tick<_Operation> (*this, addr, data);
}

template <typename _Host>
inline qword RicohCPU::runUntil (_Host&& m, i64 ticks_)
{
	while (q.time < ticks_)
		step (m);
	return q.time;
}

template <typename _Host>
inline bool RicohCPU::step (_Host&& m, std::size_t s)
{
	for (auto i = 0u; i < s; ++i)
	{
		byte next = 0u;
		bool cross = false;
		bool isBreak = false;
	_brk:
		if (q.mode.bits > 0u)
		{
			if (q.mode.nonMaskable)
			{
				q.addr.w = NonMaskableVec;
				q.mode.nonMaskable = 0;
			}
			else if (q.mode.reset)
			{
				q.addr.w = ResetVec;
				q.mode.reset = 0;
			}
			else if (q.mode.interrupt)
			{
				if (q.p.i)
					goto _irqskip;
				q.p.i = 1u;
				q.addr.w = InterruptVec;
			}
			else if (q.mode.breakInsruction)
			{
				q.addr.w = InterruptVec;
				q.mode.breakInsruction = 0;
				isBreak = true;
			}
			else if (q.mode.dmaStart)
			{
				tick<kDummyPeek> (m, q.rDma.w, next);
				if (q.time & 1u)
					tick<kDummyPeek> (m, q.rDma.w, next);
				q.rDma.l = 0x0u;
				q.mode.dmaStart = 0;
				q.mode.dmaCycle = 1;
				continue;
			}
			else if (q.mode.dmaCycle)
			{
				tick<kPeek> (m, q.rDma.w, next);
				tick<kPoke> (m, 0x2004u, next);
				if (!++q.rDma.l)
					q.mode.dmaCycle = 0;
				continue;
			}
			else
			{
				assert (q.mode.bits == 0u);
			}

			tick<kDummyPeek> (m, q.pc.w, next);
			tick<kDummyPeek> (m, q.pc.w, next);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.h);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.l);
			tick<kPoke> (m, 0x100 + q.s--, q.p.bits);
			q.p.b = isBreak;
			tick<kPeek> (m, q.addr.w, q.pc.l);
			tick<kPeek> (m, q.addr.w + 1u, q.pc.h);
			continue;
		}
	_irqskip:
		tick<kPeek> (m, q.pc.w++, next);
		switch (next)
		{
			// BRK
		case 0x00:
			q.mode.breakInsruction = 1;
			goto _brk;

			// Implied
		case 0x40:
		case 0x60:
		case 0x08:
		case 0x18:
		case 0x28:
		case 0x38:
		case 0x48:
		case 0x58:
		case 0x68:
		case 0x78:
		case 0x88:
		case 0x98:
		case 0xA8:
		case 0xB8:
		case 0xC8:
		case 0xD8:
		case 0xE8:
		case 0xF8:
		case 0x0A:
		case 0x1A:
		case 0x2A:
		case 0x3A:
		case 0x4A:
		case 0x5A:
		case 0x6A:
		case 0x7A:
		case 0x8A:
		case 0x9A:
		case 0xAA:
		case 0xBA:
		case 0xCA:
		case 0xDA:
		case 0xEA:
		case 0xFA:
			tick<kPeek> (m, q.pc.w, q.tmp0.l);
			break;

			// Immediate
		case 0x09:
		case 0x29:
		case 0x49:
		case 0x69:
		case 0x80:
		case 0xC0:
		case 0xC9:
		case 0xA0:
		case 0xA2:
		case 0xA9:
		case 0xE0:
		case 0xE9:
		case 0xEB:
			q.addr.w = q.pc.w++;
			break;

			// Zero Page
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0x87:
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xA7:
		case 0xC4:
		case 0xC5:
		case 0xC6:
		case 0xC7:
		case 0xE4:
		case 0xE5:
		case 0xE6:
		case 0xE7:
			tick<kPeek> (m, q.pc.w++, q.addr.w);
			break;

			// Zero Page, X
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x54:
		case 0x55:
		case 0x56:
		case 0x57:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x94:
		case 0x95:
		case 0xB4:
		case 0xB5:
		case 0xD4:
		case 0xD5:
		case 0xD6:
		case 0xD7:
		case 0xF4:
		case 0xF5:
		case 0xF6:
		case 0xF7:
			tick<kPeek> (m, q.pc.w++, q.addr.w);
			q.addr.l += q.x;
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			break;

			// Zero Page, Y
		case 0x96:
		case 0x97:
		case 0xB7:
		case 0xB6:
			tick<kPeek> (m, q.pc.w++, q.addr.w);
			q.addr.l += q.y;
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			break;

			// Absolute
		case 0x0C:
		case 0x0D:
		case 0x0E:
		case 0x0F:
		case 0x20:
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x2F:
		case 0x4C:
		case 0x4D:
		case 0x4E:
		case 0x4F:
		case 0x6D:
		case 0x6E:
		case 0x6F:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0x8F:
		case 0xAC:
		case 0xAD:
		case 0xAE:
		case 0xAF:
		case 0xCC:
		case 0xCD:
		case 0xCE:
		case 0xCF:
		case 0xEC:
		case 0xED:
		case 0xEE:
		case 0xEF:
			tick<kPeek> (m, q.pc.w++, q.addr.l);
			tick<kPeek> (m, q.pc.w++, q.addr.h);
			break;

			// Absolute, X
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
		case 0x3C:
		case 0x3D:
		case 0x3E:
		case 0x3F:
		case 0x5C:
		case 0x5D:
		case 0x5E:
		case 0x5F:
		case 0x7C:
		case 0x7D:
		case 0x7E:
		case 0x7F:
		case 0x9D:
		case 0xBC:
		case 0xBD:
		case 0xDC:
		case 0xDD:
		case 0xDE:
		case 0xDF:
		case 0xFC:
		case 0xFD:
		case 0xFE:
		case 0xFF:
			tick<kPeek> (m, q.pc.w++, q.tmp0.l);
			tick<kPeek> (m, q.pc.w++, q.tmp0.h);
			q.addr.w = q.tmp0.w;
			q.addr.w += q.x;
			q.tmp0.l += q.x;
			cross = q.addr.h != q.tmp0.h;
			break;

			// Absolute, Y
		case 0x19:
		case 0x1B:
		case 0x39:
		case 0x3B:
		case 0x59:
		case 0x5B:
		case 0x79:
		case 0x7B:
		case 0x99:
		case 0xB9:
		case 0xBE:
		case 0xBF:
		case 0xD9:
		case 0xDB:
		case 0xF9:
		case 0xFB:
			tick<kPeek> (m, q.pc.w++, q.tmp0.l);
			tick<kPeek> (m, q.pc.w++, q.tmp0.h);
			q.addr.w = q.tmp0.w;
			q.addr.w += q.y;
			q.tmp0.l += q.y;
			cross = q.addr.h != q.tmp0.h;
			break;

			// (Indirect)
		case 0x6C:
			tick<kPeek> (m, q.pc.w++, q.tmp0.l);
			tick<kPeek> (m, q.pc.w++, q.tmp0.h);
			tick<kPeek> (m, q.tmp0.w, q.addr.l);
			tick<kPeek> (m, (++q.tmp0.l, q.tmp0.w), q.addr.h);
			break;

			// (Indirect, X)
		case 0x01:
		case 0x03:
		case 0x21:
		case 0x23:
		case 0x41:
		case 0x43:
		case 0x61:
		case 0x63:
		case 0x81:
		case 0x83:
		case 0xA1:
		case 0xA3:
		case 0xC1:
		case 0xC3:
		case 0xE1:
		case 0xE3:
			tick<kPeek> (m, q.pc.w++, q.addr.w);
			tick<kPeek> (m, q.addr.w += q.x, q.tmp0.l);
			tick<kPeek> (m, q.addr.l++, q.tmp0.l);
			tick<kPeek> (m, q.addr.l, q.tmp0.h);
			q.addr.w = q.tmp0.w;
			break;

			// (Indirect), Y
		case 0x11:
		case 0x13:
		case 0x31:
		case 0x33:
		case 0x51:
		case 0x53:
		case 0x71:
		case 0x73:
		case 0x91:
		case 0xB1:
		case 0xB3:
		case 0xD1:
		case 0xD3:
		case 0xF1:
		case 0xF3:
			tick<kPeek> (m, q.pc.w++, q.addr.w);
			tick<kPeek> (m, q.addr.l, q.tmp0.l);
			tick<kPeek> (m, ++q.addr.l, q.tmp0.h);
			q.addr.w = q.tmp0.w;
			q.addr.l += q.y;
			q.tmp0.w += q.y;
			cross = q.addr.h != q.tmp0.h;
			q.addr.w = q.tmp0.w;
			break;

			// Relative
		case 0x10:
		case 0x30:
		case 0x50:
		case 0x70:
		case 0x90:
		case 0xB0:
		case 0xD0:
		case 0xF0:
			tick<kPeek> (m, q.pc.w++, q.tmp0.w);
			q.addr.w = q.pc.w + q.tmp0.w;
			if (q.tmp0.w >= 0x80)
				q.addr.w -= 0x100;
			break;

		default:
			assert (false);
			break;
		}

		switch (next)
		{
			// JMP
		case 0x4C:
		case 0x6C:
			q.pc = q.addr;
			break;

			// JSR
		case 0x20: // Todo: double-check the order of stores/loads
			--q.pc.w;
			tick<kPoke> (m, 0x100 + q.s, q.addr.l);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.h);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.l);
			q.pc.w = q.addr.w;
			break;

			// RTS
		case 0x60:
			tick<kDummyPeek> (m, 0x100 + q.s, q.tmp0.l);
			tick<kPeek> (m, 0x100 + ++q.s, q.pc.l);
			tick<kPeek> (m, 0x100 + ++q.s, q.pc.h);
			tick<kDummyPeek> (m, q.pc.w++, q.tmp0.l);
			break;

			// RTI
		case 0x40:
			tick<kPeek> (m, 0x100 + ++q.s, q.p.bits);
			q.p.b = 0;
			q.p.e = 1;
			tick<kPeek> (m, 0x100 + ++q.s, q.pc.l);
			tick<kPeek> (m, 0x100 + ++q.s, q.pc.h);
			tick<kDummyPeek> (m, q.pc.w, q.tmp0.l);
			break;

			// LDA		
		case 0xA1:
		case 0xA5:
		case 0xA9:
		case 0xAD:
		case 0xB1:
		case 0xB5:
		case 0xB9:
		case 0xBD:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.tmp0.l == 0u);
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.a = q.tmp0.l;
			break;

			// LDX
		case 0xB6:
		case 0xA2:
		case 0xA6:
		case 0xAE:
		case 0xBE:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.tmp0.l == 0u);
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.x = q.tmp0.l;
			break;

			// LAX
		case 0xA7: // LAX ab            ;*=add 1     3
		case 0xB7: // LAX ab,Y          ;if page     4
		case 0xAF: // LAX abcd          ;No. Cycles= 4
		case 0xBF: // LAX abcd,Y        ;            4*
		case 0xA3: // LAX (ab,X)        ;boundary    6
		case 0xB3: // LAX (ab),Y        ;is crossed  5*
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.tmp0.l == 0u);
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.x = q.a = q.tmp0.l;
			break;

			// LDY
		case 0xB4:
		case 0xA0:
		case 0xA4:
		case 0xAC:
		case 0xBC:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.tmp0.l == 0u);
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.y = q.tmp0.l;
			break;


			// STX
		case 0x96:
		case 0x86:
		case 0x8E:
			tick<kPoke> (m, q.addr.w, q.x);
			break;

			// STY
		case 0x94:
		case 0x84:
		case 0x8C:
			tick<kPoke> (m, q.addr.w, q.y);
			break;

			// STA
		case 0x91:
		case 0x99:
		case 0x9D:
			tick<kDummyPeek> (m, q.addr.w, q.a);
		case 0x81:
		case 0x85:
		case 0x8D:
		case 0x95:
			tick<kPoke> (m, q.addr.w, q.a);
			break;

			// BIT
		case 0x24:
		case 0x2C:
			tick<kPeek> (m, q.addr.w, q.tmp0.w);
			q.p.z = !(q.tmp0.l & q.a);
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.p.v = !!(q.tmp0.l & 0x40u);
			break;

			// AND
		case 0x21:
		case 0x25:
		case 0x29:
		case 0x2D:
		case 0x31:
		case 0x35:
		case 0x39:
		case 0x3D:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.l = (q.a &= q.tmp0.l);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// CMP
		case 0xC1:
		case 0xC5:
		case 0xC9:
		case 0xCD:
		case 0xD1:
		case 0xD5:
		case 0xD9:
		case 0xDD:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.w);
			q.p.z = !!(q.a == q.tmp0.l);
			q.p.n = !!((q.a - q.tmp0.l) & 0x80u);
			q.p.c = !!(q.tmp0.l <= q.a);
			break;

			// CPY
		case 0xC0:
		case 0xC4:
		case 0xCC:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.y == q.tmp0.l);
			q.p.n = !!((q.y - q.tmp0.l) & 0x80u);
			q.p.c = !!(q.tmp0.l <= q.y);
			break;

			// CPX
		case 0xE0:
		case 0xE4:
		case 0xEC:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.z = !!(q.x == q.tmp0.l);
			q.p.n = !!((q.x - q.tmp0.l) & 0x80u);
			q.p.c = !!(q.tmp0.l <= q.x);
			break;


			// ORA
		case 0x01:
		case 0x05:
		case 0x09:
		case 0x0D:
		case 0x11:
		case 0x15:
		case 0x19:
		case 0x1D:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.l = (q.a |= q.tmp0.l);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// EOR
		case 0x49:
		case 0x45:
		case 0x55:
		case 0x4D:
		case 0x5D:
		case 0x59:
		case 0x41:
		case 0x51:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.l = (q.a ^= q.tmp0.l);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// ADC
		case 0x69:
		case 0x65:
		case 0x75:
		case 0x6D:
		case 0x7D:
		case 0x79:
		case 0x61:
		case 0x71:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.w);
			q.tmp1.w = q.tmp0.w + !!(q.p.c) + q.a;
			q.p.z = !q.tmp1.l;
			q.p.n = !!(q.tmp1.l & 0x80);
			q.p.c = !!q.tmp1.h;
			q.p.v = !!((~(q.a ^ q.tmp0.l) & (q.a ^ q.tmp1.l)) >> 7u);
			q.a = q.tmp1.l;
			break;

			// SBC
		case 0xE1:
		case 0xE5:
		case 0xE9:
		case 0xEB:
		case 0xED:
		case 0xF1:
		case 0xF5:
		case 0xF9:
		case 0xFD:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.w);
			q.tmp1.w = q.a - q.tmp0.w - !q.p.c;
			q.p.z = !q.tmp1.l;
			q.p.n = !!(q.tmp1.l & 0x80);
			q.p.c = !q.tmp1.h;
			q.p.v = !!(((q.a ^ q.tmp0.l) & (q.a ^ q.tmp1.l)) >> 7u);
			q.a = q.tmp1.l;
			break;

			// SEC
		case 0x38:
			q.p.c = 1;
			break;

			// SED
		case 0xF8:
			q.p.d = 1;
			break;

			// SEI
		case 0x78:
			q.p.i = 1;
			break;

			// CLC
		case 0x18:
			q.p.c = 0;
			break;

			// CLD
		case 0xD8:
			q.p.d = 0;
			break;

			// CLI
		case 0x58:
			q.p.i = 0;
			break;

			// CLV
		case 0xB8:
			q.p.v = 0;
			break;

			// DEX
		case 0xCA:
			q.tmp0.w = --q.x;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// DEY
		case 0x88:
			q.tmp0.w = --q.y;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// INX
		case 0xE8:
			q.tmp0.w = ++q.x;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// INY
		case 0xC8:
			q.tmp0.w = ++q.y;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// ASL A
		case 0x0A:
			(q.tmp0.w = q.a) <<= 1u;
			q.a = q.tmp0.l;
			q.p.c = !!(q.tmp0.w & 0x100u);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// LSR A
		case 0x4A:
			q.p.c = !!(q.a & 0x1u);
			q.tmp0.l = (q.a >>= 1u);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// ROR A
		case 0x6A:
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.a & 0x1u);
			q.tmp0.l = (q.a = (q.a >> 1u) | (q.tmp0.h << 7u));
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// ROL A
		case 0x2A:
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.a & 0x80u);
			q.tmp0.l = (q.a = (q.a << 1u) | q.tmp0.h);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TAX
		case 0xAA:
			q.tmp0.w = q.x = q.a;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TAY
		case 0xA8:
			q.tmp0.w = q.y = q.a;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TXA
		case 0x8A:
			q.tmp0.w = q.a = q.x;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TYA
		case 0x98:
			q.tmp0.w = q.a = q.y;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TSX
		case 0xBA:
			q.tmp0.w = q.x = q.s;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// TXS
		case 0x9A:
			q.tmp0.w = q.s = q.x;
			break;

			// BCS
		case 0xB0:
			if (!q.p.c)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BCC
		case 0x90:
			if (q.p.c)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BEQ
		case 0xF0:
			if (!q.p.z)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BNE
		case 0xD0:
			if (q.p.z)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BVS
		case 0x70:
			if (!q.p.v)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BVC
		case 0x50:
			if (q.p.v)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BMI
		case 0x30:
			if (!q.p.n)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// BPL
		case 0x10:
			if (q.p.n)
				break;
			if (q.pc.h != q.addr.h)
				tick<kDummyPeek> (m, (q.pc.l = q.addr.l, q.pc.w), q.tmp0.l);
			tick<kPeek> (m, q.pc.w = q.addr.w, q.tmp0.l);
			break;

			// PHP
		case 0x08:
			q.tmp0.l = q.p.bits | BreakFlag;
			tick<kPoke> (m, 0x100 + q.s--, q.tmp0.l);
			break;

			// PHA
		case 0x48:
			q.tmp0.l = q.a;
			tick<kPoke> (m, 0x100 + q.s--, q.tmp0.l);
			break;

			// PLP
		case 0x28:
			tick<kDummyPeek> (m, 0x100 + q.s, q.tmp0.l);
			tick<kPeek> (m, 0x100 + ++q.s, q.tmp0.l);
			q.p.bits = q.tmp0.l;
			q.p.b = 0;
			q.p.e = 1;
			break;

			// PLA
		case 0x68:
			tick<kDummyPeek> (m, 0x100 + q.s, q.tmp0.l);
			tick<kPeek> (m, 0x100 + ++q.s, q.tmp0.l);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			q.a = q.tmp0.l;
			break;

			// ASL
		case 0x1E:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x06:
		case 0x16:
		case 0x0E:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			q.p.c = !!(q.tmp0.l & 0x80);
			q.tmp0.l <<= 1u;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// LSR
		case 0x5E:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x46:
		case 0x56:
		case 0x4E:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			q.p.c = !!(q.tmp0.l & 0x1u);
			q.tmp0.l >>= 1u;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// ROL
		case 0x3E:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x26:
		case 0x36:
		case 0x2E:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.tmp0.l & 0x80);
			q.tmp0.l = (q.tmp0.l << 1u) | q.tmp0.h;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// ROR
		case 0x7E:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x66:
		case 0x76:
		case 0x6E:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.tmp0.l & 0x1u);
			q.tmp0.l = (q.tmp0.l >> 1u) | (q.tmp0.h << 7u);
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// INC
		case 0xFE:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0xE6:
		case 0xF6:
		case 0xEE:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			++q.tmp0.l;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// DEC
		case 0xDE:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0xC6:
		case 0xD6:
		case 0xCE:
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l);
			--q.tmp0.l;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// DCP/DCM
		case 0xC7: // DCM ab                      5
		case 0xD7: // DCM ab,X                    6
		case 0xCF: // DCM abcd        No. Cycles= 6
		case 0xDF: // DCM abcd,X                  7
		case 0xDB: // DCM abcd,Y                  7
		case 0xC3: // DCM (ab,X)                  8
		case 0xD3: // DCM (ab),Y                  8
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l--);
			q.p.z = !!(q.a == q.tmp0.l);
			q.p.n = !!((q.a - q.tmp0.l) & 0x80u);
			q.p.c = !!(q.tmp0.l <= q.a);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			break;

			// AXS
		case 0x87: 	//AXS ab          ;ab       ;            3
		case 0x97: 	//AXS ab,Y        ;ab       ;            4
		case 0x8F: 	//AXS abcd        ;cd ab    ;No. Cycles= 4
		case 0x83: 	//AXS (ab,X)      ;ab       ;            6
			tick<kPoke> (m, q.addr.w, q.a & q.x);
			break;

			// ISB/INS/ISC
		case 0xE7: // INS ab                   5
		case 0xF7: // INS ab,X                 6
		case 0xEF: // INS abcd     No. Cycles= 6
		case 0xFF: // INS abcd,X               7
		case 0xFB: // INS abcd,Y               7
		case 0xE3: // INS (ab,X)               8
		case 0xF3: // INS (ab),Y               8
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			tick<kPeek> (m, q.addr.w, q.tmp0.w);
			tick<kDummyPoke> (m, q.addr.w, q.tmp0.l++);
			tick<kPoke> (m, q.addr.w, q.tmp0.w);
			q.tmp1.w = q.a - q.tmp0.w - !q.p.c;
			q.p.z = !q.tmp1.l;
			q.p.n = !!(q.tmp1.l & 0x80);
			q.p.c = !q.tmp1.h;
			q.p.v = !!(((q.a ^ q.tmp0.l) & (q.a ^ q.tmp1.l)) >> 7u);
			q.a = q.tmp1.l;
			break;

			// ASO/SLO
		case 0x1F: // ASO abcd,X               7
		case 0x1B: // ASO abcd,Y               7
		case 0x13: // ASO (ab),Y               8
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x07: // ASO ab                   5
		case 0x17: // ASO ab,X                 6
		case 0x0F: // ASO abcd     No. Cycles= 6
		case 0x03: // ASO (ab,X)               8
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.c = !!(q.tmp0.l & 0x80);
			q.tmp0.l <<= 1u;
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l = (q.a |= q.tmp0.l));
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

			// RLA
		case 0x33: // RLA (ab),Y               8
		case 0x3B: // RLA abcd,Y               7
		case 0x3F: // RLA abcd,X               7
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x27: // RLA ab                   5
		case 0x37: // RLA ab,X                 6
		case 0x2F: // RLA abcd     No. Cycles= 6
		case 0x23: // RLA (ab,X)               8
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.tmp0.l & 0x80);
			q.tmp0.l = (q.tmp0.l << 1u) | q.tmp0.h;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPeek> (m, q.addr.w, q.a &= q.tmp0.l);
			break;

			// LSE/SRE
		case 0x5F: // LSE abcd,X              7
		case 0x5B: // LSE abcd,Y              7
		case 0x53: // LSE (ab),Y              8
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x47: // LSE ab                  5
		case 0x57: // LSE ab,X                6
		case 0x4F: // LSE abcd    No. Cycles= 6
		case 0x43: // LSE (ab,X)              8
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.p.c = !!(q.tmp0.l & 0x1u);
			q.tmp0.l >>= 1u;
			tick<kPoke> (m, q.addr.w, q.tmp0.l);
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l = (q.a ^= q.tmp0.l));
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80u);
			break;

		case 0x7B: // RRA abcd,Y              7
		case 0x7F: // RRA abcd,X              7
		case 0x73: // RRA (ab),Y              8
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
		case 0x67: // RRA ab                  5
		case 0x77: // RRA ab,X                6
		case 0x6F: // RRA abcd    No. Cycles= 6
		case 0x63: // RRA (ab,X)              8
			tick<kPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.h = !!q.p.c;
			q.p.c = !!(q.tmp0.l & 0x1u);
			q.tmp0.l = (q.tmp0.l >> 1u) | (q.tmp0.h << 7u);
			tick<kPoke> (m, q.addr.w, q.tmp0.w &= 0xff);
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
			q.tmp1.w = q.tmp0.w + (!!q.p.c) + q.a;
			q.p.z = !q.tmp1.l;
			q.p.n = !!(q.tmp1.l & 0x80);
			q.p.c = !!q.tmp1.h;
			q.p.v = !!((~(q.a ^ q.tmp0.l) & (q.a ^ q.tmp1.l)) >> 7u);
			q.a = q.tmp1.l;
			break;

			// NOP
		case 0x1C:
		case 0x3C:
		case 0x5C:
		case 0x7C:
		case 0xDC:
		case 0xFC:
			if (cross == true)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
		case 0x0C:
		case 0x04:
		case 0x14:
		case 0x34:
		case 0x44:
		case 0x54:
		case 0x64:
		case 0x74:
		case 0x80:
		case 0xD4:
		case 0xF4:
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.w);
		case 0x1A:
		case 0x3A:
		case 0x5A:
		case 0x7A:
		case 0xDA:
		case 0xEA:
		case 0xFA:
			break;

		default:
			assert (false);
			break;
		}
	}
	return true;
}

template <ResetType _Type>
void RicohCPU::reset ()
{

	if constexpr (_Type == kSoftReset)
		setSignal (ResetBit);

	if constexpr (_Type == kHardReset)
		q = State{ };
}

inline void RicohCPU::setSignal(byte bits)
{
	if (bits & ResetBit)
		q.mode.reset = 1u;
	if (bits & NonMaskableBit)
		q.mode.nonMaskable = 1u;
	if (bits & InterruptBit)
		q.mode.interrupt = 1u;
}

inline void RicohCPU::clrSignal (byte bits)
{
	if (bits & ResetBit)
		q.mode.reset = 0u;
	if (bits & NonMaskableBit)
		q.mode.nonMaskable = 0u;
	if (bits & InterruptBit)
		q.mode.interrupt = 0u;
}

inline RicohCPU::RicohCPU (State state)
	: q (std::move (state))
{}
