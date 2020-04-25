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

#include "../../../include/libmbmeteor/gba/timers.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    Timers::Timers(MMU& memory, APU& audio) : timermem(memory), timerapu(audio)
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
	    case 0x100: temp = timers[0].read(0); break;
	    case 0x101: temp = timers[0].read(1); break;
	    case 0x102: temp = timers[0].read(2); break;
	    case 0x103: temp = 0x00; break;
	    case 0x104: temp = timers[1].read(0); break;
	    case 0x105: temp = timers[1].read(1); break;
	    case 0x106: temp = timers[1].read(2); break;
	    case 0x107: temp = 0x00; break;
	    case 0x108: temp = timers[2].read(0); break;
	    case 0x109: temp = timers[2].read(1); break;
	    case 0x10A: temp = timers[2].read(2); break;
	    case 0x10B: temp = 0x00; break;
	    case 0x10C: temp = timers[3].read(0); break;
	    case 0x10D: temp = timers[3].read(1); break;
	    case 0x10E: temp = timers[3].read(2); break;
	    case 0x10F: temp = 0x00; break;
	    default: cout << "Unrecognized timer read of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); break;
	}

	return temp;
    }

    void Timers::writetimers(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x100: timers[0].write(0, val); break;
	    case 0x101: timers[0].write(1, val); break;
	    case 0x102: timers[0].write(2, val); break;
	    case 0x103: return; break;
	    case 0x104: timers[1].write(0, val); break;
	    case 0x105: timers[1].write(1, val); break;
	    case 0x106: timers[1].write(2, val); break;
	    case 0x107: return; break;
	    case 0x108: timers[2].write(0, val); break;
	    case 0x109: timers[2].write(1, val); break;
	    case 0x10A: timers[2].write(2, val); break;
	    case 0x10B: return; break;
	    case 0x10C: timers[3].write(0, val); break;
	    case 0x10D: timers[3].write(1, val); break;
	    case 0x10E: timers[3].write(2, val); break;
	    case 0x10F: return; break;
	    default: cout << "Unrecognized timer write of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); break;
	}
    }

    void Timers::updatetimers()
    {
	int prevoverflows = updatetimer(0, 0);
	prevoverflows = updatetimer(1, prevoverflows);
	prevoverflows = updatetimer(2, prevoverflows);
	updatetimer(3, prevoverflows);
    }

    int Timers::updatetimer(int timer, int prevoverflows)
    {
	if (!TestBit(timers[timer].control, 7))
	{
	    return 0;
	}

	int newticks = 0;

	if (TestBit(timers[timer].control, 2))
	{
	    newticks = prevoverflows;
	}
	else
	{
	    timers[timer].cycles += 1;
	    int prescalar = divbits[(timers[timer].control & 0x3)];
	    newticks = timers[timer].cycles >> prescalar;
	    timers[timer].cycles &= (1 << prescalar) - 1;
	}

	if (newticks <= 0)
	{
	    return 0;
	}

	int ticksuntiloverflow = (0x10000 - timers[timer].counter);

	if (newticks < ticksuntiloverflow)
	{
	    timers[timer].counter += newticks;
	    return 0;
	}

	newticks -= ticksuntiloverflow;

	ticksuntiloverflow = (0x10000 - timers[timer].reload);
	timers[timer].counter = timers[timer].reload + newticks % ticksuntiloverflow;

	if (TestBit(timers[timer].control, 6))
	{
	    timerirq(timer);
	}

	int overflowcount = (1 + (newticks / ticksuntiloverflow));

	if (timer < 2)
	{
	    for (int i = 0; i < overflowcount; i++)
	    {
		timerapu.consumesamplefifoa(timer);
		timerapu.consumesamplefifob(timer);
	    }
	}

	return overflowcount;
    }
};
