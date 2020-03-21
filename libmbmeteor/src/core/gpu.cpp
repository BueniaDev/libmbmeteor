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

	    phase = Phase::Scanline;

	    cout << "GPU::Initialized" << endl;
	}

	void GPU::shutdown()
	{
	    cout << "GPU::Shutting down..." << endl;
	}
	
	uint8_t GPU::readlcd(uint32_t addr)
	{
	    uint8_t temp = 0;

	    switch ((addr & 0xFF))
	    {
		case 0x0: temp = (dispcnt & 0xFF); break;
		case 0x1: temp = (dispcnt >> 8); break;
		case 0x2: temp = 0x00; break;
		case 0x3: temp = 0x00; break;
		case 0x4: temp = (dispstat & 0xFF); break;
		case 0x5: temp = (dispstat >> 8); break;
		case 0x6: temp = (vcount & 0xFF); break;
		case 0x7: temp = (vcount >> 8); break;
		case 0x8: temp = (bg0cnt & 0xFF); break;
		case 0x9: temp = (bg0cnt >> 8); break;
		case 0xA: temp = (bg1cnt & 0xFF); break;
		case 0xB: temp = (bg1cnt >> 8); break;
		case 0xC: temp = (bg2cnt & 0xFF); break;
		case 0xD: temp = (bg2cnt >> 8); break;
		case 0xE: temp = (bg3cnt & 0xFF); break;
		case 0xF: temp = (bg3cnt >> 8); break;
		case 0x48: temp = winctrl[0].read(); break;
		case 0x49: temp = winctrl[1].read(); break;
		case 0x4A: temp = winctrl[2].read(); break;
		case 0x4B: temp = winctrl[3].read(); break;
		default: temp = 0xFF; break;
	    }

	    return temp;
	}
	
	void GPU::writelcd(uint32_t addr, uint8_t value)
	{
	    switch ((addr & 0xFF))
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
		case 0x20: bgaff[0].pa = ((bgaff[0].pa & 0xFF00) | value); break;
		case 0x21: bgaff[0].pa = ((bgaff[0].pa & 0x00FF) | (value << 8)); break;
		case 0x22: bgaff[0].pb = ((bgaff[0].pb & 0xFF00) | value); break;
		case 0x23: bgaff[0].pb = ((bgaff[0].pb & 0x00FF) | (value << 8)); break;
		case 0x24: bgaff[0].pc = ((bgaff[0].pc & 0xFF00) | value); break;
		case 0x25: bgaff[0].pc = ((bgaff[0].pc & 0x00FF) | (value << 8)); break;
		case 0x26: bgaff[0].pd = ((bgaff[0].pd & 0xFF00) | value); break;
		case 0x27: bgaff[0].pd = ((bgaff[0].pd & 0x00FF) | (value << 8)); break;
		case 0x28: bgaff[0].x = ((bgaff[0].x & 0x0FFFFF00) | value); break;
		case 0x29: bgaff[0].x = ((bgaff[0].x & 0x0FFF00FF) | (value << 8)); break;
		case 0x2A: bgaff[0].x = ((bgaff[0].x & 0x0F00FFFF) | (value << 16)); break;
		case 0x2B: bgaff[0].x = ((bgaff[0].x & 0x00FFFFFF) | ((value & 0xF) << 24)); setcx(0); break;
		case 0x2C: bgaff[0].y = ((bgaff[0].y & 0x0FFFFF00) | value); break;
		case 0x2D: bgaff[0].y = ((bgaff[0].y & 0x0FFF00FF) | (value << 8)); break;
		case 0x2E: bgaff[0].y = ((bgaff[0].y & 0x0F00FFFF) | (value << 16)); break;
		case 0x2F: bgaff[0].y = ((bgaff[0].y & 0x00FFFFFF) | ((value & 0xF) << 24)); setcy(0); break;
		case 0x30: bgaff[1].pa = ((bgaff[1].pa & 0xFF00) | value); break;
		case 0x31: bgaff[1].pa = ((bgaff[1].pa & 0x00FF) | (value << 8)); break;
		case 0x32: bgaff[1].pb = ((bgaff[1].pb & 0xFF00) | value); break;
		case 0x33: bgaff[1].pb = ((bgaff[1].pb & 0x00FF) | (value << 8)); break;
		case 0x34: bgaff[1].pc = ((bgaff[1].pc & 0xFF00) | value); break;
		case 0x35: bgaff[1].pc = ((bgaff[1].pc & 0x00FF) | (value << 8)); break;
		case 0x36: bgaff[1].pd = ((bgaff[1].pd & 0xFF00) | value); break;
		case 0x37: bgaff[1].pd = ((bgaff[1].pd & 0x00FF) | (value << 8)); break;
		case 0x38: bgaff[1].x = ((bgaff[1].x & 0x0FFFFF00) | value); break;
		case 0x39: bgaff[1].x = ((bgaff[1].x & 0x0FFF00FF) | (value << 8)); break;
		case 0x3A: bgaff[1].x = ((bgaff[1].x & 0x0F00FFFF) | (value << 16)); break;
		case 0x3B: bgaff[1].x = ((bgaff[1].x & 0x00FFFFFF) | ((value & 0xF) << 24)); setcx(1); break;
		case 0x3C: bgaff[1].y = ((bgaff[1].y & 0x0FFFFF00) | value); break;
		case 0x3D: bgaff[1].y = ((bgaff[1].y & 0x0FFF00FF) | (value << 8)); break;
		case 0x3E: bgaff[1].y = ((bgaff[1].y & 0x0F00FFFF) | (value << 16)); break;
		case 0x3F: bgaff[1].y = ((bgaff[1].y & 0x00FFFFFF) | ((value & 0xF) << 24)); setcy(1); break;
		case 0x40: winsize[0].write(0, value); break;
		case 0x41: winsize[0].write(1, value); break;
		case 0x42: winsize[1].write(0, value); break;
		case 0x43: winsize[1].write(1, value); break;
		case 0x44: winsize[0].write(2, value); break;
		case 0x45: winsize[0].write(3, value); break;
		case 0x46: winsize[1].write(2, value); break;
		case 0x47: winsize[1].write(3, value); break;
		case 0x48: winctrl[0].write(value); break;
		case 0x49: winctrl[1].write(value); break;
		case 0x4A: winctrl[2].write(value); break;
		case 0x4B: winctrl[3].write(value); break;
		case 0x4C: bgmosaic = value; break;
		case 0x4D: objmosaic = value; break;		
		case 0x50: blendcnt = ((blendcnt & 0xFF00) | value); break;
		case 0x51: blendcnt = (((value & 0x3F) << 8) | (blendcnt & 0xFF)); break;
		case 0x52: blendeva = (value & 0x1F); break;
		case 0x53: blendevb = (value & 0x1F); break;
		case 0x54: blendevy = (value & 0x1F); break;
		case 0x55: return; break;
		default: break;
	    }
	}
	
	void GPU::updatelcd()
	{
	    scanlinecounter += 1;
	    updatevcount();
	}
	
	void GPU::updatevcount()
	{
	    switch (phase)
	    {
		case Phase::Scanline:
		{
		    if (scanlinecounter == 1006)
		    {
			phase = Phase::HBlank;
			hblank(true);
			gpumem.signalhblank();
		    }
		}
		break;
		case Phase::HBlank:
		{
		    if (scanlinecounter == 1232)
		    {
			scanlinecounter = 0;
			hblank(false);

			bool vcountflag = (++vcount == (dispstat >> 8));
			vcountset(vcountflag);

			if (vcount == 160)
			{
			    phase = Phase::VBlankScanline;
			    affvblank();
			    vblank(true);
			    drawpixels();
			}
			else
			{
			    phase = Phase::Scanline;
			    renderscanline();
			    affhblank();
			}
		    }
		}
		break;
		case Phase::VBlankScanline:
		{
		    if (scanlinecounter == 1006)
		    {
			phase = Phase::VBlankHBlank;
			hblank(true);
		    }
		}
		break;
		case Phase::VBlankHBlank:
		{
		    if (scanlinecounter == 1232)
		    {
			scanlinecounter = 0;

			hblank(false);

			if (vcount == 227)
			{
			    vcount = 0;
			    vcountset((dispstat >> 8) == 0);
			    phase = Phase::Scanline;
			    renderscanline();
			}
			else
			{
			    phase = Phase::VBlankScanline;

			    if (vcount == 226)
			    {
				vblank(false);
			    }

			    vcountset((++vcount == (dispstat >> 8)));
			}
		    }
		}
		break;
	    }
	}

	void GPU::renderscanline()
	{
	    if ((dispcnt >> 8) == 0)
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
		
	    renderobjects();
	    rendercomposite();
	}
	
	void GPU::renderobjects()
	{
		for (int i = 0; i < 240; i++)
		{
			objbuffer[i] = 0x8000;
			objinfobuffer[i] = 3;
		}
		
		if (!TestBit(dispcnt, 12))
		{
			return;
		}
		
		int tilebase = 0x10000;
		
		if ((dispcnt & 0x7) >= 3)
		{
			tilebase += 0x4000;
		}
		
		for (int i = 127; i >= 0; i--)
		{
			int attribaddr = (i << 3);
			uint16_t attrib0 = readoam16(attribaddr);
			bool rotandscale = TestBit(attrib0, 8);
			bool doublesize = TestBit(attrib0, 9);
			
			if (!rotandscale && doublesize)
			{
				continue;
			}
			
			int shape = (attrib0 >> 14);
			
			uint16_t attrib1 = readoam16((attribaddr + 2));
			int size = (attrib1 >> 14);
			
			int horizontalsize = 0;
			int verticalsize = 0;
			int mapyshift = 0;
			
			if (shape == 0)
			{
				horizontalsize = (8 << size);
				verticalsize = horizontalsize;
				mapyshift = size;
			}
			else
			{
				int mapxshift = 0;
				
				switch (size)
				{
					case 0:
					{
						horizontalsize = 16;
						verticalsize = 8;
						mapxshift = 0;
						mapyshift = 1;
					}
					break;
					case 1:
					{
						horizontalsize = 32;
						verticalsize = 8;
						mapxshift = 0;
						mapyshift = 2;
					}
					break;
					case 2:
					{
						horizontalsize = 32;
						verticalsize = 16;
						mapxshift = 1;
						mapyshift = 2;
					}
					break;
					case 3:
					{
						horizontalsize = 64;
						verticalsize = 32;
						mapxshift = 2;
						mapyshift = 3;
					}
					break;
				}
				
				if (shape == 2)
				{
					swap(horizontalsize, verticalsize);
					swap(mapxshift, mapyshift);
				}
			}
			
			int samplehorizsize = horizontalsize;
			int samplevertsize = verticalsize;
			
			if (doublesize)
			{
				horizontalsize <<= 1;
				verticalsize <<= 1;
			}
			
			int32_t ypos = (attrib0 & 0xFF);
			
			if (ypos >= 160)
			{
				ypos -= 256;
			}
			
			int32_t objecty = (vcount - ypos);
			
			if ((objecty < 0) || (objecty >= verticalsize))
			{
				continue;
			}
			
			int horizsizemask = 0;
			int vertsizemask = 0;
			
			if (rotandscale)
			{
				horizsizemask = ~(samplehorizsize - 1);
				vertsizemask = ~(samplevertsize - 1);
			}
			else
			{
				horizsizemask = (horizontalsize - 1);
				vertsizemask = (verticalsize - 1);
			}
			
			int32_t xpos = (attrib1 & 0x1FF);
			
			if (xpos >= 240)
			{
				xpos -= 512;
			}
			
			bool horizontalflip = false;
			bool verticalflip = false;
			int16_t pa = 0;
			int16_t pb  = 0;
			int16_t pc = 0;
			int16_t pd = 0;
			
			if (rotandscale)
			{
				horizontalflip = false;
				verticalflip = false;
				
				int rotandscaleparam = ((attrib1 >> 9) & 0x1F);
				int paramaddr = (rotandscaleparam << 5) + 6;
				pa = readoam16(paramaddr);
				pb = readoam16((paramaddr + 8));
				pc = readoam16((paramaddr + 16));
				pd = readoam16((paramaddr + 24));
			}
			else
			{
				horizontalflip = TestBit(attrib1, 12);
				verticalflip = TestBit(attrib1, 13);
				pa = 0x100;
				pb = 0;
				pc = 0;
				pd = 0x100;
			}
			
			int mode = ((attrib0 >> 10) & 0x3);
			bool mosaic = TestBit(attrib0, 12);
			bool singlepal = TestBit(attrib0, 13);
			uint16_t attrib2 = readoam16((attribaddr + 4));
			int tilenum = (attrib2 & 0x3FF);
			int priority = ((attrib2 >> 10) & 0x3);
			int palettenum = (attrib2 >> 12);
			
			for (int pixel = 0; pixel < horizontalsize; pixel++)
			{
				int column = (pixel + xpos);
				
				if ((column < 0) || (column >= 240))
				{
					continue;
				}
				
				int previousinfo = objinfobuffer[column];
				int previouspriority = (previousinfo & 0x3);
				
				if (priority > previouspriority)
				{
					continue;
				}
				
				int samplex = pixel;
				int sampley = objecty;
				
				if (rotandscale)
				{
					int tmpx = samplex - (horizontalsize >> 1);
					int tmpy = sampley - (verticalsize >> 1);
					samplex = pa * tmpx + pb * tmpy >> 8;
					sampley = pc * tmpx + pd * tmpy >> 8;
					samplex += samplehorizsize >> 1;
					sampley += samplevertsize >> 1;
					
					if ((samplex & horizsizemask) || (sampley & vertsizemask))
					{
						continue;
					}
				}
				else
				{
					if (horizontalflip)
					{
						samplex = ~samplex & horizsizemask;
					}
					
					if (verticalflip)
					{
						sampley = ~sampley & vertsizemask;
					}
				}
				
				if (mosaic)
				{
					samplex -= (samplex & (objmosaic & 0xF));
					sampley -= (sampley & (objmosaic >> 4));
				}
				
				int mapx = samplex >> 3;
				int mapy = sampley >> 3;
				
				int tilex = samplex & 7;
				int tiley = sampley & 7;
				
				int tileaddr = tilenum;

				
				if (TestBit(dispcnt, 6))
				{
					tileaddr += mapx + (mapy << mapyshift) << singlepal;
				}
				else
				{
					tileaddr += (mapx << singlepal) + (mapy << 5);
				}
				
				tileaddr <<= 5;	

				tileaddr += tilebase;
				
				tileaddr += tilex + (tiley << 3) >> (1 - singlepal);
				
				int paletteaddr = 0;

				if ((tileaddr >= 0x18000) && (tilebase == 0x14000))
				{
				    tileaddr -= 0x4000; // Disabling this breaks sprite rendering in Spongebob Squarepants: Battle For Bikini Bottom and Barbie Groovy Games
				}
				
				if (singlepal)
				{
					uint8_t paletteindex = gpumem.vram[tileaddr];
					
					if (paletteindex == 0)
					{
						continue;
					}
					
					paletteaddr = paletteindex;
				}
				else
				{
					int paletteindex = gpumem.vram[tileaddr] >> (TestBit(tilex, 0) << 2) & 0xF;
					if (paletteindex == 0)
					{
						continue;
					}
					
					paletteaddr = (palettenum << 4) + paletteindex;
				}
				
				uint16_t color = (readpram16(0x100 + paletteaddr) & 0x7FFF);
				
				int modeflags = mode << 2 | (TestBit(previousinfo, 3) << 3);
				
				if (mode == 2)
				{
					objinfobuffer[column] = (uint16_t)(modeflags | previouspriority);
				}
				else
				{
					objbuffer[column] = color;
					objinfobuffer[column] = (uint16_t)(modeflags | priority);
				}
			}
		}
	}

	void GPU::rendercomposite()
	{
	    uint16_t backcolor = (readpram16(0) & 0x7FFF);

	    for (int i = 0; i < 240; i++)
	    {
		int objinfo = objinfobuffer[i];
		int objpriority = (objinfo & 0x3);
		int objmode = (objinfo >> 2);

		WindowControl window;
		getwindow(objmode, i, vcount, window);

		uint16_t firstcolor = backcolor;
		uint16_t secondcolor = backcolor;

		int firstlayer = 5;
		int secondlayer = 5;

		int firstpriority = 3;
		int secondpriority = 3;

		for (int layer : bglayers)
		{
		    if (!TestBit(window.layerenableflags, layer))
		    {
			continue;
		    }
	
		    uint16_t layercolor = (layer == 4) ? objbuffer[i] : getpixel(layer, i);

		    if (layercolor == 0x8000)
		    {
			continue;
		    }

		    int layerpriority = (layer == 4) ? objpriority : (getbgcontrol(layer) & 0x3);

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
		
		if (TestBit(objmode, 0) && (firstlayer == 4) && TestBit(blendcnt, (8 + secondlayer)))
		{
			firstcolor = alphablendeffect(firstcolor, secondcolor);
		}
		else if (window.specialeffect)
		{

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
		
		if (firstlayer <= 3)
		{
		setpixel(firstlayer, i, 0x8000);
		}
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
	    renderbgaff(2);
	    renderbgtrans(3);   
	}

	void GPU::renderbgmode2()
	{
	    renderbgtrans(0);
	    renderbgtrans(1);
	    renderbgaff(2);
	    renderbgaff(3);
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

	    if (TestBit(bgcnt, 6))
	    {
		ypos -= (ypos & (bgmosaic >> 4));
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

		if (TestBit(bgcnt, 6))
		{
		    xpos -= (xpos & (bgmosaic & 0xF));
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

		    paletteaddr = ((tile >> 8) & 0xF0) + paletteindex;
		}

		uint16_t color = (readpram16(paletteaddr) & 0x7FFF);
		setpixel(layernum, i, color);
	    }
	}

	void GPU::renderbgaff(int layernum)
	{
    if (layernum < 2)
    {
        return;
    }
	
	if (!TestBit(dispcnt, (8 + layernum)))
	{
		return;
	}

    uint16_t bgcnt = getbgcontrol(layernum);
    int size = (bgcnt >> 14);
    
    int sbb = ((bgcnt >> 8) & 0x1F);
    int cbb = ((bgcnt >> 2) & 0x3);
    
    int mapbase = (sbb << 11);
    int tilebase = (cbb << 14);
    
    int bgsize = (128 << size);
	
	int maplineshift = (size + 4);
	
	BGAffine temp = bgaff[(layernum - 2)];
	
	int32_t refx = temp.cx;
	int32_t refy = temp.cy;
	
	int16_t pa = temp.pa;
	int16_t pc = temp.pc;
	
	for (int i = 0; i < 240; i++)
	{	
		int32_t xpos = refx + pa * i >> 8;
		int32_t ypos = refy + pc * i >> 8;
		
		if ((xpos < 0) || (xpos >= bgsize) || (ypos < 0) || (ypos >= bgsize))
		{
		    if (TestBit(bgcnt, 13))
		    {
			xpos %= bgsize;
			ypos %= bgsize;

			if (xpos < 0)
			{
			    xpos += bgsize;
			}

			if (ypos < 0)
			{
			    ypos += bgsize;
			}
		    }
		    else
		    {
			setpixel(layernum, i, 0x8000);
			continue;
		    }
		}

		if (TestBit(bgcnt, 6))
		{
		    xpos -= (xpos & (bgmosaic & 0xF));
		    ypos -= (ypos & (bgmosaic >> 4));
		}
	
	
        int mapcol = (xpos >> 3);
        int mapline = (ypos >> 3);
        
        int tilecol = (xpos & 7);
        int tileline = (ypos & 7);
        
        int mapaddr = mapbase + (mapline << maplineshift) + mapcol;
        
        uint8_t tilenum = gpumem.vram[mapaddr];
        
        int tileaddr = tilebase + (tilenum << 6) + (tileline << 3) + tilecol;
        
        uint16_t paladdr = gpumem.vram[tileaddr];
        
        if (paladdr == 0)
        {
            setpixel(layernum, i, 0x8000);
            continue;
        }
        
        uint16_t color = (readpram16(paladdr) & 0x7FFF);
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

		BGAffine temp = bgaff[(layernum - 2)];

		int32_t refx = temp.cx;
		int32_t refy = temp.cy;
	
		int16_t pa = temp.pa;
		int16_t pc = temp.pc;

		switch (mode)
		{
		    case 0:
		    {
			int xpos = (refx + pa * i) >> 8;
			int ypos = (refy + pc * i) >> 8;
			int pixeltemp = ((xpos * 2) + (ypos * 480));

			if (pixeltemp >= 0x12C00)
			{
			    continue;
			}

			color = readvram16(pixeltemp);
		    }
		    break;
		    case 1:
		    {
			int xpos = (refx + pa * i) >> 8;
			int ypos = (refy + pc * i) >> 8;
			uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
			int pixeltemp = (xpos + (ypos * 240));

			if (pixeltemp >= 0x9600)
			{
			    continue;
			}

			uint8_t pramcolor = gpumem.vram[(pixeltemp + frameoffs)];

			if (pramcolor == 0)
			{
			    continue;
			}

			color = readpram16(pramcolor);
		    }
		    break;
		    case 2:
		    {
			int xpos = (refx + pa * i) >> 8;
			int ypos = (refy + pc * i) >> 8;
			uint16_t frameoffs = TestBit(dispcnt, 4) ? 0xA000 : 0;
			int pixeltemp = ((xpos * 2) + (ypos * 320));

			if (pixeltemp >= 0xA000)
			{
			    continue;
			}

			if ((xpos >= 160) || (ypos >= 128))
			{
			    continue;
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
