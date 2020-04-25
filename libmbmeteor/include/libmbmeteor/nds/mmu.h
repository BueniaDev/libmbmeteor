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

#ifndef LIBMBMETEOR_DSMMU
#define LIBMBMETEOR_DSMMU

#include "../libmbmeteor_api.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <bitset>
#include <functional>
#include <BeeARM/beearm.h>
#include <BeeARM/beearm_tables.h>
using namespace std;
using namespace beearm;

namespace nds
{
    using memoryreadfunc = function<uint8_t(uint32_t)>;
    using memorywritefunc = function<void(uint32_t, uint8_t)>;

    class LIBMBMETEOR_API NDSMMU
    {
	public:
	    NDSMMU();
	    ~NDSMMU();

	    vector<uint8_t> bios9;
	    vector<uint8_t> mainmem9;
	    vector<uint8_t> mainmem7;

	    vector<uint8_t> wram7;

	    vector<uint8_t> bios7;

	    vector<uint8_t> firmware;

	    vector<uint8_t> gamecart;

		array<memoryreadfunc, 0x1000000> memoryreadhandlers9;
		array<memorywritefunc, 0x1000000> memorywritehandlers9;

		array<memoryreadfunc, 0x1000000> memoryreadhandlers7;
		array<memorywritefunc, 0x1000000> memorywritehandlers7;

		void addmemoryreadhandler9(uint32_t addr, memoryreadfunc cb)
		{
			memoryreadhandlers9.at((addr - 0x4000000)) = cb;
		}
		
		void addmemorywritehandler9(uint32_t addr, memorywritefunc cb)
		{
			memorywritehandlers9.at((addr - 0x4000000)) = cb;
		}

		void addmemoryreadhandler7(uint32_t addr, memoryreadfunc cb)
		{
			memoryreadhandlers7.at((addr - 0x4000000)) = cb;
		}
		
		void addmemorywritehandler7(uint32_t addr, memorywritefunc cb)
		{
			memorywritehandlers7.at((addr - 0x4000000)) = cb;
		}
		
		uint8_t readtemp(uint32_t addr)
		{
		    // cout << "Unrecognized read from " << hex << (int)(addr) << endl;
		    return 0x00;
		}
		
		void writetemp(uint32_t addr, uint8_t val)
		{
		    // cout << "Unrecognized write to " << hex << (int)(addr) << endl;
		    return;
		}

	    uint8_t readByte9(uint32_t addr);
	    uint16_t readWord9(uint32_t addr);
	    uint32_t readLong9(uint32_t addr);

	    void writeByte9(uint32_t addr, uint8_t val);
	    void writeWord9(uint32_t addr, uint16_t val);
	    void writeLong9(uint32_t addr, uint32_t val);

	    uint8_t readByte7(uint32_t addr);
	    uint16_t readWord7(uint32_t addr);
	    uint32_t readLong7(uint32_t addr);

	    void writeByte7(uint32_t addr, uint8_t val);
	    void writeWord7(uint32_t addr, uint16_t val);
	    void writeLong7(uint32_t addr, uint32_t val);

	    void softwareinterrupt9();
	    void softwareinterrupt7();

	    uint32_t readcp15(uint16_t id);
	    void writecp15(uint16_t id, uint32_t val);

	    uint32_t cp15ctrl = 0x78;

	    inline void updateitcmsetting()
	    {
		if (TestBit(cp15ctrl, 18))
		{
		    cout << "Enabling ITCM..." << endl;
		    exit(1);
		}
		else
		{
		    cout << "ITCM disabled..." << endl;
		}
	    }

	    inline void updatedtcmsetting()
	    {
		if (TestBit(cp15ctrl, 16))
		{
		    arm9dtcmbase = (dtcmsetting & 0xFFFFF000);
		    arm9dtcmsize = (256 << (dtcmsetting & 0x3E));
		    cout << "DTCM enable at " << hex << (int)(arm9dtcmbase) << ", size of " << dec << (int)(arm9dtcmsize) << endl;
		}
		else
		{
		    arm9dtcmbase = 0xFFFFFFFF;
		    arm9dtcmsize = 0;
		    cout << "DTCM disabled..." << endl;
		}
	    }

