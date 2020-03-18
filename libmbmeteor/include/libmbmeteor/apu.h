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

	    int psgvolume = 0;

	    int frametimer = 0;
	    uint8_t s1sweep = 0;
	    uint8_t s1length = 0;
	    int s1lengthcounter = 0;
	    uint8_t s1envelope = 0;
	    uint8_t s1freqlo = 0;
	    uint8_t s1freqhi = 0;
	    array<int, 8> s1dutycycle;
	    
	    bool s1enabled = false;
	    bool s1sweepenabled = false;
	    bool s1envelopeenabled = false;
	    bool s1negative = false;
	    uint16_t s1shadowfreq = 0;
	    int s1sweepcounter = 0;
	    int s1envelopecounter = 0;
	    int s1volume = 0;
	    int s1periodtimer = 0;
	    int s1seqpointer = 0;
	    bool prevs1sweepinc = false;
	    bool prevs1lengthdec = false;
	    bool prevs1envelopeinc = false;

	    inline void s1update(int frameseq);
	    void s1sweeptick(int frameseq);
	    void s1lengthcountertick(int frameseq);
	    void s1envelopetick(int frameseq);
	    void s1timertick();
	    float gets1outputvol();

	    int mastervolume = 0;
	    uint8_t soundselect = 0;

	    inline int getframesequencer()
	    {
		return (frametimer >> 13);
	    }

	    inline bool apulengthlow()
	    {
		return !TestBit(getframesequencer(), 0);
	    }

	    inline void writes1sweep(uint8_t value)
	    {
		s1sweep = (value & 0x7F);

		if ((((s1sweep & 0x70) >> 4) == 0) || ((s1sweep & 0x07) == 0) || (!TestBit(s1sweep, 3) && s1negative))
		{
		    s1sweepenabled = false;
		}
	    }

	    inline void reloads1lengthcounter()
	    {
		s1lengthcounter = (64 - (s1length & 0x3F));
		s1length &= 0xC0;	
	    }

	    inline void sets1dutycycle()
	    {
		switch ((s1length & 0xC0) >> 6)
		{
		    case 0: s1dutycycle = {{false, false, false, false, false, false, false, true}}; break;
		    case 1: s1dutycycle = {{true, false, false, false, false, false, false, true}}; break;
		    case 2: s1dutycycle = {{true, false, false, false, false, true, true, true}}; break;
		    case 3: s1dutycycle = {{false, true, true, true, true, true, true, false}}; break;
		    default: break;
		}
	    }

	    inline bool s1enabledleft()
	    {
		return (s1enabled && (TestBit(soundselect, 4)));
	    }

	    inline bool s1enabledright()
	    {
		return (s1enabled && (TestBit(soundselect, 0)));
	    }

	    inline void s1writereset(uint8_t value)
	    {
		bool lengthwasenable = TestBit(s1freqhi, 6);
		s1freqhi = (value & 0xC7);

		if (apulengthlow() && !lengthwasenable && TestBit(s1freqhi, 6) && s1lengthcounter > 0)
		{
		    s1lengthcounter -= 1;

		    if (s1lengthcounter == 0)
		    {
			s1enabled = false;
		    }
		}

		if (TestBit(s1freqhi, 7))
		{
		    s1resetchannel();
		}
	    }

	    inline uint16_t s1sweepcalc()
	    {
		uint16_t freqdelta = (s1shadowfreq >> (s1sweep & 0x07));

		if (TestBit(s1sweep, 3))
		{
		    freqdelta *= -1;
		    freqdelta &= 0x7FF;

		    s1negative = true;
		}

		uint16_t newfreq = ((s1shadowfreq + freqdelta) & 0x7FF);

		if (newfreq > 2047)
		{
		    s1sweepenabled = false;
		    s1enabled = false;
		}

		return newfreq;
	    }

	    inline void s1resetchannel()
	    {
		s1enabled = true;
		s1reloadperiod();
		s1freqhi &= 0x7F;

		s1shadowfreq = (s1freqlo | ((s1freqhi & 0x7) << 8));
		s1sweepcounter = ((s1sweep & 0x70) >> 4);
		s1sweepenabled = (s1sweepcounter != 0 && ((s1sweep & 0x07) != 0));
		s1sweepcalc();

		s1negative = false;

		s1volume = ((s1envelope & 0xF0) >> 4);
		s1envelopecounter = (s1envelope & 0x07);
		s1envelopeenabled = (s1envelopecounter != 0);


		if ((!TestBit(s1envelope, 3) && s1volume == 0) || (TestBit(s1envelope, 3) && s1volume == 0x0F))
		{
		    s1envelopeenabled = false;
		}

		if (s1lengthcounter == 0)
		{
		    s1lengthcounter = 64;

		    if (apulengthlow() && TestBit(s1freqhi, 6))
		    {
			s1lengthcounter -= 1;
		    }
		}
	    }
		
	    inline void s1reloadperiod()
	    {
		int frequency = (s1freqlo | ((s1freqhi & 0x07) << 8));
		s1periodtimer = ((2048 - frequency) << 1);
	    }

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
		switch ((val & 0x3))
		{
		    case 0: psgvolume = 1; break;
		    case 1: psgvolume = 2; break;
		    case 2: psgvolume = 4; break;
		    case 3: psgvolume = 0; break; 
		}

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
