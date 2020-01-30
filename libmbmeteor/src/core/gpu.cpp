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
	GPU::GPU(MMU& memory) : mem(memory)
	{
		for (int i = 0; i < 0x60; i++)
		{
			mem.addmemoryreadhandler((0x4000000 + i), bind(&GPU::readlcd, this, _1));
			mem.addmemorywritehandler((0x4000000 + i), bind(&GPU::writelcd, this, _1, _2));
		}
	}
	
	GPU::~GPU()
	{
	
	}
	
	uint8_t GPU::readlcd(uint32_t addr)
	{
		cout << "Reading from address of " << hex << (int)(addr) << endl;
		return 0xFF;
	}
	
	void GPU::writelcd(uint32_t addr, uint8_t value)
	{
		cout << "Writing value of " << hex << (int)(value) << " to address of " << hex << (int)(addr) << endl;
		return;
	}
	
	void GPU::updatelcd()
	{
		scanlinecounter += 1;
		dotcounter += 1;
		updatevcount();
	}
	
	void GPU::updatevcount()
	{
		if (dotcounter == 4)
		{
			dotcounter = 0;
			cout << "Rendering..." << endl;
		}
		
		if (scanlinecounter == 960)
		{
			scanlinecounter = 0;
		}
	}
};