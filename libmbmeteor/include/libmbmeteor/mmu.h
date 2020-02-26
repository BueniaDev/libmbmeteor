// This file is part of libmbmeteor.
// Copyright (C) 2019 Buenia.
//
// libmbmeteor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbmeteor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbmeteor.  If not, see <https://www.gnu.org/licenses/>.

#ifndef LIBMBMETEOR_MMU
#define LIBMBMETEOR_MMU

#include "libmbmeteor_api.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <functional>
#include <BeeARM/beearm.h>
#include <BeeARM/beearm_tables.h>
using namespace std;
using namespace beearm;

namespace gba
{
	using memoryreadfunc = function<uint8_t(uint32_t)>;
	using memorywritefunc = function<void(uint32_t, uint8_t)>;
	
    class LIBMBMETEOR_API MMU
    {
	public:
	    MMU();
	    ~MMU();

		void init();
		void shutdown();

		vector<uint8_t> bios;
		vector<uint8_t> wram256;
		vector<uint8_t> wram32;
		vector<uint8_t> pram;
		vector<uint8_t> vram;
		vector<uint8_t> oam;
	    vector<uint8_t> gamerom;
	    vector<uint8_t> gamesram;
	    vector<uint8_t> gameeeprom;

	    bool iseepromsave = false;
		
		BeeARM memarm; // For "open-bus" behavior
		
		array<memoryreadfunc, 0xFFF> memoryreadhandlers;
		array<memorywritefunc, 0xFFF> memorywritehandlers;

		bool dump = false;
		
		void addmemoryreadhandler(uint32_t addr, memoryreadfunc cb)
		{
			memoryreadhandlers.at((addr - 0x4000000)) = cb;
		}
		
		void addmemorywritehandler(uint32_t addr, memorywritefunc cb)
		{
			memorywritehandlers.at((addr - 0x4000000)) = cb;
		}
		
		uint8_t readtemp(uint32_t addr)
		{
		    // cout << "Unrecognized read from " << hex << (int)(addr) << endl;
		    return 0xFF;
		}
		
		void writetemp(uint32_t addr, uint8_t val)
		{
		    // cout << "Unrecognized write to " << hex << (int)(addr) << endl;
		    return;
		}

		bool iseeprom()
		{
		    return (carttype == CartridgeType::CartEEPROM);
		}

		bool issram()
		{
		    return (carttype == CartridgeType::CartSRAM);
		}

	    uint8_t readByte(uint32_t addr);
	    void writeByte(uint32_t addr, uint8_t val);
	    uint16_t readWord(uint32_t addr);
	    void writeWord(uint32_t addr, uint16_t val);
	    uint32_t readLong(uint32_t addr);
	    void writeLong(uint32_t addr, uint32_t val);

	    uint8_t readdma(uint32_t addr);
	    void writedma(uint32_t addr, uint8_t val);

	    uint8_t readinterrupts(uint32_t addr);
	    void writeinterrupts(uint32_t addr, uint8_t val);

	    uint8_t readeeprom(uint32_t addr);
	    void writeeeprom(uint8_t value);

	    int eeprombuffer = 0;
	    int eeprombufflength = 0;
	    int eepromcmd = 0;
	    int eepromaddr = 0;
	    int eepromwrite = 0;
	    int eepromstate = 1;
	    int eepromdelay = 4;
	    int eeprombitsread = 64;
	    int eeprombyteswrite = 0;
	    int eeprombits = 7;
	    int eeprombitsize = 6;
	    int eeprombitmask = 0x3F;

	    void softwareinterrupt();

	    uint16_t iereg = 0;
	    uint16_t ifreg = 0;
	    bool interruptmasterenable = false;

	    uint16_t waitcnt = 0;

	    void setinterrupt(int num)
	    {
		ifreg = BitSet(ifreg, num);
	    }

	    void clearinterrupt(int num)
	    {
		ifreg = BitReset(ifreg, num);
	    }

	    bool isinterruptsenabled()
	    {
		return (iereg & ifreg);
	    }

	    enum DmaState : int
	    {
		Inactive = -1,
		Starting = 0,
		Paused = 1,
		Active = 2,
	    };

	    uint32_t dma3src = 0;
	    uint32_t dma3dst = 0;
	    uint16_t dma3length = 0;
	    uint16_t dma3control = 0;
	    int dma3unitstocopy = 0;

	    DmaState dma3state = DmaState::Inactive;

