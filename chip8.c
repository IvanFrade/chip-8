#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

SDL_Window *window;

bool sdlInit() {
    // Init video, audio and timer submodules
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // Init window element, centered, 400x200
    window = SDL_CreateWindow("Chip 8", 
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, 
                                    400, 
                                    200, 
                                    0);

    if (window == NULL) {
        SDL_Log("Could not create a window: %s\n", SDL_GetError());
        return false;
    }

    SDL_Delay(10000); //test

    return true; // SDL init successful
}

void cleanup() {
    SDL_DestroyWindow(window);

    SDL_Quit();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // SDL initialization
    if (!sdlInit()) exit(EXIT_FAILURE);

    // SDL cleanup

    puts("Hello world!");

    exit(EXIT_SUCCESS);
}
