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

#ifndef LIBMBMETEOR_TIMERS
#define LIBMBMETEOR_TIMERS

#include "mmu.h"
#include "apu.h"
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
    class LIBMBMETEOR_API Timers
    {
	public:
	    Timers(MMU& memory, APU& audio);
	    ~Timers();

	    MMU& timermem;
	    APU& timerapu;

	    uint8_t readtimers(uint32_t addr);
	    void writetimers(uint32_t addr, uint8_t value);

	    struct Timer
	    {
		uint8_t control = 0;
		uint16_t counter = 0;
		uint16_t reload = 0;
		int cycles = 0;

		inline int getoverflowcount()
		{
		    return (0x10000 - counter);
		}

		inline int getoverflowreload()
		{
		    return (0x10000 - reload);
		}

		uint8_t read(int addr)
		{
		    uint8_t temp = 0;
		    switch (addr)
		    {
			case 0: temp = (counter & 0xFF); break;
			case 1: temp = (counter >> 8); break;
			case 2: temp = control; break;
		    }

		    return temp;
		}

		void write(int addr, uint8_t val)
		{
		    switch (addr)
		    {
			case 0: reload = ((reload & 0xFF00) | val); break;
			case 1: reload = ((val << 8) | (reload & 0xFF)); break;
			case 2:
			{
			    bool oldval = TestBit(control, 7);
			    control = (val & 0xC7);
			    
			    if (TestBit(control, 7) && !oldval)
			    {
				cycles = 0;
				counter = reload;
			    }
			}
			break;
		    }
		}
	    };

	    Timer timers[4];

	    int divbits[4] = {0, 6, 8, 10};

	    void updatetimers();

	    int updatetimer(int timer, int prevoverflows);

	    void timerirq(int timer)
	    {
		timermem.writeLong(0x3007FF8, BitSet(timermem.readLong(0x3007FF8), (3 + timer)));	
		timermem.setinterrupt((3 + timer));
	    }
    };
};

#endif // LIBMBMETEOR_TIMERS
