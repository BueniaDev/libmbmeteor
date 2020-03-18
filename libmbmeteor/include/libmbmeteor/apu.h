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

#ifndef LIBMBMETEOR_APU
#define LIBMBMETEOR_APU

#include "libmbmeteor_api.h"
#include "mmu.h"
#include <functional>
#include <queue>
using namespace gba;
using namespace std;

namespace gba
{
    using apuoutput = function<void(int16_t, int16_t)>;

    class LIBMBMETEOR_API APU
    {
	public:
	    APU(MMU& memory);
	    ~APU();

	    MMU& apumem;

	    uint8_t readapu(uint32_t addr);
	    void writeapu(uint32_t addr, uint8_t val);

	    vector<int8_t> buffer;

	    int apuclock = 0;
	    int8_t fifoasample = 0;
	    int8_t fifobsample = 0;

	    void nearestresample()
	    {
		if (samplecounter == maxsamples)
		{
		    samplecounter = 0;
		    mixaudio();
		}
		else
		{
		    samplecounter += 1;
		}
	    }

	    int samplecounter;
	    int maxsamples = (16780000 / 48000);

	    apuoutput apufunc;

	    void setaudiocallback(apuoutput cb)
	    {
		apufunc = cb;
	    }

	    void updateapu();
	    void mixaudio();

	    void updatefifoa();
	    void updatefifob();

	    bool dma1fiforec = false;

	    void consumesamplefifoa(int timer)
	    {
		if (!masterenable)
		{
		    return;
		}

		if ((!dmasoundaright) && (!dmasoundaleft))
		{
		    return;
		}

		if ((timer == 0) && (!dmasoundatimer))
		{
		    dmafifoarunning = false;
		    dma1fiforec = false;

		    if (fifoabuffer.size() > 0)
		    {
			fifoasample = fifoabuffer.front();
			fifoabuffer.pop();
			dmafifoarunning = true;
		    }

		    if (fifoabuffer.size() <= 16)
		    {
			apumem.dma1req = true;
			apumem.dma2req = true;
			apumem.signalfifo(1);
			apumem.signalfifo(2);
		    }

		    return;
		}

		if ((timer == 1) && (dmasoundatimer))
		{
		    dmafifoarunning = false;
		    dma1fiforec = false;

		    if (fifoabuffer.size() > 0)
		    {
			fifoasample = fifoabuffer.front();
			fifoabuffer.pop();
			dmafifoarunning = true;
		    }

		    if (fifoabuffer.size() <= 16)
		    {
			apumem.dma1req = true;
			apumem.dma2req = true;
			apumem.signalfifo(1);
			apumem.signalfifo(2);
		    }

		    return;
		}
	    }

	    void consumesamplefifob(int timer)
	    {
		if (!masterenable)
		{
		    return;
		}

		if ((!dmasoundbright) && (!dmasoundbleft))
		{
		    return;
		}

		if ((timer == 0) && (!dmasoundbtimer))
		{
		    dmafifobrunning = false;
		    dma1fiforec = false;

		    if (fifobbuffer.size() > 0)
		    {
			fifobsample = fifobbuffer.front();
			fifobbuffer.pop();
			dmafifobrunning = true;
		    }

		    if (fifobbuffer.size() <= 16)
		    {
			apumem.dma1req = true;
			apumem.dma2req = true;
			apumem.signalfifo(1);
			apumem.signalfifo(2);
		    }

		    return;
		}

		if ((timer == 1) && (dmasoundbtimer))
		{
		    dmafifobrunning = false;
		    dma1fiforec = false;

		    if (fifobbuffer.size() > 0)
		    {
			fifobsample = fifobbuffer.front();
			fifobbuffer.pop();
			dmafifobrunning = true;
		    }

		    if (fifobbuffer.size() <= 16)
		    {
			apumem.dma1req = true;
			apumem.dma2req = true;
			// apumem.signalfifo(1);
			apumem.signalfifo(2);
		    }

		    return;
		}
	    }

	    int16_t getfifoavol()
	    {
		return (fifoasample << (1 + dmasoundaratio));
	    }

	    int16_t getfifobvol()
	    {
		return (fifobsample << (1 + dmasoundbratio));
	    }

	    bool masterenable = false;
	    bool dmasoundaratio = false;
	    bool dmasoundaright = false;
	    bool dmasoundaleft = false;
	    bool dmasoundatimer = false;
	    bool dmasoundbratio = false;
	    bool dmasoundbright = false;
	    bool dmasoundbleft = false;
	    bool dmasoundbtimer = false;

	    bool dmafifoarunning = false;
	    bool dmafifobrunning = false;

	    uint16_t soundbias = 0x200;

	    queue<int8_t> fifoabuffer;
	    queue<int8_t> fifobbuffer;
	    queue<pair<int8_t, uint64_t>> fifoaplayback;

	    void writeratio(uint8_t val)
	    {
		dmasoundaratio = TestBit(val, 2);
		dmasoundbratio = TestBit(val, 3);
	    }

	    void writedsound(uint8_t val)
	    {
		dmasoundaright = TestBit(val, 0);
		dmasoundaleft = TestBit(val, 1);
		dmasoundatimer = TestBit(val, 2);

		dmasoundbright = TestBit(val, 4);
		dmasoundbleft = TestBit(val, 5);
		dmasoundbtimer = TestBit(val, 6);
		
		if (!fifoabuffer.empty() && TestBit(val, 3))
		{
		    cout << "Emptying queue..." << endl;
		    queue<int8_t> empty;
		    fifoabuffer = empty;
		}

		if (!fifobbuffer.empty() && TestBit(val, 7))
		{
		    cout << "Emptying queue..." << endl;
		    queue<int8_t> empty;
		    fifobbuffer = empty;
		}
	    }

	    void writemaster(uint8_t val)
	    {
		bool prevenable = masterenable;

		masterenable = TestBit(val, 7);

		if (!masterenable && prevenable)
		{
		    cout << "Disabling all sound..." << endl;
		    queue<int8_t> empty;
		    fifoabuffer = empty;
		    fifobbuffer = empty;
		    // exit(1);
		}
	    }

	    void writefifoa(size_t addr, uint8_t val)
	    {
		if (fifoabuffer.size() == 32)
		{
		    return;
		}

		if ((fifoabuffer.size() & 0x3) == addr)
		{
		    fifoabuffer.push((int8_t)(val));
		}
	    }

	    void writefifob(size_t addr, uint8_t val)
	    {
		if (fifobbuffer.size() == 32)
		{
		    return;
		}

		if ((fifobbuffer.size() & 0x3) == addr)
		{
		    fifobbuffer.push((int8_t)(val));
		}
	    }

	    int getsampleinterval()
	    {
		return (512 >> (soundbias >> 14));
	    }

	    void clampaudio(int16_t& sample, int min, int max)
	    {
		if (sample < min)
		{
		    sample = min;
		}
		else if (sample >= max)
		{
		    sample = max;
		}
	    }
    };
};

#endif // LIBMBMETEOR_APU
