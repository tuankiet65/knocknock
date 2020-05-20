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

set(UNITTEST_FILES
        "${SOURCE_FILES}"
        "cpu/operands_unittest.cpp"
        memory/memory_unittest.cpp
        memory/unittest_utils.cpp
        memory/flat_rom_unittest.cpp
        memory/mbc1_unittest.cpp
        memory/mbc2_unittest.cpp
        memory/internal_ram_unittest.cpp
        peripherals/clock_unittest.cpp
        peripherals/serial_unittest.cpp
        peripherals/joypad_unittest.cpp)
add_executable(knocknock_unittest "${UNITTEST_FILES}" "knocknock_unittest.cpp")
target_include_directories(knocknock_unittest PRIVATE .)
target_link_libraries(knocknock_unittest
        Catch2::Catch2
        glog::glog
        fmt::fmt)
target_compile_options(knocknock_unittest PRIVATE -Wall -Wextra -pedantic)
target_compile_features(knocknock_unittest PRIVATE cxx_std_17)
