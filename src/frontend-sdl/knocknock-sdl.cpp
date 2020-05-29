#include <memory>

#include <SDL2/SDL.h>
#include <glbinding/glbinding.h>
#include <glog/logging.h>
#include <imgui.h>

#include "imgui_gl_renderer.h"
#include "imgui_sdl_input.h"

namespace {

class ScopedSDL {
public:
    explicit ScopedSDL(uint32_t subsystems) {
        initialized_ = (::SDL_Init(subsystems) == 0);
    }

    ~ScopedSDL() { ::SDL_Quit(); }

    [[nodiscard]] bool initialized() const { return initialized_; }

private:
    bool initialized_;
};

using SDLWindowPtr =
    std::unique_ptr<SDL_Window, decltype(&::SDL_DestroyWindow)>;

// SDL_GLContext is a void*, so the pointer points to the context of type void.
using SDLGLContextPtr =
    std::unique_ptr<void, decltype(&::SDL_GL_DeleteContext)>;

using ImGuiContextPtr =
    std::unique_ptr<ImGuiContext, decltype(&::ImGui::DestroyContext)>;

const ImVec4 BACKGROUND_COLOR(0.45f, 0.55f, 0.60f, 1.00f);

}  // namespace

int main(int argc, const char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    // Write log to stderr
    FLAGS_logtostderr = true;

    // Enable log color.
    FLAGS_colorlogtostderr = true;

    // Log messages at level WARNING and above by default.
    FLAGS_minloglevel = google::GLOG_INFO;

    // Initialize ImGui.
    IMGUI_CHECKVERSION();
    ImGuiContextPtr imgui_context(ImGui::CreateContext(),
                                  ImGui::DestroyContext);
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.Fonts->AddFontDefault();

    // Initialize SDL
    ScopedSDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    if (!sdl.initialized()) {
        LOG(ERROR) << "Unable to initialize SDL: " << SDL_GetError();
        return 1;
    }

    SDLWindowPtr window(
        ::SDL_CreateWindow("knocknock-sdl", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, 800, 600,
                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                               SDL_WINDOW_ALLOW_HIGHDPI),
        ::SDL_DestroyWindow);
    if (!window) {
        LOG(ERROR) << "Unable to create SDL window: " << SDL_GetError();
        return 1;
    }

    // Request OpenGL 3.2 so we can pop it into RenderDoc.
    ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_CORE);

    // TODO: are these needed?
    ::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    ::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    ::SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#ifndef NDEBUG
    ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    ::SDL_GL_SetSwapInterval(1);

    SDLGLContextPtr gl_context(::SDL_GL_CreateContext(window.get()),
                               ::SDL_GL_DeleteContext);
    if (!gl_context) {
        LOG(ERROR) << "Unable to create OpenGL context: " << SDL_GetError();
        return 1;
    }

    ImGuiGLRenderer renderer(
        [](const char *proc) {
            return (glbinding::ProcAddress)::SDL_GL_GetProcAddress(proc);
        },
        io);
    if (!renderer.initialized()) {
        LOG(ERROR) << "Unable to initialize ImGuiGLRenderer";
        return 1;
    }

    // Upload font textures to GPU.
    unsigned char *font_texture_data;
    int font_texture_width, font_texture_height;
    io.Fonts->GetTexDataAsRGBA32(&font_texture_data, &font_texture_width,
                                 &font_texture_height);
    auto font_texture = renderer.create_texture(
        font_texture_data, font_texture_width, font_texture_height);
    io.Fonts->SetTexID(reinterpret_cast<void *>(&font_texture));

    ImGuiSDLInput sdl_input(io);

    uint64_t last_frame_tick = ::SDL_GetPerformanceCounter();
    while (!sdl_input.should_quit()) {
        // Handle incoming events.
        SDL_Event event;
        while (::SDL_PollEvent(&event)) {
            sdl_input.handle_event(event);
        }

        sdl_input.refresh_cursor();

        int w, h;
        ::SDL_GetWindowSize(window.get(), &w, &h);
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

        int display_w, display_h;
        ::SDL_GL_GetDrawableSize(window.get(), &display_w, &display_h);
        io.DisplayFramebufferScale = ImVec2(display_w / w, display_h / h);

        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::EndFrame();
        ImGui::Render();

        renderer.clear(BACKGROUND_COLOR);
        renderer.render(ImGui::GetDrawData());
        ::SDL_GL_SwapWindow(window.get());

        uint64_t current_tick = ::SDL_GetPerformanceCounter();
        io.DeltaTime = static_cast<float>(current_tick - last_frame_tick) /
                       static_cast<float>(::SDL_GetPerformanceFrequency());
        last_frame_tick = ::SDL_GetPerformanceCounter();
    }

    return 0;
}