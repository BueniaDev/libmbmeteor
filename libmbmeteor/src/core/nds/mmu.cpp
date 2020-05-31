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

#include "../../../include/libmbmeteor/nds/mmu.h"
using namespace nds;
using namespace std;
using namespace std::placeholders;

namespace nds
{
    NDSMMU::NDSMMU()
    {
	for (int i = 0x180; i < 0x18C; i++)
	{
	    addmemoryreadhandler9((0x4000000 + i), bind(&NDSMMU::readipc9, this, _1));
	    addmemorywritehandler9((0x4000000 + i), bind(&NDSMMU::writeipc9, this, _1, _2));

	    addmemoryreadhandler7((0x4000000 + i), bind(&NDSMMU::readipc7, this, _1));
	    addmemorywritehandler7((0x4000000 + i), bind(&NDSMMU::writeipc7, this, _1, _2));
	}

	for (int i = 0x1A0; i < 0x1C4; i++)
	{
	    addmemoryreadhandler7((0x4000000 + i), bind(&NDSMMU::readspi7, this, _1));
	    addmemorywritehandler7((0x4000000 + i), bind(&NDSMMU::writespi7, this, _1, _2));
	}

	for (int i = 0x208; i < 0x218; i++)
	{
	    addmemoryreadhandler7((0x4000000 + i), bind(&NDSMMU::readinterrupts7, this, _1));
	    addmemorywritehandler7((0x4000000 + i), bind(&NDSMMU::writeinterrupts7, this, _1, _2));
	}

	addmemoryreadhandler7(0x4000301, bind(&NDSMMU::readinterrupts7, this, _1));
	addmemorywritehandler7(0x4000301, bind(&NDSMMU::writeinterrupts7, this, _1, _2));

	mainmem9.resize(0x1000000, 0);
	mainmem7.resize(0x1000000, 0);
	wram7.resize(0x800000, 0);
	
	mapsharedwram(0);
    }

    NDSMMU::~NDSMMU()
    {

    }

    uint8_t NDSMMU::readByte9(uint32_t addr)
    {
	uint8_t temp = 0;

	uint8_t addrtemp = (addr >> 24);

	if ((addr >= arm9dtcmbase) && (addr < (arm9dtcmbase + arm9dtcmsize)))
	{
	    temp = arm9dtcm[((addr - arm9dtcmbase) & 0x3FFF)];
	    return temp;
	}

	switch (addrtemp)
	{
	    case 0x02:
	    {
		temp = mainmem9[(addr & 0xFFFFFF)];
	    }
	    break;
	    case 0x03:
	    {
	    	if (swram_ptr7 != NULL)
	    	{
	    	    temp = *(uint8_t*)&swram_ptr9[(addr & swram_mask9)];
	    	}
	   	else
	    	{
	    	    temp = 0;
	    	}
	    }
	    break;
	    case 0x04:
	    {
		// cout << "ARM9 I/O read, register of " << hex << (int)(addr & 0xFFFFFF) << endl;
		if (memoryreadhandlers9[(addr & 0xFFFFFF)])
		{
		    temp = memoryreadhandlers9[(addr & 0xFFFFFF)](addr);
		}
		else
		{
		    temp = 0x00;
		}
	    }
	    break;
	    case 0xFF: temp = bios9[(addr & 0xFFF)]; break;
	    default: cout << "Unrecognized ARM9 read from region of " << hex << (int)(addrtemp) << endl; exit(1); break;
	}

	return temp;
    }

    void NDSMMU::writeByte9(uint32_t addr, uint8_t val)
    {
	uint8_t addrtemp = (addr >> 24);

	if ((addr >= arm9dtcmbase) && (addr < (arm9dtcmbase + arm9dtcmsize)))
	{
	    arm9dtcm[((addr - arm9dtcmbase) & 0x3FFF)] = val;
	    return;
	}

	switch (addrtemp)
	{
	    case 0x02: mainmem9[(addr & 0xFFFFFF)] = val; break;
	    case 0x03:
	    {
	    	if (swram_ptr9 != NULL)
	    	{
	    	    *(uint8_t*)&swram_ptr9[(addr & swram_mask9)] = val;
	    	}
	    	else
	    	{
	    	    return;
	    	}
	    }
	    break;
	    case 0x04:
	    {
		// cout << "ARM9 I/O write, register of " << hex << (int)(addr & 0xFFFFFF) << endl;

		if (memorywritehandlers9[(addr & 0xFFFFFF)])
		{
		    memorywritehandlers9[(addr & 0xFFFFFF)](addr, val);
		}
	    }
	    break;
	    case 0xFF: return; break;
	    default: cout << "Unrecognized ARM9 write to region of " << hex << (int)(addrtemp) << endl; memarm9.printregs(); exit(1); break;
	}
    }

