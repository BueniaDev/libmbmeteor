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

#ifndef LIBMBMETEOR_MMU
#define LIBMBMETEOR_MMU

#include "libmbmeteor_api.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

namespace gba
{
    class LIBMBMETEOR_API MMU
    {
	public:
	    MMU();
	    ~MMU();

	    vector<uint8_t> gamerom;

	    uint8_t readByte(uint32_t addr);
	    void writeByte(uint32_t addr, uint8_t val);
	    uint16_t readWord(uint32_t addr);
	    void writeWord(uint32_t addr, uint16_t val);
	    uint32_t readLong(uint32_t addr);
	    void writeLong(uint32_t addr, uint32_t val);

	    bool loadROM(string filename);
    };
};

#endif // LIBMBMETEOR_MMU
