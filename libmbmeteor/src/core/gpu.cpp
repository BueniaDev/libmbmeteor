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

	    for (int i = 0; i < 240; i++)
	    {
		scanlinebuffer[i] = black;
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
		case 0x6: return; break; // VCount should not be written to
		case 0x7: return; break; // VCount should not be written to
		case 0x8: bg0cnt = ((bg0cnt & 0xFF00) | value); break;
		case 0x9: bg0cnt = ((value << 8) | (bg0cnt & 0xFF)); break;
		case 0x10: bg0hoffs = ((bg0hoffs & 0x100) | value); break;
		case 0x11: bg0hoffs = (((value & 0x1) << 8) | (bg0hoffs & 0xFF)); break;
		case 0x12: bg0voffs = ((bg0voffs & 0x100) | value); break;
		case 0x13: bg0voffs = (((value & 0x1) << 8) | (bg0voffs & 0xFF)); break;
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

		if (vcount == 228)
		{
		    vcount = 0;
		}
	    }
	    else if (scanlinecounter <= 960)
	    {
		if ((scanlinecounter % 4) == 0)
		{
		    renderpixel();
		}

		if (scanlinecounter == 960)
		{
		    renderscanline();
		}
	    }
	}

	void GPU::renderpixel()
	{
	    if (((dispcnt >> 8) & 0x1F) == 0)
	    {
		return;
	    }

	    int tempdisp = (dispcnt & 0x7);

	    switch (tempdisp)
	    {
		case 0: renderbgmode0(); break; // TODO: BG Mode 0
		case 3: renderbgmode3(); break;
		case 4: renderbgmode4(); break;
		case 5: renderbgmode5(); break;
		default: cout << "Unrecognized BG mode of " << dec << (int)(tempdisp) << endl; exit(1); break;
	    }

	    pixelx += 1;
	}

	void GPU::renderscanline()
	{
	    for (int i = 0; i < 240; i++)
	    {
		if (vcount >= 160)
		{
		    continue;
		}

		int pixeltemp = (i + (vcount * 240));
		framebuffer[pixeltemp] = scanlinebuffer[i];
		scanlinebuffer[i] = black;
	    }

	    pixelx = 0;
	}

	void GPU::rendercomposite()
	{
	    for (int i = 0; i < 1; i++)
	    {
		uint16_t tempcolor = getpixel(i, pixelx);

		if (tempcolor == 0x8000)
		{
		    scanlinebuffer[pixelx] = black;
		    continue;
		}

		uint16_t color = (tempcolor & 0x7FFF);

	    	int red = ((color & 0x1F) << 3);
	    	int green = (((color >> 5) & 0x1F) << 3);
	    	int blue = (((color >> 10) & 0x1F) << 3);

	    	scanlinebuffer[pixelx].red = red;
	    	scanlinebuffer[pixelx].green = green;
	    	scanlinebuffer[pixelx].blue = blue;
	    }
	}

	void GPU::renderbgmode0()
	{
	    renderbg(0);
	    rendercomposite();
	}

	void GPU::renderbg(int layernum)
	{
	    if (!TestBit(dispcnt, (8 + layernum)))
	    {
		return;
	    }

	    uint16_t bgcnt = getbgcontrol(layernum);
	    int size = (bgcnt >> 14);
	    int sbb = ((bgcnt >> 8) & 0x1F);
	    int cbb = ((bgcnt >> 2) & 0x3);

	    int bghoffs = getbghoffs(layernum);
	    int bgvoffs = getbgvoffs(layernum);

	    int totalwidth = ((256 << TestBit(size, 0)) - 1);
	    int totalheight = ((256 << TestBit(size, 1)) - 1);

	    int scy = ((vcount + bgvoffs) & totalheight);
	    int bgmapbase = (sbb << 11);
	    int bgtilebase = (cbb << 14);

	    if (scy & ~0xFF)
	    {
		scy &= 0xFF;
		bgmapbase += (1024 << ((totalwidth & ~255) ? 2 : 1));
	    }

	    int scx = ((pixelx + bghoffs) & totalwidth);
	    int tx = (scx & 7);
	    int ty = (scy & 7);

	    if (scx & ~0xFF)
	    {
		scx &= 0xFF;
		bgmapbase += (1024 << 1);
	    }

	    if ((tx == 0) || (pixelx == 0))
	    {
	        uint16_t tilenum = readtilegba(bgmapbase, scx, scy);
		setbgdata(layernum, tilenum);
	    }

	    uint16_t tile = getbgdata(layernum);

	    uint16_t color = getgbacolor(layernum, tile, bgtilebase, tx, ty);

	    setpixel(layernum, pixelx, color);
	}

	void GPU::renderbgmode3()
	{
	    int colortemp = ((pixelx * 2) + (vcount * 480));

	    if (colortemp >= 0x12C00)
	    {
		return;
	    }

	    uint16_t color = readvram16(colortemp);

	    int red = ((color & 0x1F) << 3);
	    int green = (((color >> 5) & 0x1F) << 3);
	    int blue = (((color >> 10) & 0x1F) << 3);

	    scanlinebuffer[pixelx].red = red;
	    scanlinebuffer[pixelx].green = green;
	    scanlinebuffer[pixelx].blue = blue;
	}

	void GPU::renderbgmode4()
	{
	    uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
	    int pixeltemp = (pixelx + (vcount * 240));

	    if (pixeltemp >= 0x9600)
	    {
		return;
	    }

	    uint8_t pramcolor = gpumem.vram[(pixeltemp + frameoffs)];
		
	    if (pramcolor == 0)
	    {
		return;
	    }

	    uint16_t color = readpram16(pramcolor);
	    int red = ((color & 0x1F) << 3);
	    int green = (((color >> 5) & 0x1F) << 3);
	    int blue = (((color >> 10) & 0x1F) << 3);

	    scanlinebuffer[pixelx].red = red;
	    scanlinebuffer[pixelx].green = green;
	    scanlinebuffer[pixelx].blue = blue;
	}

	void GPU::renderbgmode5()
	{
	    uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
	    int pixeltemp = ((pixelx * 2) + (vcount * 320));

	    if (pixeltemp >= 0xA000)
	    {
		return;
	    }

	    if ((pixelx >= 160) || (vcount >= 128))
	    {
		return;
	    }

	    uint16_t color = readvram16((pixeltemp + frameoffs));

	    int red = ((color & 0x1F) << 3);
	    int green = (((color >> 5) & 0x1F) << 3);
	    int blue = (((color >> 10) & 0x1F) << 3);

	    scanlinebuffer[pixelx].red = red;
	    scanlinebuffer[pixelx].green = green;
	    scanlinebuffer[pixelx].blue = blue;
	}
};
