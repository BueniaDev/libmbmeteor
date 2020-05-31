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

#ifndef LIBMBMETEOR_CPU
#define LIBMBMETEOR_CPU

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include "mmu.h"
#include "gpu.h"
#include "apu.h"
#include "input.h"
#include "timers.h"
#include <BeeARM/beearm.h>
#include <BeeARM/beearm_tables.h>
using namespace std;
using namespace gba;
using namespace beearm;

namespace gba
{
    class LIBMBMETEOR_API CPUInterface : public BeeARMInterface
    {
	public:
	    CPUInterface(MMU& memory, GPU& graphics, Timers& timers, APU& audio);
	    ~CPUInterface();

	    MMU& mem;
	    GPU& gpu;
	    Timers& timer;
	    APU& apu;

	    bool isswi = false;
	    int clockcycles = 0;
	    int tempcycles = 0;

	    uint8_t readByte(uint32_t addr)
	    {
		return mem.readByte(addr);
	    }

	    void writeByte(uint32_t addr, uint8_t val)
	    {
		mem.writeByte(addr, val);
	    }

	    uint16_t readWord(uint32_t addr)
	    {
		return mem.readWord(addr);
	    }

	    void writeWord(uint32_t addr, uint16_t val)
	    {
		mem.writeWord(addr, val);
	    }

	    uint32_t readLong(uint32_t addr)
	    {
		return mem.readLong(addr);
	    }

	    void writeLong(uint32_t addr, uint32_t val)
	    {
		mem.writeLong(addr, val);
	    }

	    int clockcycle(uint32_t val, int flags)
	    {
		int temp = 1;

		if (val < 0x2000000)
		{
		    temp = (1 << TestBit(flags, 0));
		}
		else if (val < 0x2040000)
		{
		    temp = (3 << TestBit(flags, 0));
		}
		else if (val < 0x3000000)
		{
		    temp = 1;
		}
		else if (val < 0x4000000)
		{
		    temp = 1;
		}
		else if (val < 0x5000000)
		{
		    temp = 1;
		}
		else if (val < 0x6000000)
		{
		    temp = 1;
		}
		else if (val < 0x7000000)
		{
		    temp = (1 << TestBit(flags, 0));
		}
		else if (val < 0x8000000)
		{
		    temp = (1 << TestBit(flags, 0));
		}
		else if (val < 0xA000000)
		{
		    int ftemp = 1;

		    switch ((flags & 0x3))
		    {
			case 0:
			{
			    ftemp = (TestBit(mem.waitcnt, 4) ? 2 : 3);
			}
			break;
			case 1:
			{
			    ftemp = (TestBit(mem.waitcnt, 4) ? 4 : 6);
			}
			break;
			case 2:
			{
			    switch (((mem.waitcnt >> 2) & 0x3))
			    {
				case 0: ftemp = 5; break;
				case 1: ftemp = 4; break;
				case 2: ftemp = 3; break;
				case 3: ftemp = 9; break;
			    }
			    break;
			}
			break;
			case 3:
			{
			    int waittemp = ((mem.waitcnt >> 2) & 0x7);

			    switch (waittemp)
			    {
				case 0: ftemp = 8; break;
				case 1: ftemp = 7; break;
				case 2: ftemp = 6; break;
				case 3: ftemp = 12; break;
				case 4: ftemp = 7; break;
				case 5: ftemp = 6; break;
				case 6: ftemp = 5; break;
				case 7: ftemp = 11; break;
			    }
			}
			break;
		    }

		    temp = ftemp;
		}
		else if (val < 0xC000000)
		{
		    int ftemp = 1;

		    switch ((flags & 0x3))
		    {
			case 0:
			{
			    ftemp = (TestBit(mem.waitcnt, 7) ? 2 : 5);
			}
			break;
			case 1:
			{
			    ftemp = (TestBit(mem.waitcnt, 7) ? 4 : 10);
			}
			break;
			case 2:
			{
			    switch (((mem.waitcnt >> 5) & 0x3))
			    {
				case 0: ftemp = 5; break;
				case 1: ftemp = 4; break;
				case 2: ftemp = 3; break;
				case 3: ftemp = 9; break;
			    }
			    break;
			}
			break;
			case 3:
			{
			    int waittemp = ((mem.waitcnt >> 5) & 0x7);

			    switch (waittemp)
			    {
				case 0: ftemp = 10; break;
				case 1: ftemp = 9; break;
				case 2: ftemp = 8; break;
				case 3: ftemp = 14; break;
				case 4: ftemp = 7; break;
				case 5: ftemp = 6; break;
				case 6: ftemp = 5; break;
				case 7: ftemp = 11; break;
			    }
			}
			break;
		    }

		    temp = ftemp;
		}
		else if (val < 0xE000000)
		{
		    int ftemp = 1;

		    switch ((flags & 0x3))
		    {
			case 0:
			{
			    ftemp = (TestBit(mem.waitcnt, 10) ? 2 : 9);
			}
			break;
			case 1:
			{
			    ftemp = (TestBit(mem.waitcnt, 10) ? 4 : 18);
			}
			break;
			case 2:
			{
			    switch (((mem.waitcnt >> 8) & 0x3))
			    {
				case 0: ftemp = 5; break;
				case 1: ftemp = 4; break;
				case 2: ftemp = 3; break;
				case 3: ftemp = 9; break;
			    }
			    break;
			}
			break;
			case 3:
			{
			    int waittemp = ((mem.waitcnt >> 8) & 0x7);

			    switch (waittemp)
			    {
				case 0: ftemp = 14; break;
				case 1: ftemp = 13; break;
				case 2: ftemp = 12; break;
				case 3: ftemp = 18; break;
				case 4: ftemp = 7; break;
				case 5: ftemp = 6; break;
				case 6: ftemp = 5; break;
				case 7: ftemp = 11; break;
			    }
			}
			break;
		    }

		    temp = ftemp;
		}
		else if (val < 0xF000000)
		{
		    temp = 1;
		}
		else
		{
		    temp = 1;
		}

		return temp;
	    }

