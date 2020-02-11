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

#include "../../include/libmbmeteor/mmu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    MMU::MMU()
    {
		for (int i = 0x60; i < 0x3FF; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readtemp, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writetemp, this, _1, _2));
		}
    }

    MMU::~MMU()
    {

    }
	
	void MMU::init()
	{
		wram256.resize(0x40000, 0);
		wram32.resize(0x8000, 0);
		pram.resize(0x400, 0);
		vram.resize(0x18000, 0);
		oam.resize(0x400, 0);
		cout << "MMU::Initialized" << endl;
	}
	
	void MMU::shutdown()
	{
		wram256.clear();
		wram32.clear();
		pram.clear();
		vram.clear();
		oam.clear();
		cout << "MMU::Shutting down..." << endl;
	}

    uint8_t MMU::readByte(uint32_t addr)
    {
	uint8_t temp = 0;
	addr = (addr % 0x10000000);

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
			if (addr < 0x4000)
			{
				temp = 0xFF;
			}
			else
			{
				temp = readopenbus(addr);
			}
		}
		break;
		case 0x1:
		{
			temp = readopenbus(addr);
		}
		break;
		case 0x2:
		{
			temp = wram256[(addr & 0x3FFFF)];
		}
		break;
		case 0x3:
		{
			temp = wram32[(addr & 0x7FFF)];
		}
		break;
		case 0x4:
		{
			temp = memoryreadhandlers[(addr & 0xFFF)](addr);
		}
		break;
		case 0x5:
		{
			temp = pram[(addr & 0xFFF)];
		}
		break;
		case 0x6:
		{
			temp = vram[(addr & 0xFFFFFF)];
		}
		break;
		case 0x7:
		{
			temp = oam[(addr & 0xFFF)];
		}
		break;
	    case 0x8:
	    {
		temp = gamerom[(addr & 0xFFFFFF)];
	    }
	    break;
	    default: cout << "Unrecognized read from region of " << hex << (int)(addrtemp) << endl; temp = 0xFF; break;
	}

	return temp;
    }

    void MMU::writeByte(uint32_t addr, uint8_t val)
    {
	addr = (addr % 0x10000000);

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x2:
		{
			wram256[(addr & 0xFFFFF)] = val;
		}
		break;
		case 0x3:
		{
			wram32[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x4:
		{
			memorywritehandlers[(addr & 0xFFF)](addr, val);
		}
		break;
		case 0x5:
		{
			pram[(addr & 0xFFF)] = val;
		}
		break;
		case 0x6:
		{
			vram[(addr & 0xFFFFFF)] = val;
		}
		break;
		case 0x7:
		{
			oam[(addr & 0xFFF)] = val;
		}
		break;
	    case 0x8:
	    {
		return;
	    }
	    break;
	    default: cout << "Unrecognized write to region of " << hex << (int)(addrtemp) << endl; break;
	}
    }

    uint16_t MMU::readWord(uint32_t addr)
    {
	return ((readByte(addr + 1) << 8) | (readByte(addr)));
    }

    void MMU::writeWord(uint32_t addr, uint16_t val)
    {
	writeByte(addr, (val & 0xFF));
	writeByte((addr + 1), (val >> 8));
    }

    uint32_t MMU::readLong(uint32_t addr)
    {
	return ((readWord(addr + 2) << 16) | (readWord(addr)));
    }

    void MMU::writeLong(uint32_t addr, uint32_t val)
    {
	writeWord(addr, (val & 0xFFFF));
	writeWord((addr + 2), (val >> 16));
    }

    bool MMU::loadROM(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    cout << size << endl;
	    gamerom.resize(size, 0);
	    file.seekg(0, ios::beg);
	    file.read((char*)&gamerom[0], size);
	    cout << (int)(gamerom.size()) << endl;
	    cout << "File succesfully loaded." << endl;
	    file.close();
	    return true;
	}
	else
	{
	    cout << "Error" << endl;
	    return false;
	}
    }
};