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

#include "../../../include/libmbmeteor/nds/cpu.h"
using namespace nds;
using namespace std;

namespace nds
{
    CPU9Interface::CPU9Interface(NDSMMU& memory) : mem(memory)
    {

    }

    CPU9Interface::~CPU9Interface()
    {
	
    }

    CPU7Interface::CPU7Interface(NDSMMU& memory) : mem(memory)
    {

    }

    CPU7Interface::~CPU7Interface()
    {
	
    }

    NDSCPU::NDSCPU(NDSMMU& memory) : mem(memory)
    {
	arm9 = &mem.memarm9;
	inter9 = new CPU9Interface(mem);
	arm9->setinterface(inter9);

	arm7 = &mem.memarm7;
	inter7 = new CPU7Interface(mem);
	arm7->setinterface(inter7);
    }

    NDSCPU::~NDSCPU()
    {

    }

    void NDSCPU::init()
    {
	arm9->init(0xFFFF0000, 0xD3);
	arm7->init(0x0, 0xD3);
	cout << "CPU::Initialized" << endl;
    }

    void NDSCPU::shutdown()
    {
	cout << "CPU::Shutting down..." << endl;
    }
}
