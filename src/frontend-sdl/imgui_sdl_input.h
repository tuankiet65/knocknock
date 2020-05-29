#pragma once

#include <vector>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <imgui.h>

class ImGuiSDLInput {
public:
    explicit ImGuiSDLInput(ImGuiIO &io);
    ~ImGuiSDLInput();

    void handle_event(SDL_Event event);
    void refresh_cursor();

    [[nodiscard]] bool should_quit() const { return should_quit_; }

    ImGuiSDLInput(const ImGuiSDLInput &) = delete;
    ImGuiSDLInput &operator=(const ImGuiSDLInput &) = delete;

private:
    void handle_event_keyboard(SDL_KeyboardEvent event);
    void handle_event_mouse_motion(SDL_MouseMotionEvent event);
    void handle_event_mouse_button(SDL_MouseButtonEvent event);
    void handle_event_mouse_wheel(SDL_MouseWheelEvent event);
    void handle_event_quit(SDL_QuitEvent event);
    void handle_event_text_input(SDL_TextInputEvent event);

    ImGuiIO &io_;

    std::vector<SDL_Cursor *> cursors_;

    bool should_quit_;
};
