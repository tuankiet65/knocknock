cmake_minimum_required(VERSION 3.0.0)
project(knocknock
        VERSION 0.1
        LANGUAGES CXX)

option(USE_SYSTEM_GLOG "If enabled, use system supplied glog instead of the included version in third_party/" OFF)
set(WITH_GFLAGS OFF CACHE BOOL "Disable gflags")
set(BUILD_TESTING OFF CACHE BOOL "Disable building unittests for glog")
if (USE_SYSTEM_GLOG)
    find_package(glog REQUIRED)
else ()
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

add_subdirectory("src")
