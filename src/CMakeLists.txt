cmake_minimum_required(VERSION 3.0.0)
project(knocknock VERSION 0.0.1)

set(SOURCE_FILES
    "knocknock.cpp"
    "cpu/cpu.cpp"
    "cpu/register.cpp"
    "memory/memory.cpp"
)
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(knocknock "${SOURCE_FILES}")
