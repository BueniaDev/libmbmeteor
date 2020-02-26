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

#ifndef LIBMBMETEOR_H
#define LIBMBMETEOR_H

#include "mmu.h"
#include "cpu.h"
#include "gpu.h"
#include "input.h"
#include "libmbmeteor_api.h"
#include <cstring>
#include <memory>
#include <vector>
#include <utility>
#include <iostream>
using namespace gba;
using namespace std;

namespace gba
{
    class LIBMBMETEOR_API GBACore
    {
	public:
	    GBACore();
	    ~GBACore();

	    unique_ptr<MMU> coremmu;
	    unique_ptr<GPU> coregpu;
	    unique_ptr<Input> coreinput;
	    unique_ptr<CPU> corecpu;

	    bool init();
	    void shutdown();

	    bool getoptions(int argc, char* argv[]);
	    void printusage(char* argv);
	    bool loadROM(string filename);
	    bool loadBIOS(string filename);
	    RGB getpixel(int x, int y);
	    void keypressed(Button button);
	    void keyreleased(Button button);
	    void runcore();

	    int overspentcycles = 0;

	    string romname;
	    string biosname;
    };
};



#endif // LIBMBMETEOR_H
