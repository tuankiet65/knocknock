set(UNITTEST_FILES
        interrupt_unittest.cpp
        cpu/operands_unittest.cpp
        memory/memory_unittest.cpp
        memory/unittest_utils.cpp
        memory/flat_rom_unittest.cpp
        memory/mbc1_unittest.cpp
        memory/mbc2_unittest.cpp
        memory/internal_ram_unittest.cpp
        peripherals/clock_unittest.cpp
        peripherals/serial_unittest.cpp
        peripherals/joypad_unittest.cpp
        ppu/dma_unittest.cpp)
add_executable(knocknock_unittest "${UNITTEST_FILES}" "knocknock_unittest.cpp")
target_include_directories(knocknock_unittest PRIVATE .)
target_link_libraries(knocknock_unittest PRIVATE
        knocknock
        Catch2::Catch2
        glog::glog
        fmt::fmt)
target_compile_features(knocknock_unittest PRIVATE cxx_std_17)
target_compile_definitions(knocknock_unittest PRIVATE CATCH_CONFIG_FAST_COMPILE)

# MSVC sets the program stack size to 1MB by default, but the unittest
# allocates quite a lot stack memory, so we increase it to 10MB.
if (MSVC)
    target_link_options(knocknock_unittest PRIVATE /STACK:10485760)
endif ()
