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

#ifndef LIBMBMETEOR_GPU
#define LIBMBMETEOR_GPU

#include "mmu.h"
#include <iostream>
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
	using pixelfunc = function<void()>;

	struct RGB
	{
	    uint8_t red;
	    uint8_t green;
	    uint8_t blue;
	};

	class LIBMBMETEOR_API GPU
	{
		public:
			GPU(MMU& memory);
			~GPU();

			void init();
			void shutdown();
		
			MMU& gpumem;
		
			void updatelcd();

			pixelfunc drawpixels;
			
			uint8_t readlcd(uint32_t addr);
			void writelcd(uint32_t addr, uint8_t val);
			
			int scanlinecounter = 0;
			int pixelx = 0;

			void setpixelcallback(pixelfunc cb)
			{
			    drawpixels = cb;
			}
			

			enum Phase : int
			{
			    Scanline = 0,
			    HBlank = 1,
			    VBlankScanline = 2,
			    VBlankHBlank = 3,
			};

			Phase phase = Phase::Scanline;
			
			void updatevcount();

			void renderpixel();
			void renderscanline();
			void renderbgmode0();
			void renderbgmode1();
			void renderbgmode2();
			void renderbgmode3();
			void renderbgmode4();
			void renderbgmode5();

			void renderobjects();
			void rendercomposite();
			void renderbitmap(int layernum, int mode);

			void renderbgreg(int layernum);
			void renderbgaff(int layernum);
			void renderbgtrans(int layernum);

			int bglayers[5] = {3, 2, 1, 0, 4};

			uint16_t getbgcontrol(int layernum)
			{
			    uint16_t temp = 0;

			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0cnt; break;
				case 1: temp = bg1cnt; break;
				case 2: temp = bg2cnt; break;
				case 3: temp = bg3cnt; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			int getbghoffs(int layernum)
			{
			    uint16_t temp = 0;

			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0hoffs; break;
				case 1: temp = bg1hoffs; break;
				case 2: temp = bg2hoffs; break;
				case 3: temp = bg3hoffs; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			int getbgvoffs(int layernum)
			{
			    uint16_t temp = 0;

			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0voffs; break;
				case 1: temp = bg1voffs; break;
				case 2: temp = bg2voffs; break;
				case 3: temp = bg3voffs; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			uint16_t getpixel(int layernum, int x)
			{
			    uint16_t temp = 0;
			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0buffer[x]; break;
				case 1: temp = bg1buffer[x]; break;
				case 2: temp = bg2buffer[x]; break;
				case 3: temp = bg3buffer[x]; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			void setpixel(int layernum, int x, uint16_t pixel)
			{
			    switch ((layernum & 0x3))
			    {
				case 0: bg0buffer[x] = pixel; break;
				case 1: bg1buffer[x] = pixel; break;
				case 2: bg2buffer[x] = pixel; break;
				case 3: bg3buffer[x] = pixel; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }
			}

			RGB framebuffer[(240 * 160)];
			uint16_t bg0buffer[240];
			uint16_t bg1buffer[240];
			uint16_t bg2buffer[240];
			uint16_t bg3buffer[240];
			uint16_t objbuffer[240];
			int objinfobuffer[240];

			RGB black = {0, 0, 0};

			uint16_t dispcnt = 0;
			uint16_t dispstat = 0;
			uint16_t vcount = 0;
			uint16_t bg0cnt = 0;

			int bg0hoffs = 0;
			int bg0voffs = 0;

			uint16_t bg0data = 0;

			uint16_t bg1cnt = 0;

			int bg1hoffs = 0;
			int bg1voffs = 0;

			uint16_t bg1data = 0;

			uint16_t bg2cnt = 0;

			int bg2hoffs = 0;
			int bg2voffs = 0;

			uint16_t bg2data = 0;

			uint16_t bg3cnt = 0;

			int bg3hoffs = 0;
			int bg3voffs = 0;

			uint16_t bg3data = 0;

			struct BGAffine
			{
			    int16_t pa = 0x100;
			    int16_t pb = 0;
			    int16_t pc = 0;
			    int16_t pd = 0x100;
			    int32_t x = 0;
			    int32_t y = 0;
			    int32_t cx = 0;
			    int32_t cy = 0;
			};

			void setcx(int layernum)
			{
			    bgaff[layernum].cx = (bgaff[layernum].x & 0xFFFFFFF);

			    if (TestBit(bgaff[layernum].x, 27))
			    {
				bgaff[layernum].cx |= 0xF0000000;
			    }
			}
			
			void setcy(int layernum)
			{
			    if ((bgaff[layernum].cy == 1024) && (bgaff[layernum].y == 0))
			    {
				cout << hex << (int)(dispstat) << endl;
				exit(1);
			    }

			    bgaff[layernum].cy = (bgaff[layernum].y & 0xFFFFFFF);

			    if (TestBit(bgaff[layernum].y, 27))
			    {
				bgaff[layernum].cy |= 0xF0000000;
			    }
			}

			void affhblank()
			{
			    bgaff[0].cx += bgaff[0].pb;
			    bgaff[0].cy += bgaff[0].pd;
			    bgaff[1].cx += bgaff[1].pb;
			    bgaff[1].cy += bgaff[1].pd;
			}

			bool dump = false;

			void affvblank()
			{
			    setcx(0);
			    setcy(0);
			    setcx(1);
			    setcy(1);
			}

			BGAffine bgaff[2];

			struct WindowControl
			{
			    int layerenableflags = 0;
			    bool specialeffect = false;

			    uint8_t read()
			    {
				return ((specialeffect << 5) | layerenableflags);
			    }

			    void write(uint8_t val)
			    {
				layerenableflags = (val & 0x1F);
				specialeffect = TestBit(val, 5);
			    }
			};

			struct WindowSize
			{
			    int startx = 0;
			    int endx = 0;
			    int starty = 0;
			    int endy = 0;

			    void write(int addr, uint8_t val)
			    {
				switch (addr)
				{
				    case 0: endx = val; break;
				    case 1: startx = val; break;
				    case 2: endy = val; break;
				    case 3: starty = val; break;
				}
			    }

			    bool insidewindow(int x, int y)
			    {
				if (startx <= endx)
				{
				    if ((x < startx) || (x >= endx))
				    {
					return false;
				    }
				}
				else
				{
				    if ((x >= endx) && (x < startx))
				    {
					return false;
				    }
				}

				if (starty <= endy)
				{
				    if ((y < starty) || (y >= endy))
				    {
					return false;
				    }
				}
				else
				{
				    if ((y >= endy) && (y < starty))
				    {
					return false;
				    }
				}

				return true;
			    }
			};

			void getwindow(int objmode, int x, int y, WindowControl &window)
			{
			    if ((dispcnt >> 13) == 0)
			    {
				window.layerenableflags = 0x1F;
				window.specialeffect = true;
				return;
			    }

			    for (int i = 0; i < 2; i++)
			    {
				if (TestBit(dispcnt, (13 + i)) && insidewindow(i, x, y))
				{
				    window = winctrl[i];
				    return;
				}
			    }

			    if (TestBit(dispcnt, 15) && TestBit(objmode, 1))
			    {
				window = winctrl[3];
				return;
			    }

			    window = winctrl[2];
			}

			bool insidewindow(int i, int x, int y)
			{
			    return winsize[i].insidewindow(x, y);
			}

			WindowControl winctrl[4];
			WindowSize winsize[2];

			uint8_t bgmosaic = 0;
			uint8_t objmosaic = 0;

			uint16_t blendcnt = 0;
			int blendeva = 0;
			int blendevb = 0;
			int blendevy = 0;

			uint16_t alphablendeffect(uint16_t first, uint16_t second)
			{
			    int red1 = (first & 0x1F);
			    int green1 = ((first >> 5) & 0x1F);
			    int blue1 = ((first >> 10) & 0x1F);

			    int red2 = (second & 0x1F);
			    int green2 = ((second >> 5) & 0x1F);
			    int blue2 = ((second >> 10) & 0x1F);

			    int eva = min(blendeva, 16);
			    int evb = min(blendevb, 16);

			    red1 = (red1 * eva + 8) >> 4;
			    green1 = (green1 * eva + 8) >> 4;
			    blue1 = (blue1 * eva + 8) >> 4;

			    red2 = (red2 * evb + 8) >> 4;
			    green2 = (green2 * evb + 8) >> 4;
			    blue2 = (blue2 * evb + 8) >> 4;

			    int red = min(31, (red1 + red2));
			    int green = min(31, (green1 + green2));
			    int blue = min(31, (blue1 + blue2));

			    uint16_t tempcolor = (((blue & 0x1F) << 10) | ((green & 0x1F) << 5) | (red & 0x1F));
			    return (tempcolor & 0x7FFF);
			}		

			uint16_t brightnesseffect(bool decrease, uint16_t color)
			{
			    int red = (color & 0x1F);
			    int green = ((color >> 5) & 0x1F);
			    int blue = ((color >> 10) & 0x1F);

			    int evy = min(blendevy, 16);

			    if (decrease)
			    {
				red -= (red * evy + 8) >> 4;
				green -= (green * evy + 8) >> 4;
				blue -= (blue * evy + 8) >> 4;
			    }
			    else
			    {
				red += ((31 - red) * evy + 8) >> 4;
				green += ((31 - green) * evy + 8) >> 4;
				blue += ((31 - blue) * evy + 8) >> 4;
			    }

			    uint16_t tempcolor = (((blue & 0x1F) << 10) | ((green & 0x1F) << 5) | (red & 0x1F));
			    return (tempcolor & 0x7FFF);
			}

			bool isnothblankvblank()
			{
			    return (phase != Phase::VBlankHBlank);
			}

			void vblank(bool val)
			{
			    dispstat = BitChange(dispstat, 0, val);

			    if (TestBit(dispstat, 3))
			    {
				gpumem.writeLong(0x3007FF8, BitChange(gpumem.readLong(0x3007FF8), 0, val));
			        if (val)
				{
				    gpumem.setinterrupt(0);
				}
			    }
			}

			void hblank(bool val)
			{
			    dispstat = BitChange(dispstat, 1, val);

			    if (TestBit(dispstat, 4))
			    {
				gpumem.writeLong(0x3007FF8, BitChange(gpumem.readLong(0x3007FF8), 1, val));
			        if (val)
				{
				    gpumem.setinterrupt(1);
				}
			    }
			}

			void vcountset(bool val)
			{
			    dispstat = BitChange(dispstat, 2, val);

			    if (TestBit(dispstat, 5))
			    {
				gpumem.writeLong(0x3007FF8, BitChange(gpumem.readLong(0x3007FF8), 2, val));
				if (val)
				{
				    gpumem.setinterrupt(2);
				}
			    }
			}

			uint16_t readvram16(uint32_t addr)
			{
			    return ((gpumem.vram[(addr + 1)] << 8) | (gpumem.vram[addr]));
			}
			
			uint16_t readoam16(uint32_t addr)
			{
			    return ((gpumem.oam[(addr + 1)] << 8) | (gpumem.oam[addr]));
			}

			uint16_t readpram16(int index)
			{
			    int cell = (index * 2);
			    return ((gpumem.pram[(cell + 1)] << 8) | (gpumem.pram[cell]));
			}
	};
};

#endif // LIBMBMETEOR_GPU
