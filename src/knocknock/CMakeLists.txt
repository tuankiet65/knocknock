set(SOURCE_FILES
        cartridge.cpp
        cpu/cpu.cpp
        cpu/instruction.cpp
        cpu/decoder.cpp
        cpu/operands.cpp
        memory/memory.cpp
        memory/test_memory.cpp
        memory/mmu.cpp
        memory/flat_rom.cpp
        memory/mbc1.cpp
        memory/mbc2.cpp
        memory/internal_ram.cpp
        peripherals/clock.cpp
        peripherals/serial.cpp
        peripherals/joypad.cpp)

add_library(knocknock STATIC "${SOURCE_FILES}")
target_include_directories(knocknock
        PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(knocknock PRIVATE glog::glog fmt::fmt)

target_compile_features(knocknock PRIVATE cxx_std_17)

