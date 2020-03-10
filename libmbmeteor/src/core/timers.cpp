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

#include "../../include/libmbmeteor/timers.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    Timers::Timers(MMU& memory) : timermem(memory)
    {
	for (int i = 0x100; i < 0x110; i++)
	{
	    timermem.addmemoryreadhandler((0x4000000 + i), bind(&Timers::readtimers, this, _1));
	    timermem.addmemorywritehandler((0x4000000 + i), bind(&Timers::writetimers, this, _1, _2));
	}
    }

    Timers::~Timers()
    {

    }

    uint8_t Timers::readtimers(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFF))
	{
	    case 0x100: temp = (timer0counter & 0xFF); break;
	    case 0x101: temp = (timer0counter >> 8); break;
	    case 0x102: temp = timer0control; break;
	    case 0x103: temp = 0x00; break;
	    case 0x104: temp = (timer1counter & 0xFF); break;
	    case 0x105: temp = (timer1counter >> 8); break;
	    case 0x106: temp = timer1control; break;
	    case 0x107: temp = 0x00; break;
	    case 0x108: temp = (timer2counter & 0xFF); break;
	    case 0x109: temp = (timer2counter >> 8); break;
	    case 0x10A: temp = timer2control; break;
	    case 0x10B: temp = 0x00; break;
	    case 0x10C: temp = (timer3counter & 0xFF); break;
	    case 0x10D: temp = (timer3counter >> 8); break;
	    case 0x10E: temp = timer3control; break;
	    case 0x10F: temp = 0x00; break;
	    default: cout << "Unrecognized timer read of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); break;
	}

	return temp;
    }

    void Timers::writetimers(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x100: timer0reload = ((timer0reload & 0xFF00) | val); break;
	    case 0x101: timer0reload = ((val << 8) | (timer0reload & 0xFF)); break;
	    case 0x102: timer0control = val; inittimer(0); break;
	    case 0x103: return; break;
	    case 0x104: timer1reload = ((timer1reload & 0xFF00) | val); break;
	    case 0x105: timer1reload = ((val << 8) | (timer1reload & 0xFF)); break;
	    case 0x106: timer1control = val; inittimer(1); break;
	    case 0x107: return; break;
	    case 0x108: timer2reload = ((timer2reload & 0xFF00) | val); break;
	    case 0x109: timer2reload = ((val << 8) | (timer2reload & 0xFF)); break;
	    case 0x10A: timer2control = val; inittimer(2); break;
	    case 0x10B: return; break;
	    case 0x10C: timer3reload = ((timer3reload & 0xFF00) | val); break;
	    case 0x10D: timer3reload = ((val << 8) | (timer3reload & 0xFF)); break;
	    case 0x10E: timer3control = val; inittimer(3); break;
	    case 0x10F: return; break;
	    default: cout << "Unrecognized timer write of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); break;
	}
    }

    void Timers::updatetimer0()
    {
	if (TestBit(timer0control, 7))
	{
	    if (TestBit(timer0control, 2))
	    {
		return;
	    }
	    else
	    {
		int timerprescaler = 0;

		switch ((timer0control & 0x3))
		{
		    case 0: timerprescaler = 1; break;
		    case 1: timerprescaler = 64; break;
		    case 2: timerprescaler = 256; break;
		    case 3: timerprescaler = 1024; break;
		}

		timer0cycles += 1;

		if (timer0cycles == timerprescaler)
		{
		    timer0cycles = 0;

		    timer0counter += 1;

		    if (timer0counter == 0)
		    {
			timer0counter = timer0reload;

			if (TestBit(timer0control, 6))
			{
			    timerirq(0);
			}
		    }
		}
	    }
	}
    }

    void Timers::updatetimer1()
    {
	if (TestBit(timer1control, 7))
	{
	    if (TestBit(timer1control, 2))
	    {
		if (timer0counter == 0xFFFF)
		{
		    timer1counter += 1;

		    if (timer1counter == 0)
		    {
			timer1counter = timer1reload;

			if (TestBit(timer1control, 6))
			{
			    timerirq(1);
			}
		    }
		}
	    }
	    else
	    {
		int timerprescaler = 0;

		switch ((timer1control & 0x3))
		{
		    case 0: timerprescaler = 1; break;
		    case 1: timerprescaler = 64; break;
		    case 2: timerprescaler = 256; break;
		    case 3: timerprescaler = 1024; break;
		}

		timer1cycles += 1;

		if (timer1cycles == timerprescaler)
		{
		    timer1cycles = 0;

		    timer1counter += 1;

		    if (timer1counter == 0)
		    {
			timer1counter = timer1reload;

			if (TestBit(timer1control, 6))
			{
			    timerirq(1);
			}
		    }
		}
	    }
	}
    }

    void Timers::updatetimer2()
    {
	if (TestBit(timer2control, 7))
	{
	    if (TestBit(timer2control, 2))
	    {
		if (timer1counter == 0xFFFF)
		{
		    timer2counter += 1;

		    if (timer2counter == 0)
		    {
			timer2counter = timer2reload;

			if (TestBit(timer2control, 6))
			{
			    timerirq(2);
			}
		    }
		}
	    }
	    else
	    {
		int timerprescaler = 0;

		switch ((timer2control & 0x3))
		{
		    case 0: timerprescaler = 1; break;
		    case 1: timerprescaler = 64; break;
		    case 2: timerprescaler = 256; break;
		    case 3: timerprescaler = 1024; break;
		}

		timer2cycles += 1;

		if (timer2cycles == timerprescaler)
		{
		    timer2cycles = 0;

		    timer2counter += 1;

		    if (timer2counter == 0)
		    {
			timer2counter = timer2reload;

			if (TestBit(timer2control, 6))
			{
			    timerirq(2);
			}
		    }
		}
	    }
	}
    }

    void Timers::updatetimer3()
    {
	if (TestBit(timer3control, 7))
	{
	    if (TestBit(timer3control, 2))
	    {
		if (timer2counter == 0xFFFF)
		{
		    timer3counter += 1;

		    if (timer3counter == 0)
		    {
			timer3counter = timer3reload;

			if (TestBit(timer3control, 6))
			{
			    timerirq(3);
			}
		    }
		}
	    }
	    else
	    {
		int timerprescaler = 0;

		switch ((timer3control & 0x3))
		{
		    case 0: timerprescaler = 1; break;
		    case 1: timerprescaler = 64; break;
		    case 2: timerprescaler = 256; break;
		    case 3: timerprescaler = 1024; break;
		}

		timer3cycles += 1;

		if (timer3cycles == timerprescaler)
		{
		    timer3cycles = 0;

		    timer3counter += 1;

		    if (timer3counter == 0)
		    {
			timer3counter = timer3reload;

			if (TestBit(timer3control, 6))
			{
			    timerirq(3);
			}
		    }
		}
	    }
	}
    }
};
