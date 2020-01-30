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

#include "../../include/libmbmeteor/cpu.h"
using namespace gba;
using namespace std;

namespace gba
{
    CPUInterface::CPUInterface(MMU& memory, GPU& graphics) : mem(memory), gpu(graphics)
    {

    }

    CPUInterface::~CPUInterface()
    {
	
    }

    CPU::CPU(MMU& memory, GPU& graphics) : mem(memory), gpu(graphics)
    {
	arm = &mem.memarm;
	inter = new CPUInterface(mem, gpu);
	arm->setinterface(inter);
    }

    CPU::~CPU()
    {

    }

    void CPU::init()
    {
	arm->init(0x8000000, 0x5F);
	arm->armreg.r13 = 0x3007F00;
	arm->armreg.r13irq = 0x3007FA0;
	cout << "CPU::Initialized" << endl;
    }

    void CPU::shutdown()
    {
	cout << "CPU::Shutting down..." << endl;
    }
}
