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
			
			void updatevcount();

			void renderpixel();
			void renderscanline();
			void renderbgmode0();
			void renderbgmode3();
			void renderbgmode4();
			void renderbgmode5();

			void rendercomposite();

			void renderbg(int layernum);

			uint16_t getbgcontrol(int layernum)
			{
			    uint16_t temp = 0;

			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0cnt; break;
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
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			void setpixel(int layernum, int x, uint16_t pixel)
			{
			    switch ((layernum & 0x3))
			    {
				case 0: bg0buffer[x] = pixel; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }
			}

			uint16_t getbgdata(int layernum)
			{
			    uint16_t temp = 0;

			    switch ((layernum & 0x3))
			    {
				case 0: temp = bg0data; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }

			    return temp;
			}

			void setbgdata(int layernum, uint16_t data)
			{
			    switch ((layernum & 0x3))
			    {
				case 0: bg0data = data; break;
				default: cout << "Unrecognized layer number of " << dec << (int)((layernum & 0x3)) << endl; exit(1); break;
			    }
			}

			uint16_t readtilegba(int mapbase, int x, int y)
			{
			    uint16_t tmaddr = mapbase;
			    tmaddr += ((((y >> 3) << 5) + (x >> 3)) << 1);
			    return readvram16(tmaddr);
			}

			uint16_t getgbacolor(int layernum, uint16_t tile, int cbb, int x, int y)
			{
			    uint16_t bgcnt = getbgcontrol(layernum);
			    int tilenumber = (tile & 0x3FF);

	   		    int tile4bit = TestBit(bgcnt, 7) ? 0 : 1;
	    		    int tilesizeshift = (6 - tile4bit);

			    int tileaddr = (cbb + (tilenumber << tilesizeshift) + (((y << 3) + x) >> tile4bit));

			    int paletteaddr = 0;

			    if (TestBit(bgcnt, 7))
			    {
				uint8_t paletteindex = gpumem.vram[tileaddr];

				paletteaddr = paletteindex;
			    }
			    else
			    {
				int paletteindex = (gpumem.vram[tileaddr] >> (TestBit(x, 0) << 2) & 0xF);

				paletteaddr = (((tile >> 8) & 0xF0) + paletteindex);
			    }

			    uint16_t color = readpram16(paletteaddr);

			    return (color == 0) ? 0x8000 : color;
			}

			RGB framebuffer[(240 * 160)];
			RGB scanlinebuffer[240];
			uint16_t bg0buffer[240];

			RGB black = {0, 0, 0};

			uint16_t dispcnt = 0;
			uint16_t vcount = 0;
			uint16_t bg0cnt = 0;

			int bg0hoffs = 0;
			int bg0voffs = 0;

			uint16_t bg0data = 0;

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
