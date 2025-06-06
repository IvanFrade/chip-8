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

// Emulator object
typedef struct {
    uint32_t ram[4096];         // 4KB ram
    bool display[64*32];        // Bool array of single pixels (on/off)
    bool keyboard[16];          // Hex keypad 0-F

    uint16_t PC;                // Program Counter

    uint16_t stack[12];         // 12 bit stack
    uint8_t V_reg[16];          // "V" data registers, V0 - VF
    uint16_t I_reg;             // Index register

    uint8_t delay_timer;        // Counts down at 60hz
    uint8_t sound_timer;        // Counts down at 60hz, plays sounds while >0
    
    emulator_state_t state;

    char *rom;                  // Current ROM
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

bool chip8Init(chip8_t *chip8, char romName[]) {
    const uint32_t entryPoint = 0x200; // Entry point for most ROMs is typically 0x200
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Starting defaults
    chip8->PC = entryPoint; // Start at ROM
    chip8->state = RUNNING; // Default state

    // Load font to 0x000 - 0x1FF
    memcpy(&chip8->ram[0], font, sizeof(font));

    // Load ROM
    FILE *rom = fopen(romName, "rb");
    if (!rom) { 
        SDL_Log("File %s not found", romName);
        return false;
    }

    // Calculate ROM size
    fseek(rom, 0, SEEK_END);
    const long rom_size = ftell(rom);

    memcpy(&chip8->ram[0x200], rom, rom_size);

    fclose(rom);
    
    return true;
}

// Screen clear to bg color
void clearScreen(const emulator_t emulator, const sdl_t sdl) {
    // Shift by 24, 16 8 and 0 bits respectively to get actual value from hex, then mask to keep first 8 bits only
    const uint32_t r = (emulator.bg_color >> 24) & 0x000000FF;
    const uint32_t g = (emulator.bg_color >> 16) & 0x000000FF;
    const uint32_t b = (emulator.bg_color >> 8) & 0x000000FF;
    const uint32_t a = emulator.bg_color & 0x000000FF; // Alpha channel
    
    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

void updateScreen(const sdl_t sdl) {
    SDL_RenderPresent(sdl.renderer);
}

void handleInput(chip8_t *chip8) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            // Red X on top right of window is pressed
            case SDL_QUIT:
                chip8->state = QUIT;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        chip8->state = QUIT; // Close window and exit program on escape
                        break;
                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                break;

            default:
                // Error?
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
    // Init structs to allocate memory on program startup
    sdl_t sdl = {0};
    emulator_t emulator = {0};
    chip8_t chip8 = {0};

    // Init emu config
    if (!emuConfigInit(&emulator, argc, argv)) exit(EXIT_FAILURE);    
    if (!sdlInit(emulator, &sdl)) exit(EXIT_FAILURE);
    if (!chip8Init(&chip8, "IBM_Logo.chip8")) exit(EXIT_FAILURE);

    // Initial renderer clear to make sure :)
    clearScreen(emulator, sdl);

    // Main loop
    while (chip8.state != QUIT) {
        handleInput(&chip8);
        SDL_Delay(16); // Approx 60 fps
        updateScreen(sdl);
    }

    // SDL cleanup
    cleanup(&sdl);
    exit(EXIT_SUCCESS);
}
