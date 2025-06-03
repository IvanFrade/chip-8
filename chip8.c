#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

typedef struct { 
    SDL_Window *window;
} sdl_struct;

bool sdlInit(sdl_struct *sdl) {
    // Init video, audio and timer submodules
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow(
                                    "Chip 8",                   // Title
                                    SDL_WINDOWPOS_CENTERED,     // X pos
                                    SDL_WINDOWPOS_CENTERED,     // Y pos
                                    800,                        // Width
                                    400,                        // Height
                                    0);                         // Flags

    if (sdl->window == NULL) {
        SDL_Log("Could not create a window: %s\n", SDL_GetError());
        return false;
    }

    SDL_Delay(3000); //test

    return true; // SDL init successful
}

void cleanup(sdl_struct *sdl) {
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // SDL initialization
    sdl_struct sdl = {0};
    if (!sdlInit(&sdl)) exit(EXIT_FAILURE);

    // SDL cleanup
    cleanup(&sdl);

    puts("Hello world!");

    exit(EXIT_SUCCESS);
}
