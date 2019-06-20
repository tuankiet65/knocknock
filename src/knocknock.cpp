#include "cpu/register.h"
#include "memory/mmu.h"
#include "memory/ram.h"

int main(int, char**) {
    cpu::Register reg;
    reg.set_a(0xff);
    reg.set_b(0xff);

    memory::MMU mmu;

    memory::RAM ram;
    mmu.add_region(1, &ram, ram.START_ADDR, ram.END_ADDR);
}
