cmake_minimum_required(VERSION 3.9.0)
project(knocknock
        VERSION 0.1
        LANGUAGES CXX)

# Enable all warnings. Unfortunately MSVC and GCC/Clang require different flags.
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

# Force MSVC to correctly report the value of __cplusplus.
if (MSVC)
    add_compile_options(-Zc:__cplusplus)
endif ()

if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT error LANGUAGES CXX)

    if (LTO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        # Force enable policy CMP0069 to force third party dependencies to honor the
        # INTERPROCEDURAL_OPTIMIZATION flag.
        set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
        message(STATUS "LTO enabled for all targets")
    else ()
        message(STATUS "LTO not available: ${error}")
    endif ()
endif ()

option(USE_SYSTEM_GLOG "If enabled, use system supplied glog instead of the included version in third_party/" OFF)
if (USE_SYSTEM_GLOG)
    find_package(glog REQUIRED)
else ()
    set(WITH_GFLAGS OFF)
    set(BUILD_TESTING OFF)
    add_subdirectory("third_party/glog")
endif ()

option(USE_SYSTEM_FMT "If enabled, use system supplied fmt instead of the included version in third_party/" OFF)
if (USE_SYSTEM_FMT)
    find_package(fmt REQUIRED)
else ()
    add_subdirectory("third_party/fmt")
endif ()

option(USE_SYSTEM_CATCH2 "If enabled, use system supplied Catch2 instead of the included version in third_party/" OFF)
if (USE_SYSTEM_CATCH2)
    find_package(Catch2 REQUIRED)
else ()
    add_subdirectory("third_party/Catch2")
endif ()

find_package(SDL2 REQUIRED)

# Do not build glbinding tools and examples
set(OPTION_BUILD_TOOLS OFF)
set(OPTION_BUILD_EXAMPLES OFF)
add_subdirectory(third_party/glbinding)

add_subdirectory("src")
add_subdirectory("test")
