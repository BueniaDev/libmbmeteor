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

#include "../../include/libmbmeteor/mmu.h"
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
		pram.resize(0x400, 0);
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
	addr = (addr % 0x10000000);

	uint8_t addrtemp = ((addr >> 24) & 0xF);

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
		case 0x1:
		{
			temp = 0x00;
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
			temp = pram[(addr & 0xFFF)];
		}
		break;
		case 0x6:
		{
			temp = vram[(addr & 0xFFFFFF)];
		}
		break;
		case 0x7:
		{
			temp = oam[(addr & 0xFFF)];
		}
		break;
	    case 0x8:
	    {
		temp = gamerom[(addr & 0xFFFFFF)];
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
		else
		{
		    temp = 0xFF;
		}
	    }
	    break;
	    default: cout << "Unrecognized read from region of " << hex << (int)(addrtemp) << endl; dump = true; temp = 0xFF; break;
	}

	return temp;
    }

    void MMU::writeByte(uint32_t addr, uint8_t val)
    {
	addr = (addr % 0x10000000);

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
			pram[(addr & 0xFFF)] = val;
		}
		break;
		case 0x6:
		{
			vram[(addr & 0xFFFFFF)] = val;
		}
		break;
		case 0x7:
		{
			oam[(addr & 0xFFF)] = val;
		}
		break;
	    case 0x8:
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
	    }
	    break;
	    case 0xF:
	    {
		return;
	    }
	    break;
	    default: cout << "Unrecognized write to region of " << hex << (int)(addrtemp) << endl; break;
	}
    }

    uint16_t MMU::readWord(uint32_t addr)
    {
	return ((readByte(addr + 1) << 8) | (readByte(addr)));
    }

    void MMU::writeWord(uint32_t addr, uint16_t val)
    {
	writeByte(addr, (val & 0xFF));
	writeByte((addr + 1), (val >> 8));
    }

    uint32_t MMU::readLong(uint32_t addr)
    {
	return ((readWord(addr + 2) << 16) | (readWord(addr)));
    }

    void MMU::writeLong(uint32_t addr, uint32_t val)
    {
	writeWord(addr, (val & 0xFFFF));
	writeWord((addr + 2), (val >> 16));
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
	    case 0x208: temp = (int)(interruptmasterenable); break;
	    case 0x209: temp = 0x00; break;
	    default: cout << "Unrecognized interrupt read from " << hex << (int)(addr) << endl; temp = 0xFF; memarm.printregs(); exit(1); break;
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
	    case 0xDF: dma3control = ((dma3control & 0xFF) | (val << 8)); initdma3(); break;
	    default: cout << "Unrecognized DMA write to " << hex << (int)(addr) << endl; return; break;
	}
    }

    bool MMU::loadROM(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    gamerom.resize(size, 0);
	    file.seekg(0, ios::beg);
	    file.read((char*)&gamerom[0], size);

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
		case CartridgeType::None: break;
	    }

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
