// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#pragma once

#include "utils/Types.hpp"
#include "utils/Bitfield.hpp"
#include "utils/Bitarray.hpp"
#include "utils/Image.hpp"
#include "video/OAMemory.hpp"
#include "video/Palette.hpp"

#include <cstdio>

struct RicohPPU
{
#pragma pack(push, 1)
	union VideoAddress
	{
		word bits;
		Bitfield< 0, 5, 16> xCoarse;
		Bitfield< 5, 5, 16> yCoarse;
		Bitfield<10, 1, 16> hNameTable;
		Bitfield<11, 1, 16> vNameTable;
		Bitfield<12, 3, 16> yFine;
		Bitfield<15, 1, 16> nc;				// Not connected

		Bitfield<10, 2, 16> nameTable;

		Bitfield< 0, 8, 16> loByte;
		Bitfield< 8, 6, 16> hiByte;
	};

	union ControlRegister
	{
		byte bits;
		Bitfield<0, 2> nameTable;			// 0: $2000; 1: $2400; 2: $2800; 3: $2C00
		Bitfield<2, 1> inc;			// 0: add 1; 1: add 32
		Bitfield<3, 1> spriteTable;		// 0: $0000; 1: $1000; ignored in 8x16 mode
		Bitfield<4, 1> bgTable;				// 0: $0000; 1: $1000
		Bitfield<5, 1> spriteSize;		// 0: 8x8; 1: 8x16
		Bitfield<6, 1> masterSlave;		// 0: read EXT; 1: write EXT
		Bitfield<7, 1> nmiEnabled;		// 0: NMI disabled, 1: nmi enabled
	};

	union MaskRegister
	{
		byte bits;
		Bitfield<0, 1> grayscale;					// 0: color; 1: grayscale
		Bitfield<1, 1> showLeftBackground;// 0: hide; 1: show
		Bitfield<2, 1> showLeftSprites;		// 0: hide; 1: show
		Bitfield<3, 1> showBackground;		// 0: hide; 1: show
		Bitfield<4, 1> showSprites;				// 0: hide; 1: show
		Bitfield<5, 1> redTint;						// 0: normal; 1: emphasized
		Bitfield<6, 1> greenTint;					// 0: normal; 1: emphasized
		Bitfield<7, 1> blueTint;					// 0: normal; 1: emphasized
	};

	union StatusRegister
	{
		byte bits{ 0 };
		Bitfield<0, 5> openStatusBits;
		Bitfield<5, 1> spriteOverflow;
		Bitfield<6, 1> spriteZeroHit;
		Bitfield<7, 1> nmiSignaled;
	};

	union OAMAddress
	{
		byte bits{ 0 };
		Bitfield<0, 2> atrIndex;
		Bitfield<2, 6> objIndex;
	};

#pragma pack(pop)

	static constexpr const auto ctHorizontalTicks = 341u;
	static constexpr const auto ctVerticalTicks = 262u;
	static constexpr const auto ctVblankScanline = 241u;
	static constexpr const auto ctTotalTicks = ctHorizontalTicks * ctVerticalTicks;
	static constexpr const auto ctHorizontalPixels = 256u;
	static constexpr const auto ctVerticalPixels = 240u;
	static constexpr const auto ctNmiTimeout = 15u;

