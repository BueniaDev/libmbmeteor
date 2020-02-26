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

#include "../../include/libmbmeteor/libmbmeteor.h"
using namespace gba;
using namespace std;

namespace gba
{
    GBACore::GBACore()
    {
	coremmu = make_unique<MMU>();
	coregpu = make_unique<GPU>(*coremmu);
	coreinput = make_unique<Input>(*coremmu);
	corecpu = make_unique<CPU>(*coremmu, *coregpu);
    }

    GBACore::~GBACore()
    {

    }

    bool GBACore::init()
    {
	coremmu->init();

	if (!loadBIOS(biosname))
	{
	    return false;
	}

	if (!loadROM(romname))
	{
	    return false;
	}

	corecpu->init();
	coregpu->init();
	return true;
    }

    void GBACore::shutdown()
    {
	coregpu->shutdown();
	corecpu->shutdown();
	coremmu->shutdown();
    }

    void GBACore::printusage(char* argv)
    {
	cout << "Usage: " << argv << " ROM [options]" << endl;
	cout << endl;
	cout << "Options:" << endl;
    }

    bool GBACore::getoptions(int argc, char* argv[])
    {
	if (argc < 3)
	{
	    printusage(argv[0]);
	    return false;
	}

	for (int i = 1; i < argc; i++)
	{
	    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
	    {
		printusage(argv[0]);
		return false;
	    }
	}

	romname = argv[1];
	biosname = argv[2];

	return true;
    }

    bool GBACore::loadROM(string filename)
    {
	return coremmu->loadROM(filename);
    }

    bool GBACore::loadBIOS(string filename)
    {
	return coremmu->loadBIOS(filename);
    }

    RGB GBACore::getpixel(int x, int y)
    {
	return coregpu->framebuffer[(x + (y * 240))];
    }

    void GBACore::keypressed(Button button)
    {
	coreinput->keypressed(button);
    }

    void GBACore::keyreleased(Button button)
    {
	coreinput->keyreleased(button);
    }

    void GBACore::runcore()
    {
	overspentcycles = corecpu->runfor(280897 + overspentcycles);
    }
};
