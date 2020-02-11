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

#include "../../include/libmbmeteor/input.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    Input::Input(MMU& memory) : inputmem(memory)
    {
	for (int i = 0x130; i < 0x132; i++)
	{
	    inputmem.addmemoryreadhandler((0x4000000 + i), bind(&Input::readinput, this, _1));
	    inputmem.addmemorywritehandler((0x4000000 + i), bind(&Input::writeinput, this, _1, _2));
	}
    }

    Input::~Input()
    {

    }

    uint8_t Input::readinput(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFF))
	{
	    case 0x130: temp = (keyinput & 0xFF); break;
	    case 0x131: temp = (keyinput >> 8); break;
	    default: cout << "Unrecognized read from " << hex << (int)(addr) << endl; temp = 0x00; break;
	}

	return temp;
    }

    void Input::writeinput(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x130: return; break;
	    case 0x131: return; break;
	    default: cout << "Unrecognized write to " << hex << (int)(addr) << endl; break;
	}
    }

    void Input::keypressed(Button button)
    {
	keyinput = BitReset(keyinput, button);
    }

    void Input::keyreleased(Button button)
    {
	keyinput = BitSet(keyinput, button);
    }
};