    uint16_t NDSMMU::readWord9(uint32_t addr)
    {
	return ((readByte9(addr + 1) << 8) | (readByte9(addr)));
    }

    void NDSMMU::writeWord9(uint32_t addr, uint16_t val)
    {
	writeByte9(addr, (val & 0xFF));
	writeByte9((addr + 1), (val >> 8));
    }

    uint32_t NDSMMU::readLong9(uint32_t addr)
    {
	return ((readWord9(addr + 2) << 16) | (readWord9(addr)));
    }

    void NDSMMU::writeLong9(uint32_t addr, uint32_t val)
    {
	writeWord9(addr, (val & 0xFFFF));
	writeWord9((addr + 2), (val >> 16));
    }

    uint8_t NDSMMU::readByte7(uint32_t addr)
    {
	uint8_t temp = 0;

	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
	    case 0x00: temp = bios7[(addr & 0xFFFFFF)]; break;
	    case 0x02: temp = mainmem7[(addr & 0xFFFFFF)]; break;
	    case 0x03:
	    {
		if ((addr & 0xFFFFFF) < 0x800000)
		{
		    if (swram_ptr7 != NULL)
	    	    {
	    		temp = *(uint8_t*)&swram_ptr7[(addr & swram_mask7)];
	    	    }
	   	    else
	    	    {
	    		temp = 0;
	    	    }
		}
		else
		{
		    temp = wram7[(addr & 0xFFFF)];
		}
	    }
	    break;
	    case 0x04:
	    {
		// cout << "ARM7 I/O read, register of " << hex << (int)(addr & 0xFFFFFF) << endl;

		if (memoryreadhandlers7[(addr & 0xFFFFFF)])
		{
		    temp = memoryreadhandlers7[(addr & 0xFFFFFF)](addr);
		}
		else
		{
		    temp = 0x00;
		}
	    }
	    break;
	    default: cout << "Unrecognized ARM7 read from region of " << hex << (int)(addrtemp) << endl; exit(1); break;
	}

