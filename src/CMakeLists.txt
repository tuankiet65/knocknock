set(SOURCE_FILES
        "cpu/cpu.cpp"
        "cpu/instruction.cpp"
        "cpu/decoder.cpp"
        "memory/memory.cpp"
        "memory/test_memory.cpp"
        "memory/mmu.cpp"
        "memory/flat_rom.cpp"
        "memory/mbc1.cpp"
        "memory/mbc2.cpp"
        memory/internal_ram.cpp
        "cpu/operands.cpp"
        "cartridge.cpp"
        peripherals/clock.cpp
        "peripherals/serial.cpp"
        peripherals/joypad.cpp)
add_executable(knocknock "${SOURCE_FILES}" "knocknock.cpp")
target_include_directories(knocknock PRIVATE .)
target_link_libraries(knocknock
        glog::glog
        fmt::fmt)
target_compile_options(knocknock PRIVATE -Wall -Wextra -pedantic)
target_compile_features(knocknock PRIVATE cxx_std_17)

add_subdirectory(unittests)
