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

//    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
