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
	    CPUInterface(MMU& memory, GPU& graphics);
	    ~CPUInterface();

	    MMU& mem;
		GPU& gpu;

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
		val = (val % 0x1000000);

		if (val < 0x8000000)
		{
		    return 1;
		}
		else if (val < 0xA000000)
		{
		    if (TestBit(flags, 1))
		    {
			return 5;
		    }
		    else
		    {
			return 3;
		    }
		}
		else
		{
		    return 1;
		}
	    }

	    void update()
	    {
		gpu.updatelcd();
	    }
    };

    class LIBMBMETEOR_API CPU
    {
	public:
	    CPU(MMU& memory, GPU& graphics);
	    ~CPU();

	    void init();
	    void shutdown();

	    MMU& mem;
		GPU& gpu;

	    CPUInterface *inter;

	    inline void executenextinstr()
	    {
		arm->executenextinstr();
	    }

	    inline int runfor(int cycles)
	    {
		while (cycles > 0)
		{
		    int clockcycles = arm->clockcycles;
		    executenextinstr();
		    cycles -= (arm->clockcycles - clockcycles);
		}

		return cycles;
	    }

	    BeeARM *arm;
    };
};

#endif // LIBMBMETEOR_CPU
