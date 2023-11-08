#include "main.h"
#include "software_rendering.h"

RGBA color_converter(u32 hexValue) {
    RGBA rgba;

    rgba.a = ((hexValue >> 24) & 0xFF);
    rgba.r = ((hexValue >> 16) & 0xFF);
    rgba.g = ((hexValue >> 8) & 0xFF);
    rgba.b = (hexValue & 0xFF);

//    printf("%d, %d, %d, %d\n", a, r, g, b);
    return rgba;
}

void clear_screen(Game *game, u32 color) {
    RGBA rgba = color_converter(color);

    // Set the drawing color
    SDL_SetRenderDrawColor(game->renderer, rgba.r, rgba.g, rgba.b, rgba.a);

    // Clear the screen (to the selected color)
    SDL_RenderClear(game->renderer);
}

void draw_rect_in_pixels(Game *game, int x0, int y0, int x1, int y1, u32 color) {
    RGBA rgba = color_converter(color);

    // Set the drawing color
    SDL_SetRenderDrawColor(game->renderer, rgba.r, rgba.g, rgba.b, rgba.a);

    // Draw a rectangle of the selected color
    x0 = clamp(0, x0, game->width);
    x1 = clamp(0, x1, game->width);
    y0 = clamp(0, y0, game->height);
    y1 = clamp(0, y1, game->height);

    SDL_Rect rect = {x0, game->height-y1, x1-x0, y1-y0};
    SDL_RenderFillRect(game->renderer, &rect);
}
