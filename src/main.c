#include <stdio.h>
#include "main.h"

int main() {
    SDL_Window *window;
    Game game;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
                                WINDOW_TITLE,               // window title
                                SDL_WINDOWPOS_CENTERED,     // initial x position
                                SDL_WINDOWPOS_CENTERED,     // initial y position
                                SCREEN_WIDTH,               // width, in pixels
                                SCREEN_HEIGHT,              // height, in pixels
                                SDL_WINDOW_RESIZABLE        // flags
                            );
    if (!window) {
        printf("error creating window: %s\n", SDL_GetError());
    }

    game.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game.renderer) {
        printf("error creating renderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    SDL_RenderSetLogicalSize(game.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(game.renderer,SDL_BLENDMODE_BLEND);

    game.width = SCREEN_WIDTH;
    game.height = SCREEN_HEIGHT;

    Input input = {0};
    u32 last_counter = SDL_GetPerformanceCounter();
    f64 last_dt = 0.01666f;  // 60 FPS

    while (running) {
        // Input
        for (int i = 0; i < BUTTON_COUNT; i++) {
            input.buttons[i].changed = false;
        }

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
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        game.width = event.window.data1;
                        game.height = event.window.data2;
                        SDL_RenderSetLogicalSize(game.renderer, game.width, game.height);
                    }
                    break;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);

#define process_button(vk, b) \
input.buttons[b].changed = state[vk] != input.buttons[b].is_down;\
input.buttons[b].is_down = state[vk];

        process_button(SDL_SCANCODE_LEFT, BUTTON_LEFT);
        process_button(SDL_SCANCODE_RIGHT, BUTTON_RIGHT);
        process_button(SDL_SCANCODE_UP, BUTTON_UP);
        process_button(SDL_SCANCODE_DOWN, BUTTON_DOWN);

        SDL_GetMouseState(&input.mouse.x,&input.mouse.y);
        input.mouse.y = game.height - input.mouse.y;

        // Simulation
        simulate_game(&game, &input, last_dt);

        // Render
        SDL_RenderPresent(game.renderer);

        // Get the frame time
        u32 current_counter = SDL_GetPerformanceCounter();
        last_dt = (f64)((current_counter - last_counter) / (f64)SDL_GetPerformanceFrequency());
        last_counter = current_counter;
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