	    uint32_t dtcmsetting = 0;
	    uint32_t arm9dtcmbase = 0xFFFFFFFF;
	    uint32_t arm9dtcmsize = 0;

	    uint8_t readipc9(uint32_t addr);
	    void writeipc9(uint32_t addr, uint8_t val);

	    uint8_t readipc7(uint32_t addr);
	    void writeipc7(uint32_t addr, uint8_t val);

	    uint8_t readspi7(uint32_t addr);
	    void writespi7(uint32_t addr, uint8_t val);

	    uint8_t readinterrupts7(uint32_t addr);
	    void writeinterrupts7(uint32_t addr, uint8_t val);

	    bool interruptmasterenable7 = false;
	    uint32_t iereg7 = 0;
	    uint32_t ifreg7 = 0;

	    void interruptexception7()
	    {
		memarm7.irqexception();
	    }

	    bool isinterruptsenabled7()
	    {
		return ((iereg7 & ifreg7) != 0);
	    }

	    void setinterrupt(int num)
	    {
		setinterrupt7(num);
	    }

	    void clearinterrupt(int num)
	    {
		clearinterrupt7(num);
	    }

	    void setinterrupt7(int num)
	    {
		writeLong7(0x380FFF8, BitSet(readLong7(0x380FFF8), num));
		ifreg7 = BitSet(ifreg7, num);
	    }

	    void clearinterrupt7(int num)
	    {
		writeLong7(0x380FFF8, BitReset(readLong7(0x380FFF8), num));
		ifreg7 = BitReset(ifreg7, num);
	    }

	    uint16_t ipcsync9 = 0;
	    uint16_t ipcsync7 = 0;

	    uint16_t spicontrol = 0;

	    inline uint8_t readspidata()
	    {
		uint8_t temp = 0;
		if (!TestBit(spicontrol, 15))
		{
		    return 0;
		}

		switch (((spicontrol >> 8) & 0x3))
		{
		    case 0x1: temp = readspifirmware(); break;
		}

		return temp;
	    }

	    inline void writespidata(uint8_t val)
	    {
		if (!TestBit(spicontrol, 15))
		{
		    return;
		}

		switch (((spicontrol >> 8) & 0x3))
		{
		    case 0x1: writespifirmware(val, TestBit(spicontrol, 11)); break;
		}

		if (TestBit(spicontrol, 14))
		{
		    setinterrupt7(23);
		}
	    }

	    uint32_t firmwareaddr = 0;
	    uint8_t firmwarecmd = 0;
	    uint32_t firmwaredatapos = 0;
	    bool firmwarehold = false;
	    uint8_t firmwaredata = 0;

	    inline uint8_t readspifirmware()
	    {
		return firmwaredata;
	    }

	    inline void writespifirmware(uint8_t val, bool hold)
	    {
		if (!hold)
		{
		    firmwarehold = false;
		}

		if (hold && !firmwarehold)
		{
		    firmwarecmd = val;
		    firmwarehold = true;
		    firmwaredatapos = 1;
		    firmwareaddr = 0;
		    return;
		}

		switch (firmwarecmd)
		{
		    case 0x03:
		    {
			if (firmwaredatapos < 4)
			{
			    firmwareaddr <<= 8;
			    firmwareaddr |= val;
			    firmwaredata = 0;

			    if (firmwaredatapos == 3)
			    {
				cout << "Firmware SPI read from " << hex << (int)(firmwareaddr) << endl;
			    }
			}
			else
			{
			    if (firmwareaddr >= firmware.size())
			    {
				firmwaredata = 0;
			    }
			    else
			    {
				firmwaredata = firmware[firmwareaddr];
			    }

			    firmwareaddr += 1;
			}

			firmwaredatapos += 1;
		    }
		    break;
		    default: cout << "Unrecognized firmware command of " << hex << (int)(firmwarecmd) << endl; exit(1); break;
		}
	    }

	    array<uint8_t, 0x4000> arm9dtcm;

	    bool loadBIOS9(string filename);
	    bool loadBIOS7(string filename);
	    bool loadfirmware(string filename);

	    BeeARM memarm9;
	    BeeARM memarm7;
    };
};

#endif // LIBMBMETEOR_DSMMU
