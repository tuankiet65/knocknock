#include "cpu/register.h"
#include "memory/memory.h"

int main(int, char**) {
    cpu::Register reg;
    reg.set_a(0xff);
    reg.set_b(0xff);
    memory::Memory memory;
}
