#include "SDL2/SDL.h"
#include "main.h"
#include "software_rendering.h"

f32 scale = 0.01f;

RGBA color_converter(u32 hexValue) {
    RGBA rgba;

    rgba.a = ((hexValue >> 24) & 0xFF);
    rgba.r = ((hexValue >> 16) & 0xFF);
    rgba.g = ((hexValue >> 8) & 0xFF);
    rgba.b = (hexValue & 0xFF);

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

    SDL_Rect rect = {x0, game->height - y1, x1-x0, y1-y0};
    SDL_RenderFillRect(game->renderer, &rect);
}

v2 pixels_to_world(Game *game, v2i pixels_coord) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    v2 result;
    result.x = (f32)pixels_coord.x - (game->width * .5f);
    result.y = (f32)pixels_coord.y - (game->height * .5f);

    result.x /= aspect_multiplier;
    result.x /= scale;

    result.y /= aspect_multiplier;
    result.y /= scale;

    return result;
}

f32 calculate_aspect_multiplier(Game *game) {
    f32 aspect_multiplier = game->height;
    f32 ratio = 16 / 9;

    if (game->width / game->height < ratio) {
        aspect_multiplier = game->width / ratio;
    }

    return aspect_multiplier;
}

void draw_rect(Game *game, v2 p, v2 half_size, u32 color) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size.x *= aspect_multiplier * scale;
    half_size.y *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p.x += game->width * .5f;
    p.y += game->height * .5f;

    int x0 = (int)(p.x - half_size.x);
    int y0 = (int)(p.y - half_size.y);
    int x1 = (int)(p.x + half_size.x);
    int y1 = (int)(p.y + half_size.y);

    draw_rect_in_pixels(game, x0, y0, x1, y1, color);
}

void clear_screen_and_draw_rect(Game *game, v2 p, v2 half_size, u32 color, u32 clear_color) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size.x *= aspect_multiplier * scale;
    half_size.y *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p.x += game->width * .5f;
    p.y += game->height * .5f;

    int x0 = (int)(p.x - half_size.x);
    int y0 = (int)(p.y - half_size.y);
    int x1 = (int)(p.x + half_size.x);
    int y1 = (int)(p.y + half_size.y);

    draw_rect_in_pixels(game, x0, y0, x1, y1, color);

    draw_rect_in_pixels(game, 0, 0, x0, game->height, clear_color);           // left border
    draw_rect_in_pixels(game, x1, 0, game->width, game->height, clear_color); // right border
    draw_rect_in_pixels(game, x0, y1, x1, game->height, clear_color);         // top border
    draw_rect_in_pixels(game, x0, 0, x1, y0, clear_color);                    // bottom border
}
