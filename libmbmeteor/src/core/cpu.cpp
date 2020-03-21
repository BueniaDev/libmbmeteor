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
    CPUInterface::CPUInterface(MMU& memory, GPU& graphics, Timers& timers, APU& audio) : mem(memory), gpu(graphics), timer(timers), apu(audio)
    {

    }

    CPUInterface::~CPUInterface()
    {
	
    }

    CPU::CPU(MMU& memory, GPU& graphics, Input& input, Timers& timers, APU& audio) : mem(memory), gpu(graphics), joyp(input), timer(timers), apu(audio)
    {
	arm = &mem.memarm;
	inter = new CPUInterface(mem, gpu, timer, apu);
	arm->setinterface(inter);
    }

    CPU::~CPU()
    {

    }

    void CPU::init()
    {
	arm->init(0x0, 0xD3);
	cout << "CPU::Initialized" << endl;
    }

    void CPU::shutdown()
    {
	cout << "CPU::Shutting down..." << endl;
    }
}
