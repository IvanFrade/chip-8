#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

bool sdlInit() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    return true; // SDL init successful
}

void cleanup() {
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
