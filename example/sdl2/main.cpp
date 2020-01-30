#include <libmbmeteor/mmu.h>
#include <libmbmeteor/cpu.h>
#include <libmbmeteor/gpu.h>
using namespace gba;

MMU mmu;
GPU gpu(mmu);
CPU cpu(mmu, gpu);

int main(int argc, char* argv[])
{
	mmu.init();
	
    if (!mmu.loadROM(argv[1]))
    {
	return 1;
    }

    cpu.init();

    int overspentcycles = cpu.runfor(279666 + overspentcycles);


    cpu.shutdown();
	mmu.shutdown();
    return 0;
}
