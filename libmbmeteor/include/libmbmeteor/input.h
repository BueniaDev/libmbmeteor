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

#ifndef LIBMBMETEOR_INPUT
#define LIBMBMETEOR_INPUT

#include "libmbmeteor_api.h"
#include "mmu.h"
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
    enum Button : int
    {
	A = 0,
	B = 1,
	Select = 2,
	Start = 3,
	Right = 4,
	Left = 5,
	Up = 6,
	Down = 7,
	R = 8,
	L = 9,
    };

    class LIBMBMETEOR_API Input
    {
	public:
	    Input(MMU& memory);
	    ~Input();

	    MMU& inputmem;

	    uint16_t keyinput = 0x3FF;

	    uint8_t readinput(uint32_t addr);
	    void writeinput(uint32_t addr, uint8_t val);

	    void keypressed(Button button);
	    void keyreleased(Button button);
    };
};

#endif // LIBMBMETEOR_INPUT
