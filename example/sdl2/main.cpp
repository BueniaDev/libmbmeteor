#include <libmbmeteor/mmu.h>
#include <libmbmeteor/cpu.h>
using namespace gba;

MMU mmu;
CPU cpu(mmu);

int main(int argc, char* argv[])
{
    if (!mmu.loadROM(argv[1]))
    {
	return 1;
    }

    cpu.init();

    int overspentcycles = cpu.runfor(279666 + overspentcycles);


    cpu.shutdown();
    return 0;
}
