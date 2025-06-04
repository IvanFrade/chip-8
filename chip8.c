#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

// Emulator config
typedef struct {
    uint32_t width;         // Window width
    uint32_t height;        // Window height
    uint32_t scale_factor;  // Scale the original 64x32 pixel screen
    uint32_t bg_color;      // Background Color, 
    uint32_t fg_color;      // Foreground color, 
} emulator_t;           

typedef enum {
    QUIT, 
    RUNNING, 
    PAUSED,
} emulator_state_t;

// SDL window
typedef struct { 
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef struct {
    emulator_state_t state;
} chip8_t;

// Setup emulator config by passed in arguments
bool emuConfigInit(emulator_t *emulator, const int argc, char **argv) {
    // Default config
    emulator->width = 64;
    emulator->height = 32;
    emulator->scale_factor = 20;
    (void)argc;
    (void)argv;

    // Render color 
    emulator->fg_color = 0xFFFFFFFF; // White
    emulator->bg_color = 0x000000FF; // Black

    // TODO: set config according to args

    return true;
}

bool sdlInit(const emulator_t emulator, sdl_t *sdl) {
    // Init video, audio and timer submodules
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("Chip 8",                                // Title
                                    SDL_WINDOWPOS_CENTERED,                 // X pos
                                    SDL_WINDOWPOS_CENTERED,                 // Y pos
                                    emulator.width * emulator.scale_factor, // Width
                                    emulator.height * emulator.scale_factor,// Height
                                    0);                                     // Flags
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

    return true; // SDL init successful
}

bool chip8Init(chip8_t *chip8) {
    chip8 = chip8;
    return true;
}

// Screen clear to bg color
void clearScreen(const emulator_t emulator, const sdl_t sdl) {
    const uint32_t r = (emulator.bg_color >> 24) & 0x000000FF;
    const uint32_t g = (emulator.bg_color >> 16) & 0x000000FF;
    const uint32_t b = (emulator.bg_color >> 8) & 0x000000FF;
    const uint32_t a = emulator.bg_color & 0x000000FF;
    
    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

void updateScreen(const sdl_t sdl) {
    SDL_RenderPresent(sdl.renderer);
}

void handleInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            // close window
            break;
        
        default:
            break;
        }
    }
}

void cleanup(sdl_t *sdl) {
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    // Init structs to allocate memory
    sdl_t sdl = {0};
    emulator_t emulator = {0};
    chip8_t chip8 = {0};

    // Init emu configuration
    if (!emuConfigInit(&emulator, argc, argv)) exit(EXIT_FAILURE);    
    if (!sdlInit(emulator, &sdl)) exit(EXIT_FAILURE);
    if (!chip8Init(&chip8)) exit(EXIT_FAILURE);

    // Initial renderer clear
    clearScreen(emulator, sdl);

    // Main loop
    while (true) {
        SDL_Delay(16); // Approx 60 fps
        updateScreen(sdl);
    }

    // SDL cleanup
    cleanup(&sdl);

    puts("Hello world!");

    exit(EXIT_SUCCESS);
}
