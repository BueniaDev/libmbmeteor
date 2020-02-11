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

		vector<uint8_t> wram256;
		vector<uint8_t> wram32;
		vector<uint8_t> pram;
		vector<uint8_t> vram;
		vector<uint8_t> oam;
	    vector<uint8_t> gamerom;
		
		BeeARM memarm; // For "open-bus" behavior
		
		array<memoryreadfunc, 0x3FF> memoryreadhandlers;
		array<memorywritefunc, 0x3FF> memorywritehandlers;
		
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
		    cout << "Unrecognized read from " << hex << (int)(addr) << endl;
		    return 0xFF;
		}
		
		void writetemp(uint32_t addr, uint8_t val)
		{
		    cout << "Unrecognized write to " << hex << (int)(addr) << endl;
		    return;
		}

	    uint8_t readByte(uint32_t addr);
	    void writeByte(uint32_t addr, uint8_t val);
	    uint16_t readWord(uint32_t addr);
	    void writeWord(uint32_t addr, uint16_t val);
	    uint32_t readLong(uint32_t addr);
	    void writeLong(uint32_t addr, uint32_t val);

	    bool loadROM(string filename);
		
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
