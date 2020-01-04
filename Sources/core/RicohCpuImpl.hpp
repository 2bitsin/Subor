#include "RicohCpu.hpp"

template <auto ... _Test, typename _Value>
constexpr bool is_in (_Value&& value)
{
	return (... || (value == _Test));
}

template <BusOperation _Operation, typename _Host, typename _Value>
inline auto RicohCPU::tick (_Host&& m, word addr, _Value&& data)
{

	byte discard = 0;
	if constexpr (_Operation == kDummyPeek)
		return tick<kPeek> (m, addr, discard);
	if constexpr (_Operation == kDummyPoke)
		return tick<kPoke> (m, addr, data);

	++q.cnt_clock;
	if constexpr (_Operation == kPoke)
	{
		if (addr == 0x4014u)
		{
			q.rDma.h = (byte)data;
			q.mode.dmaStart = 1;
			return kSuccess;
		}
	}
	return	m.template tick<_Operation> (*this, addr, data);
}

template <typename _Host, typename _ShouldStop>
inline bool RicohCPU::stepUntil (_Host&& m, _ShouldStop&& s)
{
	byte discard;
	while (!s (*this, m))
	{
		word next = 0u;
		bool cross = false;

		if (q.mode.stall)
			continue;
		else if (q.mode.dmaStart)
		{
			tick<kDummyPeek> (m, q.rDma.w, next);
			if (q.cnt_clock & 1u)
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
		else if (std::exchange (q.mode.nmi, 0u))
			next = 0x101u;
		else if (std::exchange (q.mode.rst, 0u))
			next = 0x102u;
		else if (q.mode.irq && !q.p.i)
			next = 0x100u;
		else
		{
			if (q.mode.wait)
				continue;
			tick<kPeek> (m, q.pc.w++, next);
		}

		if (is_in<0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x41, 0x43, 0x44, 0x45, 0x46, 0x47, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x53, 0x54, 0x55, 0x56, 0x57, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x61, 0x63, 0x64, 0x65, 0x66, 0x67, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x73, 0x74, 0x75, 0x76, 0x77, 0x79, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x81, 0x83, 0x84, 0x85, 0x86, 0x87, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x93, 0x94, 0x95, 0x96, 0x97, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA1, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC1, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD9, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE1, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF9, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF>(next))
			tick<kPeek> (m, q.pc.w++, q.addr.w);

		if (is_in<0x96, 0x97, 0xB7, 0xB6> (next))
			q.addr.l += q.y;

		if (is_in<0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37, 0x54, 0x55, 0x56, 0x57, 0x74, 0x75, 0x76, 0x77, 0x94, 0x95, 0xB4, 0xB5, 0xD4, 0xD5, 0xD6, 0xD7, 0xF4, 0xF5, 0xF6, 0xF7> (next))
			q.addr.l += q.x;

		if (is_in<0x2, 0x8, 0x9, 0xA, 0xB, 0x12, 0x18, 0x1A, 0x22, 0x28, 0x29, 0x2A, 0x2B, 0x32, 0x38, 0x3A, 0x40, 0x42, 0x48, 0x49, 0x4A, 0x4B, 0x52, 0x58, 0x5A, 0x60, 0x62, 0x68, 0x69, 0x6A, 0x6B, 0x72, 0x78, 0x7A, 0x80, 0x82, 0x88, 0x89, 0x8A, 0x8B, 0x92, 0x98, 0x9A, 0xA0, 0xA2, 0xA8, 0xA9, 0xAA, 0xAB, 0xB2, 0xB8, 0xBA, 0xC0, 0xC2, 0xC8, 0xC9, 0xCA, 0xCB, 0xD2, 0xD8, 0xDA, 0xE0, 0xE2, 0xE8, 0xE9, 0xEA, 0xEB, 0xF2, 0xF8, 0xFA> (next))
			q.addr.w = q.pc.w;
		
		if (is_in<0x01, 0x03, 0x21, 0x23, 0x41, 0x43, 0x61, 0x63, 0x81, 0x83, 0xA1, 0xA3, 0xC1, 0xC3, 0xE1, 0xE3> (next))
			q.addr.w += q.x;

		if (is_in<0x96, 0x97, 0xB7, 0xB6, 0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37, 0x54, 0x55, 0x56, 0x57, 0x74, 0x75, 0x76, 0x77, 0x94, 0x95, 0xB4, 0xB5, 0xD4, 0xD5, 0xD6, 0xD7, 0xF4, 0xF5, 0xF6, 0xF7, 0x40, 0x60, 0x08, 0x18, 0x28, 0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xA8, 0xB8, 0xC8, 0xD8, 0xE8, 0xF8, 0x0A, 0x1A, 0x2A, 0x3A, 0x4A, 0x5A, 0x6A, 0x7A, 0x8A, 0x9A, 0xAA, 0xBA, 0xCA, 0xDA, 0xEA, 0xFA, 0x01, 0x03, 0x21, 0x23, 0x41, 0x43, 0x61, 0x63, 0x81, 0x83, 0xA1, 0xA3, 0xC1, 0xC3, 0xE1, 0xE3>(next))
			tick<kDummyPeek> (m, q.addr.w, discard);

		if (is_in<0x0C, 0x0D, 0x0E, 0x0F, 0x20, 0x2C, 0x2D, 0x2E, 0x2F, 0x4C, 0x4D, 0x4E, 0x4F, 0x6D, 0x6E, 0x6F, 0x8C, 0x8D, 0x8E, 0x8F, 0xAC, 0xAD, 0xAE, 0xAF, 0xCC, 0xCD, 0xCE, 0xCF, 0xEC, 0xED, 0xEE, 0xEF, 0x19, 0x1B, 0x39, 0x3B, 0x59, 0x5B, 0x79, 0x7B, 0x99, 0x9B, 0x9E, 0x9F, 0xB9, 0xBB, 0xBE, 0xBF, 0xD9, 0xDB, 0xF9, 0xFB, 0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F, 0x5C, 0x5D, 0x5E, 0x5F, 0x7C, 0x7D, 0x7E, 0x7F, 0x9C, 0x9D, 0xBC, 0xBD, 0xDC, 0xDD, 0xDE, 0xDF, 0xFC, 0xFD, 0xFE, 0xFF, 0x6C> (next))
			tick<kPeek> (m, q.pc.w, q.addr.h);

		if (is_in<0x2, 0x9, 0xB, 0xC, 0xD, 0xE, 0xF, 0x12, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x29, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x32, 0x39, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x42, 0x49, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x52, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x62, 0x69, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x72, 0x79, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x82, 0x89, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x92, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA2, 0xA9, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC2, 0xC9, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD2, 0xD9, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE2, 0xE9, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF2, 0xF9, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF> (next))
			++q.pc.w;

		if (is_in<0x10, 0x30, 0x50, 0x70, 0x90, 0xB0, 0xD0, 0xF0> (next))
			q.addr.w += q.pc.w - ((q.addr.w & 0x80) << 1u);

		if (is_in<0x01, 0x03, 0x21, 0x23, 0x41, 0x43, 0x61, 0x63, 0x6C, 0x81, 0x83, 0xA1, 0xA3, 0xC1, 0xC3, 0xE1, 0xE3, 0x11, 0x13, 0x31, 0x33, 0x51, 0x53, 0x71, 0x73, 0x91, 0x93, 0xB1, 0xB3, 0xD1, 0xD3, 0xF1, 0xF3> (next))
		{
			next == 0x6C ? tick<kPeek> (m, q.addr.w, q.tmp0.l) : tick<kPeek> (m, q.addr.l, q.tmp0.l);
			q.addr.l += 1u;
			next == 0x6C ? tick<kPeek> (m, q.addr.w, q.tmp0.h) : tick<kPeek> (m, q.addr.l, q.tmp0.h);
			q.addr.w = q.tmp0.w;
		}

		if (is_in<0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x39, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x79, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xD9, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xF9, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF>(next))
			q.tmp0.w = q.addr.w;

		if (is_in<0x11, 0x13, 0x19, 0x1B, 0x31, 0x33, 0x39, 0x3B, 0x51, 0x53, 0x59, 0x5B, 0x71, 0x73, 0x79, 0x7B, 0x91, 0x93, 0x99, 0x9B, 0x9E, 0x9F, 0xB1, 0xB3, 0xB9, 0xBB, 0xBE, 0xBF, 0xD1, 0xD3, 0xD9, 0xDB, 0xF1, 0xF3, 0xF9, 0xFB> (next))
			q.addr.w += q.y;

		if (is_in<0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F, 0x5C, 0x5D, 0x5E, 0x5F, 0x7C, 0x7D, 0x7E, 0x7F, 0x9C, 0x9D, 0xBC, 0xBD, 0xDC, 0xDD, 0xDE, 0xDF, 0xFC, 0xFD, 0xFE, 0xFF> (next))
			q.addr.w += q.x;

		if (is_in<0x11, 0x13, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x31, 0x33, 0x39, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x51, 0x53, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x71, 0x73, 0x79, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x91, 0x93, 0x99, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xB1, 0xB3, 0xB9, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xD1, 0xD3, 0xD9, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xF1, 0xF3, 0xF9, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF>(next))
			cross = q.addr.h != q.tmp0.h;			
		
		switch (next)
		{
		case 0x000:// BRK
		case 0x100:// IRQ			
		case 0x101:// NMI			
		case 0x102:// RST			
			q.p.b = (next == 0x000u);
			q.p.i = (next == 0x100u);
			q.addr.w = vectors [next & 0xFF];
			tick<kDummyPeek> (m, q.pc.w, next);
			tick<kDummyPeek> (m, q.pc.w, next);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.h);
			tick<kPoke> (m, 0x100 + q.s--, q.pc.l);
			tick<kPoke> (m, 0x100 + q.s--, q.p.bits);
			tick<kPeek> (m, q.addr.w, q.pc.l);
			tick<kPeek> (m, q.addr.w + 1u, q.pc.h);
			break;
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

		case 0xBB:
			if (cross)
				tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
			q.tmp0.l &= q.s;
			q.a = q.tmp0.l;
			q.x = q.tmp0.l;
			q.s = q.tmp0.l;
			q.p.z = !q.tmp0.l;
			q.p.n = !!(q.tmp0.l & 0x80);
			tick<kDummyPeek> (m, q.addr.w, q.tmp0.l);
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
		new (&q) State {};
}

inline void RicohCPU::setSignal (byte bits)
{
	if (bits & ResetBit)
		q.mode.rst = 1u;
	if (bits & NonMaskableBit)
		q.mode.nmi = 1u;
	if (bits & InterruptBit)
		q.mode.irq = 1u;
}

inline void RicohCPU::clrSignal (byte bits)
{
	if (bits & ResetBit)
		q.mode.rst = 0u;
	if (bits & NonMaskableBit)
		q.mode.nmi = 0u;
	if (bits & InterruptBit)
		q.mode.irq = 0u;
}

inline RicohCPU::RicohCPU (State state)
	: q (std::move (state))
{}
