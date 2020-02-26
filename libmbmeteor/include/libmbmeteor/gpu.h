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

#include "libmbmeteor_api.h"
#include "mmu.h"
#include <iostream>
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
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
			
			uint8_t readlcd(uint32_t addr);
			void writelcd(uint32_t addr, uint8_t val);
			
			int scanlinecounter = 0;
			int pixelx = 0;
			int lcdmode;
			
			void updatevcount();

			void renderpixel();
			void renderscanline();
			void renderbgmode0();
			void renderbgmode1();
			void renderbgmode2();
			void renderbgmode3();
			void renderbgmode4();
			void renderbgmode5();

			void rendercomposite();
			void renderbitmap(int layernum, int mode);

			void renderbgreg(int layernum);
			void renderbgtrans(int layernum);

			int bglayers[4] = {3, 2, 1, 0};

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

			uint16_t readvram16(uint32_t addr)
			{
			    return ((gpumem.vram[(addr + 1)] << 8) | (gpumem.vram[addr]));
			}

			uint16_t readpram16(int index)
			{
			    int cell = (index * 2);
			    return ((gpumem.pram[(cell + 1)] << 8) | (gpumem.pram[cell]));
			}
	};
};

#endif // LIBMBMETEOR_GPU
