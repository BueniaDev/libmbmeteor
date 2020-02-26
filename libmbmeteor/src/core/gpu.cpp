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

#include "../../include/libmbmeteor/gpu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
	GPU::GPU(MMU& memory) : gpumem(memory)
	{
		for (int i = 0; i < 0x60; i++)
		{
			gpumem.addmemoryreadhandler((0x4000000 + i), bind(&GPU::readlcd, this, _1));
			gpumem.addmemorywritehandler((0x4000000 + i), bind(&GPU::writelcd, this, _1, _2));
		}
	}
	
	GPU::~GPU()
	{
	
	}

	void GPU::init()
	{
	    for (int i = 0; i < (240 * 160); i++)
	    {
		framebuffer[i] = black;
	    }

	    cout << "GPU::Initialized" << endl;
	}

	void GPU::shutdown()
	{
	    cout << "GPU::Shutting down..." << endl;
	}
	
	uint8_t GPU::readlcd(uint32_t addr)
	{
	    uint8_t temp = 0;

	    switch ((addr & 0x5F))
	    {
		case 0x0: temp = (dispcnt & 0xFF); break;
		case 0x1: temp = (dispcnt >> 8); break;
		case 0x2: temp = 0x00; break;
		case 0x3: temp = 0x00; break;
		case 0x4: temp = (dispstat & 0xFF); break;
		case 0x5: temp = (dispstat >> 8); break;
		case 0x6: temp = (vcount & 0xFF); break;
		case 0x7: temp = (vcount >> 8); break;
		default: cout << "Unrecognized GPU read of " << hex << (int)((addr & 0x5F)) << endl; temp = 0xFF; break;
	    }

	    return temp;
	}
	
	void GPU::writelcd(uint32_t addr, uint8_t value)
	{
	    switch ((addr & 0x5F))
	    {
		case 0x0: dispcnt = ((dispcnt & 0xFF00) | value); break;
		case 0x1: dispcnt = ((value << 8) | (dispcnt & 0xFF)); break;
		case 0x2: return; break;
		case 0x3: return; break;
		case 0x4: dispstat = ((dispstat & 0xFF00) | (value & 0xF8)); break; // The bottom 3 bits of DISPSTAT are read-only
		case 0x5: dispstat = ((value << 8) | (dispstat & 0xFF)); break;
		case 0x6: return; break; // VCount should not be written to
		case 0x7: return; break; // VCount should not be written to
		case 0x8: bg0cnt = ((bg0cnt & 0xFF00) | value); break;
		case 0x9: bg0cnt = ((value << 8) | (bg0cnt & 0xFF)); break;
		case 0xA: bg1cnt = ((bg1cnt & 0xFF00) | value); break;
		case 0xB: bg1cnt = ((value << 8) | (bg1cnt & 0xFF)); break;
		case 0xC: bg2cnt = ((bg2cnt & 0xFF00) | value); break;
		case 0xD: bg2cnt = ((value << 8) | (bg2cnt & 0xFF)); break;
		case 0xE: bg3cnt = ((bg3cnt & 0xFF00) | value); break;
		case 0xF: bg3cnt = ((value << 8) | (bg3cnt & 0xFF)); break;
		case 0x10: bg0hoffs = ((bg0hoffs & 0x100) | value); break;
		case 0x11: bg0hoffs = (((value & 0x1) << 8) | (bg0hoffs & 0xFF)); break;
		case 0x12: bg0voffs = ((bg0voffs & 0x100) | value); break;
		case 0x13: bg0voffs = (((value & 0x1) << 8) | (bg0voffs & 0xFF)); break;
		case 0x14: bg1hoffs = ((bg1hoffs & 0x100) | value); break;
		case 0x15: bg1hoffs = (((value & 0x1) << 8) | (bg1hoffs & 0xFF)); break;
		case 0x16: bg1voffs = ((bg1voffs & 0x100) | value); break;
		case 0x17: bg1voffs = (((value & 0x1) << 8) | (bg1voffs & 0xFF)); break;
		case 0x18: bg2hoffs = ((bg2hoffs & 0x100) | value); break;
		case 0x19: bg2hoffs = (((value & 0x1) << 8) | (bg2hoffs & 0xFF)); break;
		case 0x1A: bg2voffs = ((bg2voffs & 0x100) | value); break;
		case 0x1B: bg2voffs = (((value & 0x1) << 8) | (bg2voffs & 0xFF)); break;
		case 0x1C: bg3hoffs = ((bg3hoffs & 0x100) | value); break;
		case 0x1D: bg3hoffs = (((value & 0x1) << 8) | (bg3hoffs & 0xFF)); break;
		case 0x1E: bg3voffs = ((bg3voffs & 0x100) | value); break;
		case 0x1F: bg3voffs = (((value & 0x1) << 8) | (bg3voffs & 0xFF)); break;
		case 0x50: blendcnt = ((blendcnt & 0xFF00) | value); break;
		case 0x51: blendcnt = (((value & 0x3F) << 8) | (blendcnt & 0xFF)); break;
		case 0x52: blendeva = (value & 0x1F); break;
		case 0x53: blendevb = (value & 0x1F); break;
		case 0x54: blendevy = (value & 0x1F); break;
		case 0x55: return; break;
		default: cout << "Unrecognized GPU write of " << hex << (int)((addr & 0x5F)) << endl; break;
	    }
	}
	
	void GPU::updatelcd()
	{
	    scanlinecounter += 1;
	    updatevcount();
	}
	
	void GPU::updatevcount()
	{
	    if (scanlinecounter == 1232)
	    {
		scanlinecounter = 0;
		vcount += 1;
		hblank(false);

		if (vcount == 160)
		{
		    vblank(true);
		}
		else if (vcount == 228)
		{
		    vblank(false);
		    vcount = 0;
		}
	    }
	    else if (scanlinecounter <= 960)
	    {
		if (scanlinecounter == 960)
		{
		    renderscanline();
		    hblank(true);
		}
	    }
	}

	void GPU::renderscanline()
	{
	    if (((dispcnt >> 8) & 0x1F) == 0)
	    {
		return;
	    }

	    int bgmode = (dispcnt & 0x7);

	    switch (bgmode)
	    {
		case 0: renderbgmode0(); break;
		case 1: renderbgmode1(); break;
		case 2: renderbgmode2(); break;
		case 3: renderbgmode3(); break;
		case 4: renderbgmode4(); break;
		case 5: renderbgmode5(); break;
		default: cout << "Unrecognized BG mode of " << hex << (int)(bgmode) << endl; exit(1); break;
	    }

	    rendercomposite();
	}

	void GPU::rendercomposite()
	{
	    uint16_t backcolor = (readpram16(0) & 0x7FFF);

	    for (int i = 0; i < 240; i++)
	    {
		uint16_t firstcolor = backcolor;
		uint16_t secondcolor = backcolor;

		int firstlayer = 3;
		int secondlayer = 3;

		int firstpriority = 3;
		int secondpriority = 3;

		for (int layer : bglayers)
		{
		    uint16_t layercolor = getpixel(layer, i);

		    if (layercolor == 0x8000)
		    {
			continue;
		    }

		    int layerpriority = (getbgcontrol(layer) & 0x3);

		    if (layerpriority <= firstpriority)
		    {
			secondcolor = firstcolor;
			secondlayer = firstlayer;
			secondpriority = firstpriority;

			firstcolor = layercolor;
			firstlayer = layer;
			firstpriority = layerpriority;
		    }
		    else if (layerpriority <= secondpriority)
		    {
			secondcolor = layercolor;
			secondlayer = layer;
			secondpriority = layerpriority;
		    }
		}

		int blendspec = ((blendcnt >> 6) & 0x3);

		switch (blendspec)
		{
		    case 0: break;
		    case 1:
		    {
			if (TestBit(blendcnt, firstlayer) && TestBit(blendcnt, (8 + secondlayer)))
			{
			    firstcolor = alphablendeffect(firstcolor, secondcolor);
			}
		    }
		    break;
		    case 2:
		    {
			if (TestBit(blendcnt, firstlayer))
			{
			    firstcolor = brightnesseffect(false, firstcolor);
			}
		    }
		    break;
		    case 3:
		    {
			if (TestBit(blendcnt, firstlayer))
			{
			    firstcolor = brightnesseffect(true, firstcolor);
			}
		    }
		    break;
		}

		uint16_t color = firstcolor;
		int red = ((color & 0x1F) << 3);
		int green = (((color >> 5) & 0x1F) << 3);
		int blue = (((color >> 10) & 0x1F) << 3);

		if (vcount >= 160)
		{
		    continue;
		}

		int temp = (i + (vcount * 240));
		framebuffer[temp].red = red;
		framebuffer[temp].green = green;
		framebuffer[temp].blue = blue;
		setpixel(firstlayer, i, 0x8000);
	    }
	}

	void GPU::renderbgmode0()
	{
	    renderbgreg(0);
	    renderbgreg(1);
	    renderbgreg(2);
	    renderbgreg(3);
	}

	void GPU::renderbgmode1()
	{
	    renderbgreg(0);
	    renderbgreg(1);
	    renderbgtrans(3);   
	}

	void GPU::renderbgmode2()
	{
	    renderbgtrans(0);
	    renderbgtrans(1);
	}

	void GPU::renderbgmode3()
	{
	    renderbgtrans(0);
	    renderbgtrans(1);
	    renderbitmap(2, 0);
	    renderbgtrans(3);
	}

	void GPU::renderbgmode4()
	{
	    renderbgtrans(0);
	    renderbgtrans(1);
	    renderbitmap(2, 1);
	    renderbgtrans(3);
	}

	void GPU::renderbgmode5()
	{
	    renderbgtrans(0);
	    renderbgtrans(1);
	    renderbitmap(2, 2);
	    renderbgtrans(3);
	}

	void GPU::renderbgreg(int layernum)
	{
	    if (!TestBit(dispcnt, (8 + layernum)))
	    {
		renderbgtrans(layernum);
		return;
	    }

	    uint16_t bgcnt = getbgcontrol(layernum);

	    int bghoffs = getbghoffs(layernum);
	    int bgvoffs = getbgvoffs(layernum);

	    int tile4bit = TestBit(bgcnt, 7) ? 0 : 1;
	    int tilesizeshift = (6 - tile4bit);

	    int size = (bgcnt >> 14);
	    int sbb = ((bgcnt >> 8) & 0x1F);
	    int cbb = ((bgcnt >> 2) & 0x3);

	    int totalwidth = ((256 << TestBit(size, 0)) - 1);
	    int totalheight = ((256 << TestBit(size, 1)) - 1);

	    int ypos = ((vcount + bgvoffs) & totalheight);
	    int bgmapbase = (sbb << 11);
	    int bgtilebase = (cbb << 14);

	    if (ypos & ~0xFF)
	    {
		ypos &= 0xFF;
		bgmapbase += 1024 << (totalwidth & ~0xFF ? 2 : 1);
	    }

	    int tilerow = ((ypos >> 3) << 5);
	    int tline = (ypos & 7);

	    for (int i = 0; i < 240; i++)
	    {
		int xpos = ((i + bghoffs) & totalwidth);

		int map = bgmapbase;

		if (xpos & ~255)
		{
		    xpos &= 255;
		    map += (1024 << 1);
		}

		int tilecol = (xpos >> 3);
		int tcol = (xpos & 7);

		int mapaddr = map + (tilerow + tilecol << 1);

		uint16_t tile = readvram16(mapaddr);

		int tilenum = (tile & 0x3FF);

		int samplecol = TestBit(tile, 10) ? (~tcol & 7) : tcol;
		int sampleline = TestBit(tile, 11) ? (~tline & 7) : tline;

		int tileaddr = bgtilebase + (tilenum << tilesizeshift) + ((sampleline << 3) + samplecol >> tile4bit);

		uint16_t paletteaddr = 0;

		if (TestBit(bgcnt, 7))
		{
		    int paletteindex = gpumem.vram[tileaddr];

		    if (paletteindex == 0)
		    {
			setpixel(layernum, i, 0x8000);
			continue;
		    }

		    paletteaddr = paletteindex;
		}
		else
		{
		    int paletteindex = gpumem.vram[tileaddr] >> (TestBit(samplecol, 0) << 2) & 0xF;

		    if (paletteindex == 0)
		    {
			setpixel(layernum, i, 0x8000);
			continue;
		    }

		    paletteaddr = (tile >> 8 & 0xF0) | paletteindex;
		}

		uint16_t color = (readpram16(paletteaddr) & 0x7FFF);
		setpixel(layernum, i, color);
	    }
	}

	void GPU::renderbitmap(int layernum, int mode)
	{
	    if (!TestBit(dispcnt, (8 + layernum)))
	    {
		return;
	    }

	    for (int i = 0; i < 240; i++)
	    {
		uint16_t color = 0x7FFF;

		switch (mode)
		{
		    case 0:
		    {
			int pixeltemp = ((i * 2) + (vcount * 480));

			if (pixeltemp >= 0x12C00)
			{
			    return;
			}

			color = readvram16(pixeltemp);
		    }
		    break;
		    case 1:
		    {
			uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
			int pixeltemp = (i + (vcount * 240));

			if (pixeltemp >= 0x9600)
			{
			    return;
			}

			uint8_t pramcolor = gpumem.vram[(pixeltemp + frameoffs)];

			if (pramcolor == 0)
			{
			    return;
			}

			color = readpram16(pramcolor);
		    }
		    break;
		    case 2:
		    {
			uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
			int pixeltemp = ((i * 2) + (vcount * 320));

			if (pixeltemp >= 0xA000)
			{
			    return;
			}

			if ((i >= 160) || (vcount >= 128))
			{
			    return;
			}

			color = readvram16((pixeltemp + frameoffs));
		    }
		    break;
		}

		setpixel(layernum, i, color);
	    }
	}

	void GPU::renderbgtrans(int layernum)
	{
	    for (int i = 0; i < 240; i++)
	    {
		setpixel(layernum, i, 0x8000);
	    }
	}
};
