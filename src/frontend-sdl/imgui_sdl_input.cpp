#include "imgui_sdl_input.h"

#include <memory>

#include <SDL2/SDL_clipboard.h>

namespace {

std::unique_ptr<void, decltype(&SDL_free)> clipboard_text(nullptr, SDL_free);

const char *sdl_get_clipboard(void *) {
    clipboard_text.reset(::SDL_GetClipboardText());
    return static_cast<const char *>(clipboard_text.get());
}

void sdl_set_clipboard(void *, const char *text) {
    ::SDL_SetClipboardText(text);
}

constexpr float MOUSE_WHEEL_SPEED = 0.5;

}  // namespace

ImGuiSDLInput::ImGuiSDLInput(ImGuiIO &io)
    : io_(io), cursors_(ImGuiMouseCursor_COUNT), should_quit_(false) {
    io_.BackendPlatformName = "ImGuiSDLInput";
    io_.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    // Setup keyboard mapping.
    io_.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io_.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io_.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io_.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io_.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io_.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io_.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io_.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io_.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io_.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io_.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io_.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io_.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io_.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io_.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io_.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
    io_.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io_.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io_.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io_.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io_.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io_.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

    // Setup cursors.
    cursors_[ImGuiMouseCursor_Arrow] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    cursors_[ImGuiMouseCursor_TextInput] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    cursors_[ImGuiMouseCursor_ResizeAll] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    cursors_[ImGuiMouseCursor_ResizeNS] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    cursors_[ImGuiMouseCursor_ResizeEW] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    cursors_[ImGuiMouseCursor_ResizeNESW] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    cursors_[ImGuiMouseCursor_ResizeNWSE] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    cursors_[ImGuiMouseCursor_Hand] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    cursors_[ImGuiMouseCursor_NotAllowed] =
        ::SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

    // Setup clipboard functions.
    io_.GetClipboardTextFn = sdl_get_clipboard;
    io_.SetClipboardTextFn = sdl_set_clipboard;
    io_.ClipboardUserData = nullptr;

    io_.FontAllowUserScaling = true;
    io_.ConfigWindowsMoveFromTitleBarOnly = true;
}

ImGuiSDLInput::~ImGuiSDLInput() {
    for (auto &cursor : cursors_) {
        ::SDL_FreeCursor(cursor);
        cursor = nullptr;
    }
}

void ImGuiSDLInput::handle_event(SDL_Event event) {
    // TODO: handle SDL_DROPFILE
    // TODO: need to handle SDL_WindowEvent?

    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP: handle_event_keyboard(event.key); break;
        case SDL_MOUSEMOTION: handle_event_mouse_motion(event.motion); break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            handle_event_mouse_button(event.button);
            break;
        case SDL_MOUSEWHEEL: handle_event_mouse_wheel(event.wheel); break;
        case SDL_QUIT: handle_event_quit(event.quit); break;
        case SDL_TEXTINPUT: handle_event_text_input(event.text); break;
    }
}

void ImGuiSDLInput::handle_event_keyboard(SDL_KeyboardEvent event) {
    io_.KeysDown[event.keysym.scancode] = (event.state == SDL_PRESSED);
    io_.KeyCtrl = event.keysym.mod & KMOD_CTRL;
    io_.KeyShift = event.keysym.mod & KMOD_SHIFT;
    io_.KeyAlt = event.keysym.mod & KMOD_ALT;
    io_.KeySuper = event.keysym.mod & KMOD_GUI;
}

void ImGuiSDLInput::handle_event_mouse_motion(SDL_MouseMotionEvent event) {
    io_.MousePos = ImVec2(event.x, event.y);
    io_.MouseDown[0] = event.state & SDL_BUTTON_LMASK;
    io_.MouseDown[1] = event.state & SDL_BUTTON_RMASK;
    io_.MouseDown[2] = event.state & SDL_BUTTON_MMASK;
}

void ImGuiSDLInput::handle_event_mouse_button(SDL_MouseButtonEvent event) {
    io_.MousePos = ImVec2(event.x, event.y);

    switch (event.button) {
        case SDL_BUTTON_LEFT:
            io_.MouseDown[0] = event.state == SDL_PRESSED;
            break;
        case SDL_BUTTON_RIGHT:
            io_.MouseDown[1] = event.state == SDL_PRESSED;
            break;
        case SDL_BUTTON_MIDDLE:
            io_.MouseDown[2] = event.state == SDL_PRESSED;
            break;
    }
}

void ImGuiSDLInput::handle_event_mouse_wheel(SDL_MouseWheelEvent event) {
    int x = event.x, y = event.y;
    if (event.direction == SDL_MOUSEWHEEL_FLIPPED) {
        x = -x;
        y = -y;
    }

    if (y != 0) {
        io_.MouseWheel = (y > 0) ? MOUSE_WHEEL_SPEED : -MOUSE_WHEEL_SPEED;
    }

    if (x != 0) {
        io_.MouseWheelH = (x > 0) ? MOUSE_WHEEL_SPEED : -MOUSE_WHEEL_SPEED;
    }
}

void ImGuiSDLInput::handle_event_quit(SDL_QuitEvent) {
    should_quit_ = true;
}

void ImGuiSDLInput::handle_event_text_input(SDL_TextInputEvent event) {
    io_.AddInputCharactersUTF8(event.text);
}

void ImGuiSDLInput::refresh_cursor() {
    if (io_.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) {
        return;
    }

    // ImGui is drawing the cursor for us so we hide the SDL cursor.
    if (io_.MouseDrawCursor) {
        ::SDL_ShowCursor(SDL_DISABLE);
    }

    ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    if (cursor == ImGuiMouseCursor_None) {
        ::SDL_ShowCursor(SDL_DISABLE);
    } else {
        // cursors_[cursor] might be NULL, in which case we fall back to the
        // generic arrow cursor.
        if (cursors_[cursor]) {
            ::SDL_SetCursor(cursors_[cursor]);
        } else {
            ::SDL_SetCursor(cursors_[ImGuiMouseCursor_Arrow]);
        }
        ::SDL_ShowCursor(SDL_ENABLE);
    }
}