	    void initdma3()
	    {
		if (dma3state == DmaState::Inactive)
		{
		    if (TestBit(dma3control, 15))
		    {
			cout << "DMA3 destination address: " << hex << (int)(dma3dst) << endl;
			memarm.printregs();

			dma3unitstocopy = (dma3length == 0) ? 0x10000 : dma3length;

			if ((dma3dst >> 24) == 0xD)
			{
			    if (dma3unitstocopy == 17)
			    {
				eeprombitsize = 14;
				eeprombitmask = 0x3FF;
				gameeeprom.clear();
				gameeeprom.resize(0x2000, 0);
			    }
			}
			
			int dma3starttiming = ((dma3control >> 12) & 0x3);

			switch (dma3starttiming)
			{
			    case 0: dma3state = DmaState::Starting; break;
			    default: cout << "Unrecognized DMA start timing of " << dec << (int)(dma3starttiming) << endl; exit(1); break;
			}
		    }
		}
		else
		{
		    cout << "Restarting DMA3 transfer..." << endl;
		    exit(1);
		}	
	    }

	    bool dma3inprogress()
	    {
		return ((dma3state == DmaState::Starting) || (dma3state == DmaState::Active));
	    }

	    bool dmainprogress()
	    {
		return dma3inprogress();
	    }

	    void updatedma()
	    {
		updatedma3();
	    }

	    void updatedma3()
	    {
		if (dma3state == DmaState::Starting)
		{
		    dma3state = DmaState::Active;
		}
		else if (dma3state == DmaState::Active)
		{
		    executedma3();

		    if (dma3unitstocopy == 0)
		    {
			if (!TestBit(dma3control, 9))
			{
			    dma3state = DmaState::Inactive;
			    dma3control = BitReset(dma3control, 15);

			    if (TestBit(dma3control, 14))
			    {
				cout << "DMA3 IRQ" << endl;
			    }
			}
			else
			{
			    cout << "Repeating DMA transfer..." << endl;
			    exit(1);
			}
		    }
		}
	    }

	    void executedma3()
	    {
		if (TestBit(dma3control, 10))
		{
		    if ((dma3src & 0x3) || (dma3dst & 0x3))
		    {
			cout << "Misaligned DMA address" << endl;
			exit(1);
		    }

		    int srccontrol = ((dma3control >> 7) & 0x3);
		    int dstcontrol = ((dma3control >> 5) & 0x3);

		    writeLong(dma3dst, readLong(dma3src));

		    switch (dstcontrol)
		    {
			case 0: dma3dst += 4; break;
			case 1: dma3dst -= 4; break;
			case 2: break;
			case 3: cout << "Increment/Reload" << endl; exit(1); break;
		    }

		    switch (srccontrol)
		    {
			case 0: dma3src += 4; break;
			case 1: dma3src -= 4; break;
			case 2: break;
			case 3: cout << "Prohibited" << endl; exit(1); break;
		    }
		}
		else
		{
		    if ((dma3src & 0x1) || (dma3dst & 0x1))
		    {
			cout << "Misaligned DMA address" << endl;
			exit(1);
		    }

		    int srccontrol = ((dma3control >> 7) & 0x3);
		    int dstcontrol = ((dma3control >> 5) & 0x3);

		    writeWord(dma3dst, readWord(dma3src));

		    switch (dstcontrol)
		    {
			case 0: dma3dst += 2; break;
			case 1: dma3dst -= 2; break;
			case 2: break;
			case 3: cout << "Increment/Reload" << endl; exit(1); break;
		    }

		    switch (srccontrol)
		    {
			case 0: dma3src += 2; break;
			case 1: dma3src -= 2; break;
			case 2: break;
			case 3: cout << "Prohibited" << endl; exit(1); break;
		    }
		}		

		dma3unitstocopy -= 1;
	    }

	    bool loadBIOS(string filename);
	    bool loadROM(string filename);

	    enum CartridgeType : int
	    {
		None = -1,
		CartSRAM = 0,
		CartEEPROM = 1,
	    };

	    CartridgeType carttype;

	    CartridgeType getcarttype(vector<uint8_t> &rom)
	    {
		CartridgeType type = CartridgeType::None;

		cout << dec << (int)(rom.size()) << endl;
		for (int i = 0; i < (int)(rom.size() - 7); i++)
		{
		    if (strncmp((const char*)&rom[i], "SRAM_V", 6) == 0)
		    {
			type = CartridgeType::CartSRAM;
		    }
		}

		for (int i = 0; i < (int)(rom.size() - 9); i++)
		{
		    if (strncmp((const char*)&rom[i], "EEPROM_V", 8) == 0)
		    {
			type = CartridgeType::CartEEPROM;
		    }
		}

		return type;
	    }
		
		uint8_t readopenbus(uint32_t addr)
		{
			uint8_t temp = 0;
			
			uint32_t armtemp = 0;
			
			if (memarm.instmode == memarm.armmode)
			{
				armtemp = readLong((memarm.getreg(15) + 8));
			}
			else
			{
				uint16_t thumbtemp = readWord((memarm.getreg(15) + 4));
				armtemp = ((thumbtemp << 16) | thumbtemp);
			}
			
			int addrtemp = (addr & 0x3);
			temp = ((armtemp >> (addrtemp << 3)) & 0xFF);
			return temp;
		}
    };
};

#endif // LIBMBMETEOR_MMU
