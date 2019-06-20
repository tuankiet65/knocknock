cmake_minimum_required(VERSION 3.0.0)
project(knocknock VERSION 0.0.1)

include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCE_FILES
    "cpu/cpu.cpp"
    "cpu/register.cpp"
    "memory/memory.cpp"
    "memory/mmu.cpp"
    "memory/ram.cpp")
add_executable(knocknock "${SOURCE_FILES}" "knocknock.cpp")

set(UNITTEST_FILES
    "${SOURCE_FILES}"
    "cpu/register_unittest.cpp")
add_executable(knocknock_unittest "${UNITTEST_FILES}" "knocknock_unittest.cpp")
option(USE_SYSTEM_CATCH2 "If enabled, use system supplied Catch2 instead of the included version in third_party/" OFF)
if (USE_SYSTEM_CATCH2)
    find_package(Catch2 REQUIRED)
else()
    add_subdirectory("third_party/Catch2")
endif()
target_link_libraries(knocknock_unittest Catch2::Catch2)
