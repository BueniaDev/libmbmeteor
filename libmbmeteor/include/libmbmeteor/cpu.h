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
	    CPUInterface(MMU& memory, GPU& graphics, Timers& timers);
	    ~CPUInterface();

	    MMU& mem;
	    GPU& gpu;
	    Timers& timer;

	    bool isswi = false;
	    int clockcycles = 0;

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
		val = (val % 0x10000000);

		if (val < 0x2000000)
		{
		    return 1;
		}
		else if (val < 0x2040000)
		{
		    return 3;
		}
		else if (val < 0x8000000)
		{
		    return 1;
		}
		else if (val < 0xA000000)
		{
		    if (TestBit(flags, 1))
		    {
			int temp = 0;

			int nonseq = ((mem.waitcnt >> 2) & 0x3);

			switch (nonseq)
			{
			    case 0: temp = 5; break;
			    case 1: temp = 4; break;
			    case 2: temp = 3; break;
			    case 3: temp = 9; break;
			}

			return temp;
		    }
		    else
		    {
			return TestBit(mem.waitcnt, 4) ? 3 : 2;
		    }
		}
		else if (val < 0xC000000)
		{
		    if (TestBit(flags, 1))
		    {
			int temp = 0;

			int nonseq = ((mem.waitcnt >> 2) & 0x3);

			switch (nonseq)
			{
			    case 0: temp = 5; break;
			    case 1: temp = 4; break;
			    case 2: temp = 3; break;
			    case 3: temp = 9; break;
			}

			return temp;
		    }
		    else
		    {
			return TestBit(mem.waitcnt, 4) ? 5 : 2;
		    }
		}
		else if (val < 0xD000000)
		{
		    return 1;
		}
		else if (val < 0xE000000)
		{
		    if (mem.iseeprom())
		    {
			return 9;
		    }
		    else
		    {
			return 1;
		    }
		}
		else if (val < 0xF000000)
		{
		    if (mem.issram())
		    {
			return 9;
		    }
		    else
		    {
			return 1;
		    }
		}
		else
		{
		    return 1;
		}
	    }

	    void update()
	    {
		clockcycles += 1;
		gpu.updatelcd();
		mem.updatedma();
		timer.updatetimers();
	    }

	    void softwareinterrupt(uint32_t val)
	    {
		mem.softwareinterrupt();
	    }
    };

    class LIBMBMETEOR_API CPU
    {
	public:
	    CPU(MMU& memory, GPU& graphics, Input& input, Timers& timers);
	    ~CPU();

	    void init();
	    void shutdown();

	    MMU& mem;
	    GPU& gpu;
	    Input& joyp;
	    Timers& timer;

	    int tempcycles = 0;

	    bool dump = false;
	    bool isinterrupt = false;

	    int temp = 0;

	    CPUInterface *inter;

	    inline void executenextinstr()
	    {
		arm->executenextinstr();
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
		}

		return cycles;
	    }

	    BeeARM *arm;
    };
};

#endif // LIBMBMETEOR_CPU
