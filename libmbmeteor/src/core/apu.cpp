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

#include "../../include/libmbmeteor/apu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    APU::APU(MMU& memory) : apumem(memory)
    {
	for (int i = 0x60; i < 0xA8; i++)
	{
	    apumem.addmemoryreadhandler((0x4000000 + i), bind(&APU::readapu, this, _1));
	    apumem.addmemorywritehandler((0x4000000 + i), bind(&APU::writeapu, this, _1, _2));
	}
    }

    APU::~APU()
    {

    }

    uint8_t APU::readapu(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFF))
	{
	    case 0x80: temp = 0; break;
	    case 0x81: temp = 0; break;
	    case 0x88: temp = (soundbias & 0xFF); break;
	    case 0x89: temp = (soundbias >> 8); break;
	    default: temp = 0x00; break;
	}

	return temp;
    }

    void APU::writeapu(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFF))
	{
	    case 0x80: return; break;
	    case 0x81: return; break;
	    case 0x82: writeratio(val); break;
	    case 0x83: writedsound(val); break;
	    case 0x84: writemaster(val); break;
	    case 0x85: return; break;
	    case 0x88: soundbias = ((soundbias & 0xFF00) | val); break;
	    case 0x89: soundbias = ((val << 8) | (soundbias & 0xFF)); break;
	    case 0xA0: writefifoa(0, val); break;
	    case 0xA1: writefifoa(1, val); break;
	    case 0xA2: writefifoa(2, val); break;
	    case 0xA3: writefifoa(3, val); break;
	    case 0xA4: writefifob(0, val); break;
	    case 0xA5: writefifob(1, val); break;
	    case 0xA6: writefifob(2, val); break;
	    case 0xA7: writefifob(3, val); break;
	    default: break;
	}
    }

    void APU::updatefifoa()
    {

    }

    void APU::updatefifob()
    {

    }

    void APU::updateapu()
    {
	apuclock += 1;

	if (apuclock == getsampleinterval())
	{
	    apuclock = 0;
	}

	nearestresample();
    }

    void APU::mixaudio()
    {
	int16_t sample[2] = {0, 0};
	int16_t finalsample[2] = {0, 0};

	if (dmasoundaleft && dmafifoarunning)
	{
	    sample[0] += getfifoavol();
	}

	if (dmasoundaright && dmafifoarunning)
	{
	    sample[1] += getfifoavol();
	}

	if (dmasoundbleft && dmafifobrunning)
	{
	    sample[0] += getfifobvol();
	}

	if (dmasoundbright && dmafifobrunning)
	{
	    sample[1] += getfifobvol();
	}

	for (int i = 0; i < 2; i++)
	{
	    int16_t tempsample = sample[i];
	    tempsample += (soundbias & 0x3FF);
	    clampaudio(tempsample, 0, 0x3FF);
	    tempsample -= 0x200;

	    float fsample = ((float)tempsample / 0x200);
	    finalsample[i] = (int16_t)(fsample * 28000);
	}

	if (apufunc)
	{
	    apufunc(finalsample[0], finalsample[1]);
	}
    }
};