	Image<dword, ctHorizontalPixels, ctVerticalPixels> sPixels [2u];
	Palette sPalette{ { 0 } };
	qword sFrame = 0u;
	qword sClock = 0u;
	qword sScanline = { 0u };
	qword sDotcycle = { 0u };
	Bitshifter<64> sPreviousNmiState{ 0u };
	byte sFrameReady{ 0u };
	byte sRegisterLatch{ 0u };
	byte sNmiTimeout{ 0u };
	byte sXFine{ 0u };
	ControlRegister sCtrl{ 0 };
	StatusRegister sStat{ 0 };
	MaskRegister sMask{ 0 };
	byte sBusBuffer{ 0u };
	VideoAddress sAddress{ 0 };
	VideoAddress sScroll{ 0 };
	byte sRegBuffer{ 0u };
	byte sTileIndex{ 0u };
	word sTileAddr{ 0u };
	Bitarray<4, 16> sTileBits{ 0u }; 
	Bitarray<1, 8> sTileLatch [2u] = { { 0u }, { 0u } };
	Bitarray<2, 4> sAttrLatch{ 0u };
	byte sNextSprite{ 0u };	
	OAMAddress sOAMAddress{ 0u };
	byte sOAMLatch{ 0u };
	byte sSpriteCounter = 0u;
	Bitarray<1, 8> sSpriteZeroActive{ 0u };
	Bitarray<1, 8> sSpritePriorities;
	OAMemory<0x08u> sSecondaryOAM;
	OAMemory<0x40u> sPrimaryOAM;
	Bitarray<4, 8> sSpritePatterns [8u];
	byte sSpritePositions [8u];

	constexpr auto width () const { return ctHorizontalPixels; }
	constexpr auto height () const { return ctVerticalPixels; }
	bool ready () const { return sFrameReady; }

	template <typename _VideoSink>
	void grabFrame (_VideoSink&& write)
	{
		auto& src = sPixels [(sFrame + 1u)&1u];
		for (auto y = 0u; y < ctVerticalPixels; ++y)
			for (auto x = 0u; x < ctHorizontalPixels; ++x)
				write (x, y, src.value (x, y));
		sFrameReady = 0u;
	}

	RicohPPU ()
	{
		reset<kSoftReset> ();
	}

	template <ResetType _Type>
	void reset ()
	{
		if constexpr (_Type == ResetType::kHardReset)
		{
			this->~RigohBBU ();
			new (this) RicohPPU ();
		}
		if constexpr (_Type == ResetType::kSoftReset)
		{
			sClock = 240*341u;
			sFrame = 0u;
			sCtrl.bits = 0;
			sMask.bits = 0;
			sOAMAddress.bits = 0;
			sAddress.hNameTable = 0;
			nmiUpdate ();
		}
	}

	void nmiUpdate ()
	{
		sPreviousNmiState.left (nmiState ());
		if (sPreviousNmiState.extract (0, 2u) == 1u)
			sNmiTimeout = ctNmiTimeout;
	}

	void nmiUpdate (byte newState)
	{
		sStat.nmiSignaled = byte{ !!newState };
		nmiUpdate ();
	}

	bool nmiState () const
	{
		return sStat.nmiSignaled
			&& sCtrl.nmiEnabled;
	}

	template <typename _Host>
	void nmiTimerTick (_Host&& host)
	{
		if (sNmiTimeout > 0u)
			if (!--sNmiTimeout && nmiState ())
				host.nmi ();
	}

	template <typename _Host, typename _Value>
	auto peek (_Host&& m, word addr, _Value&& data)
	{
		return ppuTick<kPeek> (m, addr, data);
	}

	template <typename _Host, typename _Value>
	auto poke (_Host&& m, word addr, _Value&& data)
	{
		return ppuTick<kPoke> (m, addr, data);
	}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto ppuTick (_Host&& host, word addr, _Value&& data)
	{
		addr &= 0x3fff;
		if (addr >= 0x3f00u)
		{
			if constexpr (_Operation == kPeek)
				data = sPalette [addr & 0xffu];
			if constexpr (_Operation == kPoke)
				sPalette [addr & 0xffu] = data;
		}
		host.ppuTick<_Operation> (addr, data);
	}


