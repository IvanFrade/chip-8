#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

// Emulator config
typedef struct {
    uint32_t width;  // Window width
    uint32_t height; // Window height
} emulator_t;

// SDL window
typedef struct { 
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

// Setup emulator config by passed in arguments
bool emuConfigInit(emulator_t *emulator, const int argc, char **argv) {
    // Default config
    emulator->width = 640;
    emulator->height = 320;
    (void)argc;
    (void)argv;

    // TODO: set config according to args

    return true;
}

bool sdlInit(const emulator_t emulator, sdl_t *sdl) {
    // Init video, audio and timer submodules
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("Chip 8",                   // Title
                                    SDL_WINDOWPOS_CENTERED,     // X pos
                                    SDL_WINDOWPOS_CENTERED,     // Y pos
                                    emulator.width,            // Width
                                    emulator.height,           // Height
                                    0);                         // Flags
    if (sdl->window == NULL) {
        SDL_Log("Could not create a window: %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, 
                                       -1, 
                                       0);
    if (sdl->renderer == NULL) {
        SDL_Log("Could not create a renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
    SDL_RenderClear(sdl->renderer);
    SDL_RenderPresent(sdl->renderer);

    SDL_Delay(3000); //test

    return true; // SDL init successful
}

void cleanup(sdl_t *sdl) {
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    sdl_t sdl = {0};
    emulator_t emulator = {0};

    // Init emu configuration
    if (!emuConfigInit(&emulator, argc, argv)) exit(EXIT_FAILURE);    

    if (!sdlInit(emulator, &sdl)) exit(EXIT_FAILURE);

    // SDL cleanup
    cleanup(&sdl);

    puts("Hello world!");

    exit(EXIT_SUCCESS);
}
