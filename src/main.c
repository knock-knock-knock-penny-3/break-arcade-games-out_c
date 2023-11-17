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
    set_screen(&game, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(game.renderer,SDL_BLENDMODE_BLEND);

    v2i mouse_pointer = {0};
    Input input = {0};

//    SDL_ShowCursor(SDL_DISABLE);
//    SDL_WarpMouseInWindow(window, game.screen_center.x, game.screen_center.y);
    SDL_GetMouseState(&input.mouse_p.x,&input.mouse_p.y);

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
                case SDL_QUIT: {
                    if (window) {
                        // Close and destroy the window
                        SDL_DestroyWindow(window);
                        window = NULL;
                    }
                    running = false;
                } break;

                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        set_screen(&game, event.window.data1, event.window.data2);
                    }
                } break;

#if DEVELOPMENT
                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_r: {
                            start_game(0);
                        } break;

                        case SDLK_ESCAPE: {
                            running = false;
                        } break;
                    }
                } break;
#endif
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

#if DEVELOPMENT
        set_slowmotion(state[SDL_SCANCODE_SPACE]);
#endif

        SDL_GetMouseState(&mouse_pointer.x,&mouse_pointer.y);
        mouse_pointer.y = game.screen_size.y - mouse_pointer.y;

        input.mouse_dp = sub_v2i(mouse_pointer, input.mouse_p);

        SDL_Log("%d - %d", SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS, SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_CAPTURE);
//        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) {
//            SDL_WarpMouseInWindow(window, game.screen_center.x, game.screen_center.y);
//        }
        input.mouse_p = mouse_pointer;

        // Simulation
        simulate_game(&game, &input, last_dt);
        draw_number(&game, mouse_pointer.x, (v2){-game.arena_half_size.x + 50.f, game.arena_half_size.y + 2.5f}, 2.5f, 0xFFFFFFFF);

        // Render
        SDL_RenderPresent(game.renderer);

        // Get the frame time
        u32 current_counter = SDL_GetPerformanceCounter();
        last_dt = min(.1f, (f64)((current_counter - last_counter) / (f64)SDL_GetPerformanceFrequency()));
        last_counter = current_counter;
    }

    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