	    void update()
	    {
		clockcycles += 1;
		// tempcycles += 1;
		gpu.updatelcd(1);
		timer.updatetimers();
		apu.updateapu();
	    }

	    void softwareinterrupt(uint32_t val)
	    {
		mem.softwareinterrupt();
	    }

	    int getversion()
	    {
		return 4;
	    }

	    uint32_t readcoprocessor(uint16_t id)
	    {
		return 0;
	    }

	    void writecoprocessor(uint16_t id, uint32_t val)
	    {
		return;
	    }
	    
	    void exceptionreturncallback()
	    {
	    	if ((mem.memarm.getcpsr() & 0x1F) == 0x12)
	    	{
	    	    mem.lastbiosvalue = 0xE55EC002;
	    	}
	    	else if ((mem.memarm.getcpsr() & 0x1F) == 0x13)
	    	{
	    	    mem.lastbiosvalue = 0xE3A02004;
	    	}
	    }
    };

    class LIBMBMETEOR_API CPU
    {
	public:
	    CPU(MMU& memory, GPU& graphics, Input& input, Timers& timers, APU& audio);
	    ~CPU();

	    void init();
	    void shutdown();

	    MMU& mem;
	    GPU& gpu;
	    Input& joyp;
	    Timers& timer;
	    APU& apu;

	    int tempcycles = 0;

	    bool tempbool = false;
	    bool dump = false;
	    bool isinterrupt = false;

	    int temp = 0;
	    uint32_t val = 0;

	    CPUInterface *inter;

	    inline void executenextinstr()
	    {
		arm->executenextinstr();
	    }
	    
	    inline void interrupt()
	    {
	        arm->irqexception();
	    }

	    inline int runfor(int cycles)
	    {
		while (cycles > 0)
		{
		    if (mem.dmainprogress())
		    {   
			mem.updatedma();
			continue;
		    }

		    if ((mem.interruptmasterenable && !TestBit(arm->getcpsr(), 7)))
		    {
			if (mem.isinterruptsenabled())
			{
			    arm->irqexception();
			}
		    }

		    executenextinstr();
		    cycles -= inter->clockcycles;
		    inter->clockcycles = 0;
		    
		    /*
		    if (arm->getreg(15) == 0x80018D6)
		    {
		        dump = true;
		    }
		    
		    if (dump == true)
		    {
		        arm->printregs();
		        cout << endl;
		        
		        if (arm->getreg(15) == 0x80018F4)
		        {
		            exit(1);
		        }
		    }
		    */
		}

		return cycles;
	    }

	    BeeARM *arm;
    };
};

#endif // LIBMBMETEOR_CPU
