cmake_minimum_required(VERSION 3.0.0)
project(knocknock VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCE_FILES
    # "cpu/cpu.cpp"
    # "cpu/instructions.cpp"
    "memory/memory.cpp"
    "memory/test_memory.cpp"
    # "memory/mmu.cpp"
    "cpu/operands.cpp"
    "cartridge.cpp")
add_executable(knocknock "${SOURCE_FILES}" "knocknock.cpp")
option(USE_SYSTEM_GLOG "If enabled, use system supplied glog instead of the included version in third_party/" OFF)
set(WITH_GFLAGS OFF CACHE BOOL "Disable gflags")
set(BUILD_TESTING OFF CACHE BOOL "Disable building unittests for glog")
if (USE_SYSTEM_GLOG)
    find_package(glog REQUIRED)
else()
    add_subdirectory("third_party/glog")
endif()
target_link_libraries(knocknock glog::glog)
option(USE_SYSTEM_FMT "If enabled, use system supplied fmt instead of the included version in third_party/" OFF)
if (USE_SYSTEM_FMT)
    find_package(fmt REQUIRED)
else()
    add_subdirectory("third_party/fmt")
endif()
target_link_libraries(knocknock fmt::fmt)

set(UNITTEST_FILES
    "${SOURCE_FILES}"
    "cpu/operands_unittest.cpp")
add_executable(knocknock_unittest "${UNITTEST_FILES}" "knocknock_unittest.cpp")
option(USE_SYSTEM_CATCH2 "If enabled, use system supplied Catch2 instead of the included version in third_party/" OFF)
if (USE_SYSTEM_CATCH2)
    find_package(Catch2 REQUIRED)
else()
    add_subdirectory("third_party/Catch2")
endif()
target_link_libraries(knocknock_unittest Catch2::Catch2)
target_link_libraries(knocknock_unittest glog::glog)
target_link_libraries(knocknock_unittest fmt::fmt)
