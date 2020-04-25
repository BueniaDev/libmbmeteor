#include <libmbmeteor/libmbmeteor.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include <cassert>
using namespace gba;
using namespace std;
using namespace std::placeholders;

mbMeteor ndscore;

int main()
{
    ndscore.loadBIOS("bios9.bin", "bios7.bin");
    ndscore.loadfirmware("firmware.bin");
    ndscore.nds.corecpu->init();

    for (int i = 0; i < 20; i++)
    {
	ndscore.runcore();
    }
}
