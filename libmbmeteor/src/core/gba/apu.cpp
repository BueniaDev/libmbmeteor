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

#include "../../../include/libmbmeteor/gba/apu.h"
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
	    case 0x60: temp = s1sweep; break;
	    case 0x62: temp = s1length; break;
	    case 0x63: temp = s1envelope; break;
	    case 0x65: temp = (s1freqhi & 0x40); break;
	    case 0x78: temp = noiselength; break;
	    case 0x79: temp = noiseenvelope; break;
	    case 0x7C: temp = noisefreqlo; break;
	    case 0x7D: temp = (noisefreqhi & 0x40); break;
	    case 0x80: temp = mastervolume; break;
	    case 0x81: temp = soundselect; break;
	    case 0x88: temp = (soundbias & 0xFF); break;
	    case 0x89: temp = (soundbias >> 8); break;
	    default: temp = 0xFF; break;
	}

	return temp;
    }

    void APU::writeapu(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFF))
	{
	    case 0x60: writes1sweep(val); break;
	    case 0x62:
	    {
		s1length = val;
		reloads1lengthcounter();
		sets1dutycycle();
	    }
	    break;
	    case 0x63:
	    {
		s1envelope = val;

		if (((s1envelope & 0xF0) >> 4) == 0)
		{
		    s1enabled = false;
		}
	    }
	    break;
	    case 0x64: s1freqlo = val; break;
	    case 0x65: s1writereset(val); break;
	    case 0x68:
	    {
		s2length = val;
		reloads2lengthcounter();
		sets2dutycycle();
	    }
	    break;
	    case 0x69:
	    {
		s2envelope = val;

		if (((s2envelope & 0xF0) >> 4) == 0)
		{
		    s2enabled = false;
		}
	    }
	    break;
	    case 0x6C: s2freqlo = val; break;
	    case 0x6D: s2writereset(val); break;
	    case 0x78:
	    {
		noiselength = (val & 0x3F);
		reloadnoiselengthcounter();
	    }
	    break;
	    case 0x79: writenoiseenvelope(val); break;
	    case 0x7A: return; break;
	    case 0x7B: return; break;
	    case 0x7C: noisefreqlo = val; break;
	    case 0x7D: noisewritereset(val); break;
	    case 0x7E: return; break;
	    case 0x7F: return; break;
	    case 0x80: mastervolume = val; break;
	    case 0x81: soundselect = val; break;
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

    void APU::s1sweeptick(int frameseq)
    {
	bool sweepinc = TestBit(frameseq, 1);

	if (s1sweepenabled)
	{
	    if (!sweepinc && prevs1sweepinc)
	    {
		s1sweepcounter -= 1;

		if (s1sweepcounter == 0)
	    	{
		    s1shadowfreq = s1sweepcalc();
		    s1freqlo = (s1shadowfreq & 0xFF);
		    s1freqhi &= 0xF8;
		    s1freqhi |= ((s1shadowfreq & 0x0700) >> 8);

		    s1sweepcalc();
		    s1sweepcounter = (((s1sweep & 0x70) >> 4) + 1);
	    	}
	    }
    	}

	prevs1sweepinc = sweepinc;
    }
	
    void APU::s1lengthcountertick(int frameseq)
    {
	bool lengthcounterdec = TestBit(frameseq, 0);

	if (TestBit(s1freqhi, 6) && s1lengthcounter > 0)
	{
	    if (!lengthcounterdec && prevs1lengthdec)
	    {
		s1lengthcounter -= 1;

		if (s1lengthcounter == 0)
		{
		    s1enabled = false;
		}
	    }
	}

	prevs1lengthdec = lengthcounterdec;
    }
	
    void APU::s1envelopetick(int frameseq)
    {
	bool envelopeinc = TestBit(frameseq, 2);

	if (s1envelopeenabled)
	{
	    if (!envelopeinc && prevs1envelopeinc)
	    {
		s1envelopecounter -= 1;

		if (s1envelopecounter == 0)
		{
		    if (!TestBit(s1envelope, 3))
		    {
			s1volume -= 1;

			if (s1volume == 0)
			{
			    s1envelopeenabled = false;
			}
		    }
		    else
		    {
			s1volume += 1;
			if (s1volume == 0x0F)
			{
			    s1envelopeenabled = false;
			}
		    }

		    s1envelopecounter = (s1envelope & 0x7);
		}
	    }
	}

	prevs1envelopeinc = envelopeinc;
    }
	
    inline void APU::s1timertick()
    {
	if (s1periodtimer == 0)
	{
	    s1seqpointer = ((s1seqpointer + 1) & 7);
	    s1reloadperiod();
	}
	else
	{
	    s1periodtimer -= 1;
	}
    }
	
    void APU::s1update(int frameseq)
    {
	s1sweeptick(frameseq);
	s1timertick();
	s1lengthcountertick(frameseq);
	s1envelopetick(frameseq);
    }
	
    float APU::gets1outputvol()
    {
	int outputvol = 0;
	outputvol = (s1dutycycle[s1seqpointer] * s1volume);

	return ((float)(outputvol) / 100.f);
    }
	
    void APU::s2lengthcountertick(int frameseq)
    {
	bool lengthcounterdec = TestBit(frameseq, 0);

	if (TestBit(s2freqhi, 6) && s2lengthcounter > 0)
	{
	    if (!lengthcounterdec && prevs2lengthdec)
	    {
		s2lengthcounter -= 1;

		if (s2lengthcounter == 0)
		{
		    s2enabled = false;
		}
	    }
	}

	prevs2lengthdec = lengthcounterdec;
    }
	
    void APU::s2envelopetick(int frameseq)
    {
	bool envelopeinc = TestBit(frameseq, 2);

	if (s2envelopeenabled)
	{
	    if (!envelopeinc && prevs2envelopeinc)
	    {
		s2envelopecounter -= 1;

		if (s2envelopecounter == 0)
		{
		    if (!TestBit(s2envelope, 3))
		    {
			s2volume -= 1;

			if (s2volume == 0)
			{
			    s2envelopeenabled = false;
			}
		    }
		    else
		    {
			s2volume += 1;
			if (s2volume == 0x0F)
			{
			    s2envelopeenabled = false;
			}
		    }

		    s2envelopecounter = (s2envelope & 0x7);
		}
	    }
	}

	prevs2envelopeinc = envelopeinc;
    }
	
    inline void APU::s2timertick()
    {
	if (s2periodtimer == 0)
	{
	    s2seqpointer = ((s2seqpointer + 1) & 7);
	    s2reloadperiod();
	}
	else
	{
	    s2periodtimer -= 1;
	}
    }
	
    void APU::s2update(int frameseq)
    {
	s2timertick();
	s2lengthcountertick(frameseq);
	s2envelopetick(frameseq);
    }
	
    float APU::gets2outputvol()
    {
	int outputvol = 0;
	outputvol = (s2dutycycle[s2seqpointer] * s2volume);

	return ((float)(outputvol) / 100.f);
    }

    void APU::noiselengthcountertick(int frameseq)
    {
	bool lengthcounterdec = TestBit(frameseq, 0);

	if (TestBit(noisefreqhi, 6) && noiselengthcounter > 0)
	{
	    if (!lengthcounterdec && prevnoiselengthdec)
	    {
		noiselengthcounter -= 1;

		if (noiselengthcounter == 0)
		{
		    noiseenabled = false;
		}
	    }
	}

	prevnoiselengthdec = lengthcounterdec;
    }

    void APU::noiseenvelopetick(int frameseq)
    {
	bool envelopeinc = TestBit(frameseq, 2);

	if (noiseenvelopeenabled)
	{
	    if (!envelopeinc && prevnoiseenvelopeinc)
	    {
		noiseenvelopecounter -= 1;

		if (noiseenvelopecounter == 0)
		{
		    if (!TestBit(noiseenvelope, 3))
		    {
			noisevolume -= 1;

			if (noisevolume == 0)
			{
			    noiseenvelopeenabled = false;
			}
		    }
		    else
		    {
			noisevolume += 1;

			if (noisevolume == 0x0F)
			{
			    noiseenvelopeenabled = false;
			}
		    }

		    noiseenvelopecounter = (noiseenvelope & 0x7);
		}
	    }
	}

	prevnoiseenvelopeinc = envelopeinc;
    }

    void APU::noisetimertick()
    {
	if (noiseperiodtimer == 0)
	{
	    if (((noisefreqlo & 0xF0) >> 4) < 14)
	    {
		int xoredbits = ((noiselfsr ^ (noiselfsr >> 1)) & 1);
		noiselfsr >>= 1;
		noiselfsr |= (xoredbits << 14);

		if (TestBit(noisefreqlo, 3))
		{
		    noiselfsr = BitReset(noiselfsr, 6);
		    noiselfsr |= (xoredbits << 6);
		}
	    }

	    noisereloadperiod();
	}
	else
	{
	    noiseperiodtimer -= 1;
	}
    }
	
    void APU::noiseupdate(int frameseq)
    {
	noisetimertick();
	noiselengthcountertick(frameseq);
	noiseenvelopetick(frameseq);
    }

    float APU::getnoiseoutputvol()
    {
	int outputvol = 0;
	if (noiseenabled)
	{
	    outputvol = (TestBit((~noiselfsr), 0) * noisevolume);
	}
	else
	{
	    outputvol = 0;
	}

	return ((float)(outputvol) / 100.f);
    }

    void APU::updateapu()
    {
	apuclock += 1;

	if (apuclock == 8)
	{
	    frametimer += 1;
	    apuclock = 0;
	    s1update(getframesequencer());
	    s2update(getframesequencer());
	    noiseupdate(getframesequencer());
	}

	nearestresample();
    }

    void APU::mixaudio()
    {
	static constexpr float ampl = 8000;

	auto sound1 = gets1outputvol();
	auto sound2 = gets2outputvol();
	auto sound4 = getnoiseoutputvol();

        float leftsample = 0;
        float rightsample = 0;

	if (s1enabledleft())
        {
	    leftsample += sound1;
        }

	if (s2enabledleft())
        {
	    leftsample += sound2;
        }

	if (noiseenabledleft())
	{
	    leftsample += sound4;
	}

	if (s1enabledright())
        {
	    rightsample += sound1;
        }

	if (s2enabledright())
        {
	    rightsample += sound2;
        }

	if (noiseenabledright())
	{
	    rightsample += sound4;
	}

	int mastervolleft = ((mastervolume >> 4) & 0x7);
	int mastervolright = (mastervolume & 0x7);

	auto leftvolume = (((float)(mastervolleft)) / 7.0f);
	auto rightvolume = (((float)(mastervolright)) / 7.0f);

	auto left = (int16_t)(leftsample * leftvolume * ampl);
	auto right = (int16_t)(rightsample * rightvolume * ampl);

	left *= psgvolume;
	right *= psgvolume;

	left /= 28;
	right /= 28;

	int16_t sample[2] = {left, right};
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

	int16_t tempsample = sample[0];
	tempsample += (soundbias & 0x3FF);
	clampaudio(tempsample, 0, 0x3FF);
	tempsample -= 0x200;

	float fsample = ((float)tempsample / 0x200);
	finalsample[0] = (int16_t)(fsample * 30000);

	tempsample = sample[1];
	tempsample += (soundbias & 0x3FF);
	clampaudio(tempsample, 0, 0x3FF);
	tempsample -= 0x200;

	fsample = ((float)tempsample / 0x200);
	finalsample[1] = (int16_t)(fsample * 30000);


	if (apufunc)
	{
	    apufunc(finalsample[0], finalsample[1]);
	}
    }
};
