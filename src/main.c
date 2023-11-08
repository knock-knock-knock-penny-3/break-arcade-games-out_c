#include <stdio.h>
#include "main.h"

int main() {
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
                                WINDOW_TITLE,               // window title
                                SDL_WINDOWPOS_UNDEFINED,    // initial x position
                                SDL_WINDOWPOS_UNDEFINED,    // initial y position
                                SCREEN_WIDTH,               // width, in pixels
                                SCREEN_HEIGHT,              // height, in pixels
                                SDL_WINDOW_RESIZABLE        // flags
                            );

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

        // Simulation


        // Render

    }

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
