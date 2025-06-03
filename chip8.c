#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

// Emulator defs
typedef struct {
    uint32_t width;  // Window width
    uint32_t height; // Window height
} emulator_t;

// SDL window
typedef struct { 
    SDL_Window *window;
} sdl_t;

bool sdlInit(emulator_t *emulator, sdl_t *sdl) {
    // Init video, audio and timer submodules
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow(
                                    "Chip 8",                   // Title
                                    SDL_WINDOWPOS_CENTERED,     // X pos
                                    SDL_WINDOWPOS_CENTERED,     // Y pos
                                    emulator->width,            // Width
                                    emulator->height,           // Height
                                    0);                         // Flags

    if (sdl->window == NULL) {
        SDL_Log("Could not create a window: %s\n", SDL_GetError());
        return false;
    }

    SDL_Delay(3000); //test

    return true; // SDL init successful
}

void cleanup(sdl_t *sdl) {
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // SDL initialization
    emulator_t emulator = {
        800, 
        400
    };

    sdl_t sdl = {0};
    if (!sdlInit(&emulator, &sdl)) exit(EXIT_FAILURE);

    // SDL cleanup
    cleanup(&sdl);

    puts("Hello world!");

    exit(EXIT_SUCCESS);
}