	template <MemoryOperation _Operation, typename _Host, typename _Value>
	auto tickRegister (_Host&& host, word addr, _Value&& data)
	{
		if constexpr (_Operation == kPoke)
			sRegBuffer = (byte)data;
		if constexpr (_Operation == kPoke)
			data = sRegBuffer;
		switch (addr & 0x07u)
		{
		case 0x00u:
			if constexpr (_Operation == kPoke)
			{
				sCtrl.bits = sRegBuffer;
				sScroll.nameTable = sCtrl.nameTable;
				nmiUpdate ();
			}
			break;
		case 0x01u:
			if constexpr (_Operation == kPoke)
				sMask.bits = sRegBuffer;
			break;
		case 0x02u:
			if constexpr (_Operation == kPeek)
			{
				sStat.openStatusBits = sRegBuffer;
				data = sStat.bits;
				nmiUpdate (0u);
				sRegisterLatch = 0u;
			}
			break;
		case 0x03u:
			if constexpr (_Operation == kPoke)
				sOAMAddress.bits = sRegBuffer;
			break;
		case 0x04u:
			if constexpr (_Operation == kPoke)
				sPrimaryOAM.poke (sOAMAddress.bits++, sRegBuffer);
			if constexpr (_Operation == kPeek)
			{
				if (!(sScanline < 240u && sDotcycle >= 1u && sDotcycle <= 64u))
					data = sPrimaryOAM.peek (sOAMAddress.bits);
				else
					data = 0xffu;
			}
			break;
		case 0x05u:
			if constexpr (_Operation == kPoke)
				if (sRegisterLatch ^= 1)
					bits::unpack<3, 5> (
						sRegBuffer,
						sXFine,
						sScroll.xCoarse);
				else
					bits::unpack<3, 5> (
						sRegBuffer,
						sScroll.yFine,
						sScroll.yCoarse);
			break;
		case 0x06u:
			if constexpr (_Operation == kPoke)
			{
				if (sRegisterLatch ^= 1)
					sScroll.hiByte = sRegBuffer;
				else
				{
					sScroll.loByte = sRegBuffer;
					sAddress.bits = sScroll.bits;
				}
			}
			break;
		case 0x07u:
			if constexpr (_Operation == kPoke)
				poke (host, sAddress.bits, sRegBuffer);
			if constexpr (_Operation == kPeek)
			{
				peek (host, sAddress.bits, data);
				if ((sAddress.bits & 0x3fff) < 0x3f00)
					std::swap (sBusBuffer, (byte&)data);
				else
					peek (host, sAddress.bits - 0x1000u, sBusBuffer);
			}
			sAddress.bits += (1u << (sCtrl.inc*5u));
			break;
		}

	}

	template <MemoryOperation _Operation, typename _Host, typename _Value>
	void tick (_Host&& host, word addr, _Value&& data)
	{
		if (addr >= 0x2000u && addr < 0x4000u)
			tickRegister<_Operation> (host, addr, data);
		tickInternal (host);
		tickInternal (host);
		tickInternal (host);
	}

	void scrollUpdate (bool isFetchDotcycle, bool isPrerenderScanline)
	{
		// Scrolling logic
		if (isFetchDotcycle && !(sDotcycle & 0x7u))
		{
			if (!(++sAddress.xCoarse))
				sAddress.hNameTable ^= 1u;
		}
		if (sDotcycle == 256u)
			if (!++sAddress.yFine)
				if ((++sAddress.yCoarse) == 30u)
				{
					sAddress.yCoarse = 0u;
					sAddress.vNameTable ^= 1u;
				}
		if (sDotcycle == 257u)
		{
			sAddress.hNameTable = sScroll.hNameTable;
			sAddress.xCoarse = sScroll.xCoarse;
		}
		if (sDotcycle >= 280u && sDotcycle <= 304u && isPrerenderScanline)
		{
			sAddress.vNameTable = sScroll.vNameTable;
			sAddress.yCoarse = sScroll.yCoarse;
			sAddress.yFine = sScroll.yFine;
		}
	}


