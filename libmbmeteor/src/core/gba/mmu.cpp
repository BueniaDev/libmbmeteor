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

#include "../../../include/libmbmeteor/gba/mmu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
    MMU::MMU()
    {
		for (int i = 0x60; i < 0xB0; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readtemp, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writetemp, this, _1, _2));
		}

		for (int i = 0xB0; i <= 0xDF; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readdma, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writedma, this, _1, _2));
		}

		for (int i = 0xE0; i < 0x200; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readtemp, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writetemp, this, _1, _2));
		}

		for (int i = 0x200; i < 0x20A; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readinterrupts, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writeinterrupts, this, _1, _2));
		}

		for (int i = 0x20A; i < 0xFFF; i++)
		{
			addmemoryreadhandler((0x4000000 + i), bind(&MMU::readtemp, this, _1));
			addmemorywritehandler((0x4000000 + i), bind(&MMU::writetemp, this, _1, _2));
		}
    }

    MMU::~MMU()
    {

    }
	
	void MMU::init()
	{
		bios.resize(0x4000, 0);
		wram256.resize(0x40000, 0);
		wram32.resize(0x8000, 0);
		pram.resize(0x8000, 0);
		vram.resize(0x18000, 0);
		oam.resize(0x400, 0);
		cout << "MMU::Initialized" << endl;
	}
	
	void MMU::shutdown()
	{
		bios.clear();
		wram256.clear();
		wram32.clear();
		pram.clear();
		vram.clear();
		oam.clear();
		cout << "MMU::Shutting down..." << endl;
	}

    uint8_t MMU::readByte(uint32_t addr)
    {
	uint8_t temp = 0;

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
			if (addr < 0x4000)
			{
				temp = bios[addr];
			}
			else
			{
				temp = 0x00;
			}
		}
		break;
		case 0x2:
		{
			temp = wram256[(addr & 0x3FFFF)];
		}
		break;
		case 0x3:
		{
			temp = wram32[(addr & 0x7FFF)];
		}
		break;
		case 0x4:
		{
			if (memoryreadhandlers[(addr & 0xFFF)])
			{
			    temp = memoryreadhandlers[(addr & 0xFFF)](addr);
			}
			else
			{
			    temp = 0xFF;
			}
		}
		break;
		case 0x5:
		{
			temp = pram[(addr & 0x7FFF)];
		}
		break;
		case 0x6:
		{
			temp = vram[(addr & 0x1FFFF)];
		}
		break;
		case 0x7:
		{
			temp = oam[(addr & 0xFFF)];
		}
		break;
	    case 0x8:
	    {
		temp = gamerom[(addr - 0x8000000)];

		
		if (rtcenable || solarenable)
		{

		if ((addr >= 0x80000C4) && (addr < 0x80000CA))
		{
		    if (isgpioreadable)
		    {
			switch ((addr & 0xFF))
			{
			    case 0xC4: temp = gpiodata; break;
			    case 0xC6: temp = gpiodirection; break;
			    case 0xC8: temp = isgpioreadable; break;
			    default: temp = 0x00; break;
			}
		    }
		    else
		    {
			temp = 0x00;
		    }
		}

		}
	    }
	    break;
	    case 0x9:
	    {
		temp = gamerom[(addr - 0x8000000)];
	    }
	    break;
	    case 0xA:
	    {
		temp = gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xB:
	    {
		temp = gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xC:
	    {
		temp = gamerom[(addr - 0xC000000)];
	    }
	    break;
	    case 0xD:
	    {
		if (iseeprom())
		{
		    if (!TestBit(addr, 0))
		    {
			temp = readeeprom(addr);
		    }
		    else
		    {
			temp = 0;
		    }
		}
	    }
	    break;
	    case 0xE:
	    {
		if (issram())
		{
		    if ((addr & 0xFFFF) <= 0x7FFF)
		    {
			temp = gamesram[(addr & 0xFFFF)];
		    }
		    else
		    {
			temp = 0xFF;
		    }
		}
		else if (isflash128k())
		{
		    temp = readflash128k(addr);
		}
		else if (isflash64k())
		{
		    temp = readflash64k(addr);
		}
	    }
	    break;
	    default: temp = 0x00; break;
	}

	return temp;
    }

    void MMU::writeByte(uint32_t addr, uint8_t val)
    {
	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
		    return;
		}
		break;
		case 0x1:
		{
		    return;
		}
		break;
		case 0x2:
		{
			wram256[(addr & 0x3FFFF)] = val;
		}
		break;
		case 0x3:
		{
			wram32[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x4:
		{
			if (memorywritehandlers[(addr & 0xFFF)])
			{
			    memorywritehandlers[(addr & 0xFFF)](addr, val);
			}
		}
		break;
		case 0x5:
		{
			pram[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x6:
		{
			vram[(addr & 0x1FFFF)] = val;
		}
		break;
		case 0x7:
		{
			oam[(addr & 0xFFF)] = val;
		}
		break;
	    case 0x8:
	    {
		if (!rtcenable && !solarenable)
		{
		    return;
		}

		if ((addr >= 0x80000C4) && (addr < 0x80000CA))
		{
		    switch ((addr & 0xFF))
		    {
			case 0xC4:
			{
			    gpiodata = (val & 0xF);
			    
			    if (rtcenable)
			    {
			        processrtc();
			    }

			    if (solarenable)
			    {
				processsolar();
			    }
			}
			break;
			case 0xC6:
			{
			    gpiodirection = (val & 0xF);
			}
			break;
			case 0xC8:
			{
			    isgpioreadable = TestBit(val, 0);
			}
			break;
			default: return; break;
		    }
		}
		else
		{
		    return;
		}
	    }
	    break;
	    case 0x9:
	    {
		return;
	    }
	    break;
	    case 0xA:
	    {
		return;
	    }
	    break;
	    case 0xD:
	    {
		if (iseeprom())
		{
		    if (!TestBit(addr, 0))
		    {
			writeeeprom(val);
		    }
		}
	    }
	    break;
	    case 0xE:
	    {
		if (issram())
		{
		    if ((addr & 0xFFFF) <= 0x7FFF)
		    {
			gamesram[(addr & 0xFFFF)] = val;
		    }
		}
		else if (isflash128k())
		{
		    writeflash128k(addr, val);
		}
		else if (isflash64k())
		{
		    writeflash64k(addr, val);
		}		
	    }
	    break;
	    case 0xF:
	    {
		return;
	    }
	    break;
	    default: return; break;
	}
    }

    uint16_t MMU::readWord(uint32_t addr)
    {
	uint16_t temp = 0;

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
			if (addr < 0x4000)
			{
				temp = *(uint16_t*)&bios[addr];
			}
			else
			{
				temp = 0x00;
			}
		}
		break;
		case 0x2:
		{
			temp = *(uint16_t*)&wram256[(addr & 0x3FFFF)];
		}
		break;
		case 0x3:
		{
			temp = *(uint16_t*)&wram32[(addr & 0x7FFF)];
		}
		break;
		case 0x4:
		{
			temp = ((readByte(addr + 1) << 8) | (readByte(addr)));
		}
		break;
		case 0x5:
		{
			temp = *(uint16_t*)&pram[(addr & 0x7FFF)];
		}
		break;
		case 0x6:
		{
			temp = *(uint16_t*)&vram[(addr & 0x1FFFF)];
		}
		break;
		case 0x7:
		{
			temp = *(uint16_t*)&oam[(addr & 0xFFF)];
		}
		break;
	    case 0x8:
	    {
		temp = ((readByte(addr + 1) << 8) | (readByte(addr)));
	    }
	    break;
	    case 0x9:
	    {
		temp = *(uint16_t*)&gamerom[(addr - 0x8000000)];
	    }
	    break;
	    case 0xA:
	    {
		temp = *(uint16_t*)&gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xB:
	    {
		temp = *(uint16_t*)&gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xC:
	    {
		temp = *(uint16_t*)&gamerom[(addr - 0xC000000)];
	    }
	    break;
	    case 0xD:
	    {
		temp = ((readByte(addr + 1) << 8) | (readByte(addr)));
	    }
	    break;
	    case 0xE:
	    {
		temp = ((readByte(addr + 1) << 8) | (readByte(addr)));
	    }
	    break;
	    default: temp = 0x00; break;
	}

	return temp;
    }

    void MMU::writeWord(uint32_t addr, uint16_t val)
    {
	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
		    return;
		}
		break;
		case 0x1:
		{
		    return;
		}
		break;
		case 0x2:
		{
			*(uint16_t*)&wram256[(addr & 0x3FFFF)] = val;
		}
		break;
		case 0x3:
		{
			*(uint16_t*)&wram32[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x4:
		{
	    		writeByte(addr, (val & 0xFF));
	    		writeByte((addr + 1), (val >> 8));
		}
		break;
		case 0x5:
		{
			*(uint16_t*)&pram[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x6:
		{
			*(uint16_t*)&vram[(addr & 0x1FFFF)] = val;
		}
		break;
		case 0x7:
		{
			*(uint16_t*)&oam[(addr & 0xFFF)] = val;
		}
		break;
	    case 0x8:
	    {
	    	writeByte(addr, (val & 0xFF));
	    	writeByte((addr + 1), (val >> 8));
	    }
	    break;
	    case 0x9:
	    {
		return;
	    }
	    break;
	    case 0xA:
	    {
		return;
	    }
	    break;
	    case 0xD:
	    {
	    	writeByte(addr, (val & 0xFF));
	    	writeByte((addr + 1), (val >> 8));
	    }
	    break;
	    case 0xE:
	    {
	    	writeByte(addr, (val & 0xFF));
	    	writeByte((addr + 1), (val >> 8));		
	    }
	    break;
	    case 0xF:
	    {
		return;
	    }
	    break;
	    default: return; break;
	}
    }

    uint32_t MMU::readLong(uint32_t addr)
    {
	uint32_t temp = 0;

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
			if (addr < 0x4000)
			{
				temp = *(uint32_t*)&bios[addr];
			}
			else
			{
				temp = 0x00;
			}
		}
		break;
		case 0x2:
		{
			temp = *(uint32_t*)&wram256[(addr & 0x3FFFF)];
		}
		break;
		case 0x3:
		{
			temp = *(uint32_t*)&wram32[(addr & 0x7FFF)];
		}
		break;
		case 0x4:
		{
			temp = ((readWord(addr + 2) << 16) | (readWord(addr)));
		}
		break;
		case 0x5:
		{
			temp = *(uint32_t*)&pram[(addr & 0x7FFF)];
		}
		break;
		case 0x6:
		{
			temp = *(uint32_t*)&vram[(addr & 0x1FFFF)];
		}
		break;
		case 0x7:
		{
			temp = *(uint32_t*)&oam[(addr & 0xFFF)];
		}
		break;
	    case 0x8:
	    {
		temp = ((readWord(addr + 2) << 16) | (readWord(addr)));
	    }
	    break;
	    case 0x9:
	    {
		temp = *(uint32_t*)&gamerom[(addr - 0x8000000)];
	    }
	    break;
	    case 0xA:
	    {
		temp = *(uint32_t*)&gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xB:
	    {
		temp = *(uint32_t*)&gamerom[(addr - 0xA000000)];
	    }
	    break;
	    case 0xC:
	    {
		temp = *(uint32_t*)&gamerom[(addr - 0xC000000)];
	    }
	    break;
	    case 0xD:
	    {
		temp = ((readWord(addr + 2) << 16) | (readWord(addr)));
	    }
	    break;
	    case 0xE:
	    {
		temp = ((readWord(addr + 2) << 16) | (readWord(addr)));
	    }
	    break;
	    default: temp = 0x00; break;
	}

	return temp;
    }

    void MMU::writeLong(uint32_t addr, uint32_t val)
    {
	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
		case 0x0:
		{
		    return;
		}
		break;
		case 0x1:
		{
		    return;
		}
		break;
		case 0x2:
		{
			*(uint32_t*)&wram256[(addr & 0x3FFFF)] = val;
		}
		break;
		case 0x3:
		{
			*(uint32_t*)&wram32[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x4:
		{
			writeWord(addr, (val & 0xFFFF));
			writeWord((addr + 2), (val >> 16));
		}
		break;
		case 0x5:
		{
			*(uint32_t*)&pram[(addr & 0x7FFF)] = val;
		}
		break;
		case 0x6:
		{
			*(uint32_t*)&vram[(addr & 0x1FFFF)] = val;
		}
		break;
		case 0x7:
		{
			*(uint32_t*)&oam[(addr & 0xFFF)] = val;
		}
		break;
	    case 0x8:
	    {
		writeWord(addr, (val & 0xFFFF));
		writeWord((addr + 2), (val >> 16));
	    }
	    break;
	    case 0x9:
	    {
		return;
	    }
	    break;
	    case 0xA:
	    {
		return;
	    }
	    break;
	    case 0xD:
	    {
		writeWord(addr, (val & 0xFFFF));
		writeWord((addr + 2), (val >> 16));
	    }
	    break;
	    case 0xE:
	    {
		writeWord(addr, (val & 0xFFFF));
		writeWord((addr + 2), (val >> 16));		
	    }
	    break;
	    case 0xF:
	    {
		return;
	    }
	    break;
	    default: return; break;
	}


    }

    void MMU::processrtc()
    {
	switch (rtcstate)
	{
	    case 0:
	    {
		if ((gpiodata & 0x5) == 1)
		{
		    rtcstate = 1;
		}
	    }
	    break;
	    case 1:
	    {
		if ((gpiodata & 0x5) == 5)
		{
		    rtcstate = 2;
		    rtccounter = 0;
		    rtcbyte = 0;
		}
	    }
	    break;
	    case 2:
	    {
		if (TestBit(gpiodata, 0))
		{
		    rtcbyte = BitChange(rtcbyte, (7 - rtccounter), TestBit(gpiodata, 1));
		    rtccounter += 1;

		    if (rtccounter == 8)
		    {
			if ((rtcbyte & 0xF0) != 0x60)
			{
			    cout << "Byte is flawed" << endl;
			    exit(1);
			    return;
			}

			int command = ((rtcbyte >> 1) & 0x7);
			bool readbit = TestBit(rtcbyte, 0);

			switch (command)
			{
			    case 0:
			    {
				rtcdtregs.reset();
				rtcstate = 0;
			    }
			    break;
			    case 1:
			    {
				if (readbit)
				{
				    rtclength = 1;
				    rtcdata = rtcdtregs.readcontrol();
				    rtcstate = 3;
				}
				else
				{
				    rtclength = 1;
				    rtcstate = 4;
				    rtcwritetype = 0;
				}
			    }
			    break;
			    case 2:
			    {
				if (readbit)
				{
				    rtclength = 7;
				    rtcdata = rtcdtregs.readdatetime();
				    rtcstate = 3;
				}
				else
				{
				    rtclength = 7;
				    rtcstate = 4;
				    rtcwritetype = 1;
				}
			    }
			    break;
			    case 3:
			    {
				if (readbit)
				{
				    rtclength = 3;
				    rtcdata = rtcdtregs.readtime();
				    rtcstate = 3;
				}
				else
				{
				    rtclength = 3;
				    rtcstate = 4;
				    rtcwritetype = 2;
				}
			    }
			    break;
			    default: cout << "Unrecognized command of " << dec << (int)(command) << endl; exit(1); break;
			}
		    }
		}
	    }
	    break;
	    case 3:
	    {
		if (TestBit(gpiodata, 0))
		{
		    gpiodata = BitChange(gpiodata, 1, TestBit(rtcdata[rtcserial], 0));

		    rtcdata[rtcserial] >>= 1;
		    rtcbitcounter += 1;

		    if (rtcbitcounter == 8)
		    {
			rtcbitcounter = 0;
			rtcserial += 1;

			if (rtcserial == rtclength)
			{
			    rtcdata.fill(0);
			    rtcstate = 0;
			    rtcserial = 0;
			    rtclength = 0;
			}
		    }
		}
	    }
	    break;
	    case 4:
	    {
		if (!TestBit(gpiodata, 0))
		{
		    rtcdata[rtcserial] = BitChange(rtcdata[rtcserial], rtcbitcounter, TestBit(gpiodata, 1));

		    rtcbitcounter += 1;

		    if (rtcbitcounter == 8)
		    {
			rtcbitcounter = 0;
			rtcserial += 1;

			if (rtcserial == rtclength)
			{
			    rtcdtregs.writeregisters(rtcwritetype, rtcdata);
			    rtcdata.fill(0);
			    rtcstate = 0;
			    rtcserial = 0;
			    rtclength = 0;
			    rtcwritetype = 3;
			}
		    }
		}
	    }
	    break;
	}
    }

    void MMU::processsolar()
    {
	if (TestBit(gpiodata, 1))
	{
	    solarcounter = 0;
	}

	if (TestBit(gpiodata, 0))
	{
	    solarcounter += 1;

	    if (solarcounter == solarvalue)
	    {
		gpiodata = BitSet(gpiodata, 3);
	    }
	}
    }

    void MMU::increasesolar()
    {
	solarvalue -= 8;

	if (solarvalue < 0x50)
	{
	    solarvalue = 0x50;
	}
    }

    void MMU::decreasesolar()
    {
	solarvalue += 8;

	if (solarvalue >= 0xE8)
	{
	    solarvalue = 0xE8;
	}
    }

    uint8_t MMU::readflash128k(uint32_t addr)
    {
	uint8_t temp = 0;

	if ((addr == 0xE000000) && (flashgrabid))
	{
	    temp = (flash128id & 0xFF);
	}
	else if ((addr == 0xE000001) && (flashgrabid))
	{
	    temp = (flash128id >> 8);
	}
	else
	{
	    temp = gameflash[(addr & 0xFFFF) + (flash128kbank * 0x10000)];
	}

	return temp;
    }

    void MMU::writeflash128k(uint32_t addr, uint8_t val)
    {
	if (flashwrite && ((addr >= 0xE000000) && (addr < 0xE010000)))
	{
	    gameflash[(addr & 0xFFFF) + (flash128kbank * 0x10000)] = val;
	    flashwrite = false;
	}
	else if ((addr == 0xE000000) && (flashswitch) && (flashstate == 0))
	{
	    flash128kbank = val;
	    flashswitch = false;
	}
	else if ((addr & 0xFFF0FFF) == 0xE000000)
	{
	    if ((val == 0x30) && (flashstate == 2))
	    {
		for (int i = addr; i < (int)(addr + 0x1000); i++)
		{
		    gameflash[(i & 0xFFFF) + (flash128kbank * 0x10000)] = 0xFF;
		}

		flashstate = 0;
	    }
	}
	else if (addr == 0xE005555)
	{
	    if ((val == 0xAA) && (flashstate == 0))
	    {
		flashstate += 1;
	    }
	    else if (flashstate == 2)
	    {
		switch (val)
		{
		    case 0x80:
		    {
			flashstate = 0;
		    }
		    break;
		    case 0x90:
		    {
			flashgrabid = true;
			flashstate = 0;
		    }
		    break;
		    case 0xA0:
		    {
			flashwrite = true;
			flashstate = 0;
		    }
		    break;
		    case 0xB0:
		    {
			flashswitch = true;
			flashstate = 0;
		    }
		    break;
		    case 0xF0:
		    {
			flashgrabid = false;
			flashstate = 0;
		    }
		    break;
		    default: cout << "Unrecognized flash command of " << hex << (int)(val) << endl; exit(1); break;
		}
	    }
	}
	else if (addr == 0xE002AAA)
	{
	    if ((val == 0x55) && (flashstate == 1))
	    {
		flashstate += 1;
	    }
	}
    }

    uint8_t MMU::readflash64k(uint32_t addr)
    {
	uint8_t temp = 0;

	if ((addr == 0xE000000) && (flashgrabid))
	{
	    temp = (flash64id & 0xFF);
	}
	else if ((addr == 0xE000001) && (flashgrabid))
	{
	    temp = (flash64id >> 8);
	}
	else
	{
	    temp = gameflash[(addr & 0xFFFF)];
	}

	return temp;
    }

    void MMU::writeflash64k(uint32_t addr, uint8_t val)
    {
	if (flashwrite && ((addr >= 0xE000000) && (addr < 0xE010000)))
	{
	    gameflash[(addr & 0xFFFF)] = val;
	    flashwrite = false;
	}
	else if ((addr & 0xFFF0FFF) == 0xE000000)
	{
	    if ((val == 0x30) && (flashstate == 2))
	    {
		for (int i = addr; i < (int)(addr + 0x1000); i++)
		{
		    gameflash[(i & 0xFFFF)] = 0xFF;
		}

		flashstate = 0;
	    }
	}
	else if (addr == 0xE005555)
	{
	    if ((val == 0xAA) && (flashstate == 0))
	    {
		flashstate += 1;
	    }
	    else if (flashstate == 2)
	    {
		switch (val)
		{
		    case 0x80:
		    {
			flashstate = 0;
		    }
		    break;
		    case 0x90:
		    {
			flashgrabid = true;
			flashstate = 0;
		    }
		    break;
		    case 0xA0:
		    {
			flashwrite = true;
			flashstate = 0;
		    }
		    break;
		    case 0xF0:
		    {
			flashgrabid = false;
			flashstate = 0;
		    }
		    break;
		    default: cout << "Unrecognized flash command of " << hex << (int)(val) << endl; exit(1); break;
		}
	    }
	}
	else if (addr == 0xE002AAA)
	{
	    if ((val == 0x55) && (flashstate == 1))
	    {
		flashstate += 1;
	    }
	}
    }

    uint8_t MMU::readeeprom(uint32_t addr)
    {
	uint8_t temp = 0;

	if (eepromstate == 3)
	{
	    if (eepromcmd == 2)
	    {
		temp = 1;
		eepromstate = 1;
	    }
	    else
	    {
		if (eepromdelay == 0)
		{
		    eeprombitsread -= 1;
		    temp = TestBit(gameeeprom[eepromaddr], eeprombits) ? 1 : 0;

		    eeprombits -= 1;

		    if (eeprombits < 0)
		    {
			eeprombits = 7;
			eepromaddr += 1;
		    }
		
		    if (eeprombitsread == 0)
		    {
			eepromstate = 1;
			eeprombuffer = 0;
			eeprombufflength = 0;
			eeprombitsread = 64;
			eepromdelay = 4;
		    }
		}
		else
		{
		    eepromdelay -= 1;
		    temp = 0;
		}
	    }
	}
	else
	{
	    temp = 0;
	}

	return temp;
    }

    void MMU::writeeeprom(uint8_t value)
    {
	switch (eepromstate)
	{
	    case 1:
	    {
		eeprombuffer <<= 1;
		eeprombuffer |= TestBit(value, 0);
		eeprombufflength += 1;

		if (eeprombufflength == 2)
		{
		    eeprombufflength = 0;
		    eepromcmd = (eeprombuffer & 0x3);
		    eepromstate = 2;
		}
	    }
	    break;
	    case 2:
	    {
		switch (eepromcmd)
		{
		    case 2:
		    {
			eeprombuffer <<= 1;
			eeprombuffer |= TestBit(value, 0);
			eeprombufflength += 1;

			if (eeprombufflength == eeprombitsize)
			{
			    eepromwrite = ((eeprombuffer & eeprombitmask) << 3);
			    eeprombufflength = 0;
			    eeprombuffer = 0;
			    eepromstate = 4;
			}
		    }
		    break;
		    case 3:
		    {
			eeprombuffer <<= 1;
			eeprombuffer |= TestBit(value, 0);
			eeprombufflength += 1;

			if (eeprombufflength == (eeprombitsize + 1))
			{
			    eepromaddr = (((eeprombuffer >> 1) & eeprombitmask) << 3);
			    eeprombufflength = 0;
			    eepromstate = 3;
			}
		    }
		    break;
		    default: cout << "Unrecognized EEPROM command of " << hex << (int)(eepromcmd) << endl; exit(1); break;
		}
	    }
	    break;
	    case 4:
	    {
		eeprombuffer <<= 1;
		eeprombuffer |= TestBit(value, 0);
		eeprombufflength += 1;

		if (eeprombufflength == 8)
		{
		    eeprombufflength = 0;
		    gameeeprom[eepromwrite] = (eeprombuffer & 0xFF);
		    
		    eeprombyteswrite += 1;
		    eepromwrite += 1;
		    eeprombuffer = 0;
		    
		    if (eeprombyteswrite == 8)
		    {
			eeprombyteswrite = 0;
			eepromstate = 5;
		    }
		}
	    }
	    break;
	    case 5:
	    {
		eeprombufflength += 1;

		if (eeprombufflength == 1)
		{
		    eeprombufflength = 0;
		    eepromstate = 3;
		}
	    }
	    break;
	}
    }

    void MMU::softwareinterrupt()
    {
	memarm.swiexception();
    }

    uint8_t MMU::readinterrupts(uint32_t addr)
    {
	uint8_t temp = 0;
	
	switch ((addr & 0xFFF))
	{
	    case 0x200: temp = (iereg & 0xFF); break;
	    case 0x201: temp = (iereg >> 8); break;
	    case 0x202: temp = (ifreg & 0xFF); break;
	    case 0x203: temp = (ifreg >> 8); break;
	    case 0x204: temp = (waitcnt & 0xFF); break;
	    case 0x205: temp = (waitcnt >> 8); break;
	    case 0x206: temp = 0x00; break;
	    case 0x207: temp = 0x00; break;
	    case 0x208: temp = (int)(interruptmasterenable); break;
	    case 0x209: temp = 0x00; break;
	    default: cout << "Unrecognized interrupt read from " << hex << (int)(addr) << endl; temp = 0xFF; break;
	}

	return temp;
    }

    void MMU::writeinterrupts(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x200: iereg = ((iereg & 0xFF00) | val); break;
	    case 0x201: iereg = ((val << 8) | (iereg & 0xFF)); break;
	    case 0x202:
	    {
		for (int i = 0; i < 8; i++)
		{
		    if (TestBit(val, i))
		    {
			clearinterrupt(i);
		    }
		}
	    }
	    break;
	    case 0x203:
	    {
		for (int i = 0; i < 8; i++)
		{
		    if (TestBit(val, (8 + i)))
		    {
			clearinterrupt(i);
		    }
		}
	    }
	    break;
	    case 0x204: waitcnt = ((waitcnt & 0xFF00) | val); break;
	    case 0x205: waitcnt = (((val & 0x7F) << 8) | (waitcnt & 0xFF)); break;
	    case 0x208: interruptmasterenable = TestBit(val, 0); break;
	    case 0x209: return; break;
	    default: cout << "Unrecognized interrupt write to " << hex << (int)(addr) << endl; break;
	}
    }

    uint8_t MMU::readdma(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFF))
	{
	    case 0xB8: temp = 0x00; break;
	    case 0xB9: temp = 0x00; break;
	    case 0xBA: temp = (dma0control & 0xFF); break;
	    case 0xBB: temp = (dma0control >> 8); break;
	    case 0xC4: temp = 0x00; break;
	    case 0xC5: temp = 0x00; break;
	    case 0xC6: temp = (dma1control & 0xFF); break;
	    case 0xC7: temp = (dma1control >> 8); break;
	    case 0xD0: temp = 0x00; break;
	    case 0xD1: temp = 0x00; break;
	    case 0xD2: temp = (dma2control & 0xFF); break;
	    case 0xD3: temp = (dma2control >> 8); break;
	    case 0xDC: temp = 0x00; break;
	    case 0xDD: temp = 0x00; break;
	    case 0xDE: temp = (dma3control & 0xFF); break;
	    case 0xDF: temp = (dma3control >> 8); break;
	    default: cout << "Unrecognized DMA read from " << hex << (int)(addr) << endl; temp = 0xFF; break;
	}

	return temp;
    }

    void MMU::writedma(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFF))
	{
	    case 0xB0: dma0src = ((dma0src & 0xFFFFFF00) | val); break;
	    case 0xB1: dma0src = ((dma0src & 0xFFFF00FF) | (val << 8)); break;
	    case 0xB2: dma0src = ((dma0src & 0xFF00FFFF) | (val << 16)); break;
	    case 0xB3: dma0src = ((dma0src & 0x00FFFFFF) | ((val & 0x7) << 24)); break;
	    case 0xB4: dma0dst = ((dma0dst & 0xFFFFFF00) | val); break;
	    case 0xB5: dma0dst = ((dma0dst & 0xFFFF00FF) | (val << 8)); break;
	    case 0xB6: dma0dst = ((dma0dst & 0xFF00FFFF) | (val << 16)); break;
	    case 0xB7: dma0dst = ((dma0dst & 0x00FFFFFF) | ((val & 0x7) << 24)); break;
	    case 0xB8: dma0length = ((dma0length & 0xFF00) | val); break;
	    case 0xB9: dma0length = ((dma0length & 0xFF) | (val << 8)); break;
	    case 0xBA: dma0control = ((dma0control & 0xFF00) | val); break;
	    case 0xBB: dma0control = ((dma0control & 0xFF) | (val << 8)); initdma(0); break;
	    case 0xBC: dma1src = ((dma1src & 0xFFFFFF00) | val); break;
	    case 0xBD: dma1src = ((dma1src & 0xFFFF00FF) | (val << 8)); break;
	    case 0xBE: dma1src = ((dma1src & 0xFF00FFFF) | (val << 16)); break;
	    case 0xBF: dma1src = ((dma1src & 0x00FFFFFF) | ((val & 0xF) << 24)); dma1fifosrc = dma1src; break;
	    case 0xC0: dma1dst = ((dma1dst & 0xFFFFFF00) | val); break;
	    case 0xC1: dma1dst = ((dma1dst & 0xFFFF00FF) | (val << 8)); break;
	    case 0xC2: dma1dst = ((dma1dst & 0xFF00FFFF) | (val << 16)); break;
	    case 0xC3: dma1dst = ((dma1dst & 0x00FFFFFF) | ((val & 0x7) << 24)); break;
	    case 0xC4: dma1length = ((dma1length & 0xFF00) | val); break;
	    case 0xC5: dma1length = ((dma1length & 0xFF) | (val << 8)); break;
	    case 0xC6: dma1control = ((dma1control & 0xFF00) | val); break;
	    case 0xC7: dma1control = ((dma1control & 0xFF) | (val << 8)); initdma(1); break;
	    case 0xC8: dma2src = ((dma2src & 0xFFFFFF00) | val); break;
	    case 0xC9: dma2src = ((dma2src & 0xFFFF00FF) | (val << 8)); break;
	    case 0xCA: dma2src = ((dma2src & 0xFF00FFFF) | (val << 16)); break;
	    case 0xCB: dma2src = ((dma2src & 0x00FFFFFF) | ((val & 0xF) << 24)); dma2fifosrc = dma2src; break;
	    case 0xCC: dma2dst = ((dma2dst & 0xFFFFFF00) | val); break;
	    case 0xCD: dma2dst = ((dma2dst & 0xFFFF00FF) | (val << 8)); break;
	    case 0xCE: dma2dst = ((dma2dst & 0xFF00FFFF) | (val << 16)); break;
	    case 0xCF: dma2dst = ((dma2dst & 0x00FFFFFF) | ((val & 0x7) << 24)); break;
	    case 0xD0: dma2length = ((dma2length & 0xFF00) | val); break;
	    case 0xD1: dma2length = ((dma2length & 0xFF) | (val << 8)); break;
	    case 0xD2: dma2control = ((dma2control & 0xFF00) | val); break;
	    case 0xD3: dma2control = ((dma2control & 0xFF) | (val << 8)); initdma(2); break;
	    case 0xD4: dma3src = ((dma3src & 0xFFFFFF00) | val); break;
	    case 0xD5: dma3src = ((dma3src & 0xFFFF00FF) | (val << 8)); break;
	    case 0xD6: dma3src = ((dma3src & 0xFF00FFFF) | (val << 16)); break;
	    case 0xD7: dma3src = ((dma3src & 0x00FFFFFF) | ((val & 0xF) << 24)); break;
	    case 0xD8: dma3dst = ((dma3dst & 0xFFFFFF00) | val); break;
	    case 0xD9: dma3dst = ((dma3dst & 0xFFFF00FF) | (val << 8)); break;
	    case 0xDA: dma3dst = ((dma3dst & 0xFF00FFFF) | (val << 16)); break;
	    case 0xDB: dma3dst = ((dma3dst & 0x00FFFFFF) | ((val & 0xF) << 24)); break;
	    case 0xDC: dma3length = ((dma3length & 0xFF00) | val); break;
	    case 0xDD: dma3length = ((dma3length & 0xFF) | (val << 8)); break;
	    case 0xDE: dma3control = ((dma3control & 0xFF00) | val); break;
	    case 0xDF: dma3control = ((dma3control & 0xFF) | (val << 8)); initdma(3); break;
	    default: cout << "Unrecognized DMA write to " << hex << (int)(addr) << ", value of " << hex << (int)(val) << endl; return; break;
	}
    }

    bool MMU::savebackup(string filename)
    {
	bool success = false;
	ofstream file(filename.c_str(), ios::out | ios::binary);

	if (file.is_open())
	{
	    switch (carttype)
	    {
		case CartridgeType::CartSRAM:
		{
		    file.write((char*)gamesram.data(), gamesram.size());
		}
		break;
		case CartridgeType::CartEEPROM:
		{
		    file.write((char*)gameeeprom.data(), gameeeprom.size());
		}
		break;
		case CartridgeType::CartFlash128K:
		{
		    file.write((char*)gameflash.data(), gameflash.size());
		}
		break;
		case CartridgeType::CartFlash64K:
		{
		    file.write((char*)gameflash.data(), gameflash.size());
		}
		break;
		case CartridgeType::None: break;
	    }

	    cout << "MMU::Backup save succesfully written." << endl;
	    file.close();
	    success = true; 
	}
	else
	{
	    cout << "MMU::Error - Backup save could not be written." << endl;
	    success = false;
	}

	return success;
    }

    bool MMU::loadbackup(string filename)
    {
	bool success = false;
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    file.seekg(0, ios::beg);

	    switch (carttype)
	    {
		case CartridgeType::CartSRAM:
		{
		    gamesram.resize(size, 0);
		    file.read((char*)gamesram.data(), size);
		}
		break;
		case CartridgeType::CartEEPROM:
		{
		    gameeeprom.resize(size, 0);
		    file.read((char*)gameeeprom.data(), size);
		}
		break;
		case CartridgeType::CartFlash128K:
		{
		    gameflash.resize(size, 0xFF);
		    file.read((char*)gameflash.data(), size);
		}
		break;
		case CartridgeType::CartFlash64K:
		{
		    gameflash.resize(size, 0xFF);
		    file.read((char*)gameflash.data(), size);
		}
		break;
		case CartridgeType::None: break;
	    }

	    cout << "MMU::Backup save succesfully loaded." << endl;
	    file.close();
	    success = true; 
	}
	else
	{
	    cout << "MMU::Error - Backup save could not be loaded." << endl;
	    success = false;
	}

	return success;
    }

    bool MMU::loadROM(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    gamerom.resize(0x2000000, 0);
	    file.seekg(0, ios::beg);
	    file.read((char*)gamerom.data(), size);

	    carttype = getcarttype(gamerom);

	    switch (carttype)
	    {
		case CartridgeType::CartSRAM:
		{
		    gamesram.resize(0x8000, 0);
		}
		break;
		case CartridgeType::CartEEPROM:
		{
		    gameeeprom.resize(0x200, 0);
		}
		break;
		case CartridgeType::CartFlash128K:
		{
		    gameflash.resize(0x20000, 0xFF);
		}
		break;
		case CartridgeType::CartFlash64K:
		{
		    gameflash.resize(0x10000, 0xFF);
		}
		break;
		case CartridgeType::None: break;
	    }

	    rtcenable = isrtcsupported(gamerom);
	    solarenable = issolarsupported(gamerom);

	    cout << "File succesfully loaded." << endl;
	    file.close();
	    return true;
	}
	else
	{
	    cout << "Error" << endl;
	    return false;
	}
    }

    bool MMU::loadBIOS(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();

	    if (size != 0x4000)
	    {
		cout << "BIOS size doesn't match" << endl;
		return false;
	    }

	    file.seekg(0, ios::beg);
	    file.read((char*)&bios[0], size);
	    cout << "File succesfully loaded." << endl;
	    file.close();
	    return true;
	}
	else
	{
	    cout << "Error" << endl;
	    return false;
	}
    }
};
