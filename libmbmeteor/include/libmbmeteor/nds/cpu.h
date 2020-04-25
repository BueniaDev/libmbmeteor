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

#ifndef LIBMBMETEOR_DSCPU
#define LIBMBMETEOR_DSCPU

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include "mmu.h"
#include <BeeARM/beearm.h>
#include <BeeARM/beearm_tables.h>
using namespace std;
using namespace nds;
using namespace beearm;

namespace nds
{
    class LIBMBMETEOR_API CPU9Interface : public BeeARMInterface
    {
	public:
	    CPU9Interface(NDSMMU& memory);
	    ~CPU9Interface();

	    NDSMMU& mem;

	    bool isswi = false;
	    int clockcycles = 0;

	    uint8_t readByte(uint32_t addr)
	    {
		return mem.readByte9(addr);
	    }

	    void writeByte(uint32_t addr, uint8_t val)
	    {
		mem.writeByte9(addr, val);
	    }

	    uint16_t readWord(uint32_t addr)
	    {
		return mem.readWord9(addr);
	    }

	    void writeWord(uint32_t addr, uint16_t val)
	    {
		mem.writeWord9(addr, val);
	    }

	    uint32_t readLong(uint32_t addr)
	    {
		return mem.readLong9(addr);
	    }

	    void writeLong(uint32_t addr, uint32_t val)
	    {
		mem.writeLong9(addr, val);
	    }

	    int clockcycle(uint32_t val, int flags)
	    {
		return 2;
	    }

	    void update()
	    {
		clockcycles += 1;
	    }

	    void softwareinterrupt(uint32_t val)
	    {
		mem.softwareinterrupt9();
	    }

	    int getversion()
	    {
		return 5;
	    }

	    uint32_t readcoprocessor(uint16_t id)
	    {
		return mem.readcp15(id);
	    }

	    void writecoprocessor(uint16_t id, uint32_t val)
	    {
		mem.writecp15(id, val);
	    }
    };

    class LIBMBMETEOR_API CPU7Interface : public BeeARMInterface
    {
	public:
	    CPU7Interface(NDSMMU& memory);
	    ~CPU7Interface();

	    NDSMMU& mem;

	    bool isswi = false;
	    int clockcycles = 0;

	    uint8_t readByte(uint32_t addr)
	    {
		return mem.readByte7(addr);
	    }

	    void writeByte(uint32_t addr, uint8_t val)
	    {
		mem.writeByte7(addr, val);
	    }

	    uint16_t readWord(uint32_t addr)
	    {
		return mem.readWord7(addr);
	    }

	    void writeWord(uint32_t addr, uint16_t val)
	    {
		mem.writeWord7(addr, val);
	    }

	    uint32_t readLong(uint32_t addr)
	    {
		return mem.readLong7(addr);
	    }

	    void writeLong(uint32_t addr, uint32_t val)
	    {
		mem.writeLong7(addr, val);
	    }

	    int clockcycle(uint32_t val, int flags)
	    {
		return 1;
	    }

	    void update()
	    {
		clockcycles += 1;
	    }

	    void softwareinterrupt(uint32_t val)
	    {
		cout << "ARM7 SWI" << endl;
		mem.softwareinterrupt7();
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
    };

    class LIBMBMETEOR_API NDSCPU
    {
	public:
	    NDSCPU(NDSMMU& memory);
	    ~NDSCPU();

	    void init();
	    void shutdown();

	    NDSMMU& mem;

	    int tempcycles = 0;

	    bool dump = false;
	    bool isinterrupt = false;

	    int temp = 0;

	    CPU9Interface *inter9;
	    CPU7Interface *inter7;
	    BeeARM *arm9;
	    BeeARM *arm7;

	    bool nds9sync = true;
	    bool nds7sync = false;
	    int nds9synccycles = 0;
	    int nds7synccycles = 0;
	    int cpusynccycles = 0;

	    inline void irqexception7()
	    {
		if ((mem.interruptmasterenable7 && !TestBit(arm7->getcpsr(), 7)))
		{
		    if (mem.isinterruptsenabled7())
		    {
			cout << "ARM7 IRQ" << endl;
			exit(1);
		    }
		}
	    }

	    inline void executenextinstr(int cycles)
	    {
		if (nds9sync)
		{
		    nds9synccycles = 0;
		    arm9->executenextinstr();
		    synccpu9();
		}

		if (nds7sync)
		{
		    nds7synccycles = 0;
		    irqexception7();
		    arm7->executenextinstr();

		    synccpu7();
		}
	    }

	    inline void synccpu9()
	    {
		int tempcycles = inter9->clockcycles;
		tempcycles >>= 1;
		nds9synccycles += tempcycles;

		cpusynccycles -= nds9synccycles;

		if (cpusynccycles <= 0)
		{
		    // cout << "Running NDS7..." << endl;
		    nds9sync = false;
		    nds7sync = true;
		    cpusynccycles *= -1;
		    // cout << "ARM7 clock cycles: " << dec << (int)(cpusynccycles) << endl;
		}
	    }

	    inline void synccpu7()
	    {
		nds7synccycles += inter7->clockcycles;

		cpusynccycles -= nds7synccycles;

		if (cpusynccycles <= 0)
		{
		    // cout << "Running NDS9..." << endl;
		    nds9sync = true;
		    nds7sync = false;
		    cpusynccycles *= -1;
		    // cout << "ARM9 clock cycles: " << dec << (int)(cpusynccycles) << endl;
		}
	    }

	    inline int runfor(int cycles)
	    {
		while (cycles > 0)
		{
		    executenextinstr(cycles);
		    cycles -= inter9->clockcycles;
		    inter9->clockcycles = inter7->clockcycles = 0;
		}

		return cycles;
	    }
    };
};

#endif // LIBMBMETEOR_DSCPU
