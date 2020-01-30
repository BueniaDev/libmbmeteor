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

#ifndef LIBMBMETEOR_GPU
#define LIBMBMETEOR_GPU

#include "libmbmeteor_api.h"
#include "mmu.h"
#include <iostream>
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
	class LIBMBMETEOR_API GPU
	{
		public:
			GPU(MMU& memory);
			~GPU();
		
			MMU& mem;
		
			void updatelcd();
			
			uint8_t readlcd(uint32_t addr);
			void writelcd(uint32_t addr, uint8_t val);
			
			int scanlinecounter = 0;
			int dotcounter = 0;
			
			void updatevcount();
	};
};

#endif // LIBMBMETEOR_GPU