	return temp;
    }

    void NDSMMU::writeByte7(uint32_t addr, uint8_t val)
    {
	uint8_t addrtemp = (addr >> 24);

	switch (addrtemp)
	{
	    case 0x00: return; break;
	    case 0x02: mainmem9[(addr & 0xFFFFFF)] = val; break;
	    case 0x03:
	    {
		if ((addr & 0xFFFFFF) < 0x800000)
		{
		    if (swram_ptr7 != NULL)
	    	    {
	    		*(uint8_t*)&swram_ptr7[(addr & swram_mask7)] = val;
	    	    }
	   	    else
	    	    {
	    		return;
	    	    }
		}
		else
		{
		    wram7[(addr & 0xFFFF)] = val;
		}
	    }
	    break;
	    case 0x04:
	    {
		// cout << "ARM7 I/O write, register of " << hex << (int)(addr & 0xFFFFFF) << endl;
		if (memorywritehandlers7[(addr & 0xFFFFFF)])
		{
		    memorywritehandlers7[(addr & 0xFFFFFF)](addr, val);
		}
	    }
	    break;
	    default: cout << "Unrecognized ARM7 write to region of " << hex << (int)(addrtemp) << endl; exit(1); break;
	}
    }

    uint16_t NDSMMU::readWord7(uint32_t addr)
    {
	return ((readByte7(addr + 1) << 8) | (readByte7(addr)));
    }

    void NDSMMU::writeWord7(uint32_t addr, uint16_t val)
    {
	writeByte7(addr, (val & 0xFF));
	writeByte7((addr + 1), (val >> 8));
    }

    uint32_t NDSMMU::readLong7(uint32_t addr)
    {
	return ((readWord7(addr + 2) << 16) | (readWord7(addr)));
    }

    void NDSMMU::writeLong7(uint32_t addr, uint32_t val)
    {
	writeWord7(addr, (val & 0xFFFF));
	writeWord7((addr + 2), (val >> 16));
    }

    void NDSMMU::softwareinterrupt9()
    {
	memarm9.swiexception(0xFFFF0000);
    }

    uint8_t NDSMMU::readipc9(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFF))
	{
	    case 0x180: temp = (ipcsync9 & 0xFF); break;
	    case 0x181: temp = (ipcsync9 >> 8); break;
	    case 0x182: temp = 0x00; break;
	    case 0x183: temp = 0x00; break;
	    default: cout << "Unrecognized ARM9 IPC read from address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); temp = 0; break;
	}

	return temp;
    }

    void NDSMMU::writeipc9(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x180: return; break;
	    case 0x181:
	    {
		ipcsync7 &= 0xFFF0;
		ipcsync7 |= (val & 0xF);

		ipcsync9 &= 0xFF;
		ipcsync9 |= ((val & 0x6F) << 8);

		if (TestBit(ipcsync9, 13) && TestBit(ipcsync7, 14))
		{
		    cout << "ARM7 IRQ" << endl;
		    exit(1);
		}
	    }
	    break;
	    case 0x182: return; break;
	    case 0x183: return; break;
	    default: cout << "Unrecognized ARM9 IPC write to address of " << hex << (int)((addr & 0xFFF)) << ", value of " << hex << (int)(val) << endl; exit(1); break;
	}
    }

    uint8_t NDSMMU::readipc7(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFF))
	{
	    case 0x180: temp = (ipcsync7 & 0xFF); break;
	    case 0x181: temp = (ipcsync7 >> 8); break;
	    case 0x182: temp = 0x00; break;
	    case 0x183: temp = 0x00; break;
	    default: cout << "Unrecognized ARM7 IPC read from address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); temp = 0; break;
	}

	return temp;
    }

    void NDSMMU::writeipc7(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFF))
	{
	    case 0x180: return; break;
	    case 0x181:
	    {
		ipcsync9 &= 0xFFF0;
		ipcsync9 |= (val & 0xF);

		ipcsync7 &= 0xFF;
		ipcsync7 |= ((val & 0x6F) << 8);

		if (TestBit(ipcsync7, 13) && TestBit(ipcsync9, 14))
		{
		    cout << "ARM9 IRQ" << endl;
		    exit(1);
		}
	    }
	    break;
	    case 0x182: return; break;
	    case 0x183: return; break;
	    default: cout << "Unrecognized ARM7 IPC write to address of " << hex << (int)((addr & 0xFFF)) << ", value of " << hex << (int)(val) << endl; memarm7.printregs(); exit(1); break;
	}
    }

    uint8_t NDSMMU::readspi7(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFFFFF))
	{
	    case 0x1A4: temp = 0x00; break;
	    case 0x1A5: temp = 0x00; break;
	    case 0x1A6: temp = 0x80; break;
	    case 0x1A7: temp = 0x00; break;
	    case 0x1C0: temp = (spicontrol & 0xFF); break;
	    case 0x1C1: temp = (spicontrol >> 8); break;
	    case 0x1C2: temp = readspidata(); break;
	    case 0x1C3: temp = 0x00; break;
	    default: cout << "Unrecognized ARM7 SPI read from address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); temp = 0; break;
	}

	return temp;
    }

    void NDSMMU::writespi7(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFFFFF))
	{
	    case 0x1A0: return; break;
	    case 0x1A1: return; break;
	    case 0x1A4: return; break;
	    case 0x1A5: return; break;
	    case 0x1A6: return; break;
	    case 0x1A7: return; break;
	    case 0x1A8: return; break;
	    case 0x1A9: return; break;
	    case 0x1AA: return; break;
	    case 0x1AB: return; break;
	    case 0x1AC: return; break;
	    case 0x1AD: return; break;
	    case 0x1AE: return; break;
	    case 0x1AF: return; break;
	    case 0x1C0: spicontrol = ((spicontrol & 0xFF00) | (val & 0x3)); break;
	    case 0x1C1: spicontrol = ((spicontrol & 0xFF) | ((val & 0xCF) << 8)); break;
	    case 0x1C2: writespidata(val); break;
	    case 0x1C3: return; break;
	    default: cout << "Unrecognized ARM7 SPI write to address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); return; break;
	}
    }

    uint8_t NDSMMU::readinterrupts7(uint32_t addr)
    {
	uint8_t temp = 0;

	switch ((addr & 0xFFFFFF))
	{
	    case 0x208: temp = interruptmasterenable7; break;
	    case 0x209: temp = 0x00; break;
	    case 0x20A: temp = 0x00; break;
	    case 0x20B: temp = 0x00; break;
	    case 0x210: temp = (iereg7 & 0xFF); break;
	    case 0x211: temp = ((iereg7 >> 8) & 0xFF); break;
	    case 0x212: temp = ((iereg7 >> 16) & 0xFF); break;
	    case 0x213: temp = (iereg7 >> 24); break;
	    default: cout << "Unrecognized ARM7 interrupt read from address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); temp = 0; break;
	}

	return temp;
    }

    void NDSMMU::writeinterrupts7(uint32_t addr, uint8_t val)
    {
	switch ((addr & 0xFFFFFF))
	{
	    case 0x208: interruptmasterenable7 = TestBit(val, 0); break;
	    case 0x209: return; break;
	    case 0x20A: return; break;
	    case 0x20B: return; break;
	    case 0x210: iereg7 = ((iereg7 & 0xFFFFFF00) | val); break;
	    case 0x211: iereg7 = ((iereg7 & 0xFFFF00FF) | ((val & 0x3F) << 8)); break;
	    case 0x212: iereg7 = ((iereg7 & 0xFF00FFFF) | (val << 16)); break;
	    case 0x213: iereg7 = ((iereg7 & 0x00FFFFFF) | ((val & 0x1) << 24)); break;
	    case 0x214:
	    case 0x215:
	    case 0x216:
	    case 0x217:
	    {
		for (int i = 0; i < 8; i++)
		{
		    int temp = ((addr & 3) << 3);

		    if (TestBit(ifreg7, (temp + i)))
		    {
			clearinterrupt7((temp + i));
		    }
		}
	    }
	    break;
	    case 0x301:
	    {
		if (val != 0x80)
		{
		    return;
		}

		setinterrupt7(19);
	    }
	    break;
	    default: cout << "Unrecognized ARM7 interrupt write to address of " << hex << (int)((addr & 0xFFF)) << endl; exit(1); return; break;
	}
    }

    void NDSMMU::softwareinterrupt7()
    {
	memarm7.swiexception();
    }

    uint32_t NDSMMU::readcp15(uint16_t id)
    {
	uint32_t temp = 0;

	if ((id >> 12) != 0xF)
	{
	    temp = 0;
	}
	else
	{
	    switch ((id & 0xFFF))
	    {
		case 0x100: temp = cp15ctrl; break;
		case 0x910: temp = dtcmsetting; break;
		default: cout << "Unrecognized CP15 read with ID of " << hex << (int)((id & 0xFFF)) << endl; exit(1); break;
	    }
	}

	return temp;
    }

    void NDSMMU::writecp15(uint16_t id, uint32_t val)
    {
	if ((id >> 12) != 0xF)
	{
	    return;
	}
	else
	{
	    switch ((id & 0xFFF))
	    {
		case 0x100:
		{
		    cp15ctrl &= ~0xFF085;
		    cp15ctrl |= (val & 0xFF085);
		    updatedtcmsetting();
		    updateitcmsetting();
		}
		break;
		case 0x750: return; break;
		case 0x760: return; break;
		case 0x7A4: return; break;
		case 0x910:
		{
		    dtcmsetting = val;
		    updatedtcmsetting();
		}
		break;
		default: cout << "Unrecognized CP15 ID of " << hex << (int)((id & 0xFFF)) << endl; exit(1); break;
	    }
	}
    }

    bool NDSMMU::loadBIOS9(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    file.seekg(0, ios::beg);
	    bios9.resize(size, 0);
	    file.read((char*)bios9.data(), size);
	    cout << "Success!" << endl;
	    file.close();
	    return true;
	}
	else
	{
	    cout << "Error" << endl;
	    return false;
	}
    }

    bool NDSMMU::loadBIOS7(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    file.seekg(0, ios::beg);
	    bios7.resize(size, 0);
	    file.read((char*)bios7.data(), size);
	    cout << "Success!" << endl;
	    file.close();
	    return true;
	}
	else
	{
	    cout << "Error" << endl;
	    return false;
	}
    }

    bool NDSMMU::loadfirmware(string filename)
    {
	ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);

	if (file.is_open())
	{
	    streampos size = file.tellg();
	    file.seekg(0, ios::beg);
	    firmware.resize(size, 0);
	    file.read((char*)firmware.data(), size);
	    cout << "Success!" << endl;
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
