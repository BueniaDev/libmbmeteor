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

#include "libmbmeteor_api.h"
#include "mmu.h"
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
    class LIBMBMETEOR_API Timers
    {
	public:
	    Timers(MMU& memory);
	    ~Timers();

	    MMU& timermem;

	    uint8_t readtimers(uint32_t addr);
	    void writetimers(uint32_t addr, uint8_t value);

	    uint16_t timer0counter = 0;
	    uint16_t timer0reload = 0;
	    uint8_t timer0control = 0;
	    int timer0prescaler = 0;
	    int timer0cycles = 0;

	    uint16_t timer1counter = 0;
	    uint16_t timer1reload = 0;
	    uint8_t timer1control = 0;
	    int timer1prescaler = 0;
	    int timer1cycles = 0;

	    uint16_t timer2counter = 0;
	    uint16_t timer2reload = 0;
	    uint8_t timer2control = 0;
	    int timer2prescaler = 0;
	    int timer2cycles = 0;

	    uint16_t timer3counter = 0;
	    uint16_t timer3reload = 0;
	    uint8_t timer3control = 0;
	    int timer3prescaler = 0;
	    int timer3cycles = 0;

	    bool dump = false;

	    void inittimer(int timer, uint8_t val)
	    {
		switch (timer)
		{
		    case 0:
		    {
			bool prevenable = TestBit(timer0control, 7);
			timer0control = val;

			if (TestBit(timer0control, 7) && !prevenable)
			{
			    timer0counter = timer0reload;
			}

			switch ((timer0control & 0x3))
			{
			    case 0: timer0prescaler = 1; break;
			    case 1: timer0prescaler = 64; break;
			    case 2: timer0prescaler = 256; break;
			    case 3: timer0prescaler = 1024; break;
			}


			if (!TestBit(timer0control, 7) && prevenable)
			{
			    timer0cycles = 0;
			}
		    }
		    break;
		    case 1:
		    {
			bool prevenable = TestBit(timer1control, 7);
			timer1control = val;
			
			if (TestBit(timer1control, 7) && !prevenable)
			{
			    timer1counter = timer1reload;
			}

			switch ((timer1control & 0x3))
			{
			    case 0: timer1prescaler = 1; break;
			    case 1: timer1prescaler = 64; break;
			    case 2: timer1prescaler = 256; break;
			    case 3: timer1prescaler = 1024; break;
			}

			if (TestBit(timer1control, 2))
			{
			    timer1prescaler = 1;
			}

			if (!TestBit(timer1control, 7) && prevenable)
			{
			    timer1cycles = 0;
			}
		    }
		    break;
		    case 2:
		    {
			bool prevenable = TestBit(timer2control, 7);
			timer2control = val;
			
			if (TestBit(timer2control, 7) && !prevenable)
			{
			    timer2counter = timer2reload;
			}

			switch ((timer2control & 0x3))
			{
			    case 0: timer2prescaler = 1; break;
			    case 1: timer2prescaler = 64; break;
			    case 2: timer2prescaler = 256; break;
			    case 3: timer2prescaler = 1024; break;
			}

			if (TestBit(timer2control, 2))
			{
			    timer2prescaler = 1;
			}

			if (!TestBit(timer2control, 7) && prevenable)
			{
			    timer2cycles = 0;
			}
		    }
		    break;
		    case 3:
		    {
			bool prevenable = TestBit(timer3control, 7);
			timer3control = val;
			
			if (TestBit(timer3control, 7) && !prevenable)
			{
			    timer3counter = timer3reload;
			}

			switch ((timer3control & 0x3))
			{
			    case 0: timer3prescaler = 1; break;
			    case 1: timer3prescaler = 64; break;
			    case 2: timer3prescaler = 256; break;
			    case 3: timer3prescaler = 1024; break;
			}

			if (TestBit(timer3control, 2))
			{
			    timer3prescaler = 1;
			}

			if (!TestBit(timer3control, 7) && prevenable)
			{
			    timer3cycles = 0;
			}
		    }
		    break;
		}
	    }

	    void updatetimers()
	    {
		updatetimer0();
		updatetimer1();
		updatetimer2();
		updatetimer3();
	    }

	    void updatetimer0();
	    void updatetimer1();
	    void updatetimer2();
	    void updatetimer3();

	    void timerirq(int timer)
	    {
		timermem.writeLong(0x3007FF8, BitSet(timermem.readLong(0x3007FF8), (3 + timer)));	
		timermem.setinterrupt((3 + timer));
	    }
    };
};

#endif // LIBMBMETEOR_TIMERS