	void spriteUpdate ()
	{
		// Secondary OAM Clear	
		if (sDotcycle >= 1u && sDotcycle <= 64u && !(sDotcycle & 1u))
		{
			sSecondaryOAM.poke ((sDotcycle - 1u)/2u, 0xffu);
			sOAMAddress.bits = 0u;
			sNextSprite = 0u;
		}
		// Secondary OAM preparation
		if (sDotcycle >= 65u && sDotcycle <= 256u)
		{
			if (sDotcycle & 1u)
				sOAMLatch = sPrimaryOAM.peek (sOAMAddress.bits);
			else
			{
				sSecondaryOAM.poke (sNextSprite*4u + sOAMAddress.atrIndex, sOAMLatch);
				switch (sOAMAddress.atrIndex & 0x3u)
				{
				case 0x0u:
					{
						auto height = sCtrl.spriteSize ? 16 : 8;
						auto row = int (sScanline) - int (sOAMLatch);
						if (row >= 0 && row < height)
						{
							if (!sOAMAddress.objIndex && sDotcycle == 66u)
								sSpriteZeroActive [0] = 1u;
							if (sNextSprite == 8u)
								sStat.spriteOverflow = 1u;
							++sOAMAddress.atrIndex;
							if (sNextSprite >= 8u)
								++sOAMAddress.objIndex;
						}
						else
							++sOAMAddress.objIndex;
					}
					break;
				case 0x1u:
					++sOAMAddress.atrIndex;
					break;
				case 0x2u:
					++sOAMAddress.atrIndex;
					break;
				case 0x3u:
					++sOAMAddress.atrIndex;
					++sOAMAddress.objIndex;
					++sNextSprite;
					break;
				}
			}
		}
		if (sDotcycle == 257u)
		{
			sSpriteZeroActive.left ();
			sSpriteCounter = std::min<byte> (sNextSprite, 8u);
		}
	}

	void finalComposite ()
	{
		auto xScreen = sDotcycle - 1u;
		auto yScreen = sScanline;
		auto isBgEnabled = (xScreen >= 8u
			|| sMask.showLeftBackground)
			&& sMask.showBackground;
		auto isSpEnabled = (xScreen >= 8u
			|| sMask.showLeftSprites)
			&& sMask.showSprites;
		auto xSprite = 0u;
		byte spColor = 0u;
		byte spPriority = 0u;
		byte spIndex = 0u;
		byte bgColor = byte (sTileBits [sXFine] * (isBgEnabled));
		if (isSpEnabled)
		{
			for (auto i = 0u; i < sSpriteCounter; ++i)
			{
				const auto dx = int (xScreen) - int (sSpritePositions [i]);
				if (dx < 0 || dx >= 8)
					continue;
				auto color = sSpritePatterns [i][dx];
				if (color & 3u)
				{
					spPriority = sSpritePriorities [i];
					spColor = color;
					spIndex = i;
					break;
				}
			}
		}
		byte color = 0u;
		auto b = !!(bgColor & 3);
		auto s = !!(spColor & 3);
		spColor |= 0x10u;
		if (!b && !s)
			color = 0;
		else if (!b && s)
			color = spColor;
		else if (b && !s)
			color = bgColor;
		else
		{
			color = spPriority ? bgColor : spColor;			
			if (!spIndex && sSpriteZeroActive [1u] && xScreen < 255u)
				sStat.spriteZeroHit = 1u;
		}
		sPixels [sFrame & 1u].set (xScreen, yScreen, sPalette.rgba (color));
	}

