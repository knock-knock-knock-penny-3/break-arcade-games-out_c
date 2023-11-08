#include <stdio.h>
#include "main.h"

int main() {
    SDL_Window *window;
    Game game;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
                                WINDOW_TITLE,               // window title
                                SDL_WINDOWPOS_UNDEFINED,    // initial x position
                                SDL_WINDOWPOS_UNDEFINED,    // initial y position
                                SCREEN_WIDTH,               // width, in pixels
                                SCREEN_HEIGHT,              // height, in pixels
                                SDL_WINDOW_RESIZABLE        // flags
                            );

    game.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(game.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    game.width = SCREEN_WIDTH;
    game.height = SCREEN_HEIGHT;

    b32 character = false;

    while (running) {
        // Input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT_CLOSE:
                case SDL_QUIT:
                    if (window) {
                        // Close and destroy the window
                        SDL_DestroyWindow(window);
                        window = NULL;
                    }
                    running = false;
                    break;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);

        character = state[SDL_SCANCODE_LEFT];

        // Simulation
        simulate_game(&game, character);

        // Render
        SDL_RenderPresent(game.renderer);
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
