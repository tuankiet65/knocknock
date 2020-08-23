add_subdirectory(knocknock)

if (WITH_CLI_FRONTEND)
    add_subdirectory(frontend-cli)
endif()

if (WITH_SDL_FRONTEND)
    add_subdirectory(frontend-sdl)
endif()