	template<typename _Host>
	void fetchTiles (_Host&& host,
		bool isTileFetchDotcycle,
		bool isSpriteFetchDotcycle)
	{
		if (isTileFetchDotcycle)
			sTileBits.right ();
		switch (sDotcycle & 0x7u)
		{
		case 0x1u:
			peek (host, bits::pack<12, 4> (sAddress.bits, 2u), sTileIndex);
			break;

		case 0x3u:
			sTileAddr = bits::pack<3, 3, 4, 2, 2> (
				sAddress.xCoarse/4u,
				sAddress.yCoarse/4u,
				0b1111u,
				sAddress.nameTable,
				0b010u);
			peek (host, sTileAddr, sAttrLatch.bits);
			sAttrLatch.right (0u, bits::pack<1, 1> (
				(sAddress.xCoarse%4u)/2u,
				(sAddress.yCoarse%4u)/2u));
			break;

		case 0x5u:
			{
				byte tile = sTileIndex;
				byte base = sCtrl.bgTable;
				byte line = (byte)sAddress.yFine;
				auto sidx = (sDotcycle - 257u) >> 3u;
				if (isSpriteFetchDotcycle && sidx < sSpriteCounter)
				{
					const auto hSprite = sCtrl.spriteSize ? 16u : 8u;
					const auto& object = sSecondaryOAM [sidx];
					base = sCtrl.spriteTable;
					tile = object.tile;
					line = (byte)(sScanline - object.y);
					if (object.attr.flipY)
						line = hSprite - line  - 1u;
					if (sCtrl.spriteSize)
					{
						base = object.tile & 1u;
						tile = (object.tile & 0xfeu) + ((line >> 3u) & 1u);
						line &= 7u;
					}
					sAttrLatch.bits = object.attr.palette;
				}
				sTileAddr = bits::pack<4, 8, 4> (line, tile, base);
				peek (host, sTileAddr + 0u, sTileLatch [0].bits);
			}
			break;

		case 0x7u:
			peek (host, sTileAddr + 8u, sTileLatch [1].bits);
			break;

		case 0x0u:
			if (isTileFetchDotcycle)
			{
				for (auto i = 0u; i < 8u; ++i)
					loadTileColor (sTileBits [8u + i]);
			}
			if (isSpriteFetchDotcycle)
			{
				auto obi = (sDotcycle - 257u) >> 3u;
				const auto& obj = sSecondaryOAM [obi];
				auto& sprBits = sSpritePatterns [obi];
				sSpritePriorities [obi] = obj.attr.priority;
				sSpritePositions [obi] = obj.x;
				for (auto i = 0u; i < 8u; ++i)
					loadTileColor(sprBits [obj.attr.flipX ? 7u - i : i]);
			}
			break;
		default:
			break;
		}
	}

	template<typename _Output>
	void loadTileColor (_Output&& out)
	{
		out = bits::pack<1, 1, 2> (sTileLatch [0u].left (), sTileLatch [1u].left (), sAttrLatch [0u]);
	}

	template <typename _Host>
	auto tickInternal (_Host&& host)
	{
		sScanline = sClock / ctHorizontalTicks;
		sDotcycle = sClock % ctHorizontalTicks;

		auto isRenderingEnabled = sMask.showBackground || sMask.showSprites;
		auto isVisibleScanline = sScanline <= 239u && sScanline >= 0u;
		auto isVisibleDotcycle = sDotcycle <= 256u && sDotcycle >= 1u;
		auto isPrefetchDotcycle = sDotcycle >= 321u && sDotcycle <= 336u;
		auto isTileFetchDotcycle = isPrefetchDotcycle || isVisibleDotcycle;
		auto isSpriteFetchDotcycle = sDotcycle >= 257u && sDotcycle <= 320u;
		auto isPrerenderScanline = sScanline == ctVerticalTicks - 1u;
		auto isVblankScanline = sScanline == ctVblankScanline;

		if (isRenderingEnabled)
		{
			if (sMask.showBackground || sMask.showSprites)
			{
				if (isVisibleScanline && isVisibleDotcycle)
					finalComposite ();

				if (isVisibleScanline || isPrerenderScanline)
				{
					fetchTiles (host, isTileFetchDotcycle, isSpriteFetchDotcycle);
					scrollUpdate (isTileFetchDotcycle, isPrerenderScanline);
					spriteUpdate ();
				}
			}
		}
		flagsUpdate (host, isPrerenderScanline, isVblankScanline);
		timingUpdate (isRenderingEnabled);
	}

	void timingUpdate (bool isRenderingEnabled)
	{
		auto skipTicks = isRenderingEnabled ? (sFrame & 1u) : 0u;
		const auto limitTicks = ctTotalTicks - skipTicks;
		sClock = (sClock + 1u) % limitTicks;
		sFrame += !sClock;
		sFrameReady += !sClock;
	}

	template<typename _Host>
	void flagsUpdate (_Host&& host, bool isPrerenderScanline, bool isVblankScanline)
	{
		if (sDotcycle == 1u)
		{
			if (isVblankScanline)
				nmiUpdate (1u);
			if (isPrerenderScanline)
			{
				sStat.spriteOverflow = 0u;
				sStat.spriteZeroHit = 0u;
				nmiUpdate (0u);
			}
		}
		nmiTimerTick (host);
	}


};
