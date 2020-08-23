# Custom CMake code for imgui because it doesn't come with a CMakeLists.txt
add_library(imgui
        STATIC
        ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui.cpp
        ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
        ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp)
target_include_directories(imgui
        PUBLIC
        ${PROJECT_SOURCE_DIR}/third_party/imgui
        ${PROJECT_SOURCE_DIR}/third_party/imgui_club)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUI_USE_WCHAR32)

add_executable(knocknock-sdl
        imgui_gl_renderer.cpp
        knocknock-sdl.cpp
        imgui_sdl_input.cpp)
target_link_libraries(knocknock-sdl PRIVATE imgui knocknock glbinding::glbinding SDL2::SDL2 SDL2::SDL2main glog::glog fmt::fmt)
target_include_directories(knocknock-sdl PRIVATE .)
target_compile_features(knocknock-sdl PRIVATE cxx_std_17)

# Target the Windows subsystem to disable the console window.
if (MSVC)
    target_link_options(knocknock-sdl PRIVATE /SUBSYSTEM:WINDOWS)
endif ()
