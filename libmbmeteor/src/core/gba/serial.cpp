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

#include "../../../include/libmbmeteor/gba/serial.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    Serial::Serial(MMU& memory) : serialmem(memory)
    {
	for (int i = 0x134; i < 0x136; i++)
	{
	    serialmem.addmemoryreadhandler((0x4000000 + i), bind(&Serial::readserial, this, _1));
	    serialmem.addmemorywritehandler((0x4000000 + i), bind(&Serial::writeserial, this, _1, _2));
	}

	for (int i = 0x120; i < 0x12C; i++)
	{
	    serialmem.addmemoryreadhandler((0x4000000 + i), bind(&Serial::readserial, this, _1));
	    serialmem.addmemorywritehandler((0x4000000 + i), bind(&Serial::writeserial, this, _1, _2));
	}
    }

    Serial::~Serial()
    {

    }

    uint8_t Serial::readserial(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFF))
	{
	    case 0x128: temp = (siocnt & 0xFF); break;
	    case 0x129: temp = (siocnt >> 8); break;
	    case 0x12A:
	    {
		if (mode == SerialMode::Multiplayer)
		{
		    temp = (siomltsend & 0xFF);
		}
		else
		{
		    cout << "Invalid mode" << endl;
		    temp = 0;
		}
	    }
	    break;
	    case 0x12B:
	    {
		if (mode == SerialMode::Multiplayer)
		{
		    temp = (siomltsend >> 8);
		}
		else
		{
		    cout << "Invalid mode" << endl;
		    temp = 0;
		}
	    }
	    break;
	    case 0x134: temp = (rcnt & 0xFF); break;
	    case 0x135: temp = (rcnt >> 8); break;
	    default: cout << "Unrecognized read from " << hex << (int)((addr & 0xFFF)) << endl; exit(1); temp = 0; break;
	}

	return temp;
    }

    void Serial::writeserial(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x120:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x121:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x122:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x123:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x124:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x125:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x126:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x127:
	    {
		if (mode != SerialMode::Multiplayer)
		{
		    return;
		}
	    }
	    break;
	    case 0x128: siocnt = ((siocnt & 0xFF00) | val); break;
	    case 0x129: siocnt = ((siocnt & 0xFF) | (val << 8)); updatemode(); break;
	    case 0x12A:
	    {
		if (mode == SerialMode::Multiplayer)
		{
		    siomltsend = ((siomltsend & 0xFF00) | val);
		}
		else
		{
		    cout << "Invalid mode" << endl;
		}
	    }
	    break;
	    case 0x12B:
	    {
		if (mode == SerialMode::Multiplayer)
		{
		    siomltsend = ((siomltsend & 0xFF) | (val << 8));
		}
		else
		{
		    cout << "Invalid mode" << endl;
		}
	    }
	    break;
	    case 0x134: rcnt = ((rcnt & 0xFF00) | val); break;
	    case 0x135: rcnt = ((rcnt & 0xFF) | (val << 8)); break;
	    default: cout << "Unrecognized write to " << hex << (int)((addr & 0xFFF)) << ", value of " << hex << (int)(val) << endl; exit(1); return;break;
	}
    }
};
