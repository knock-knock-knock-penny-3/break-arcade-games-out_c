#include "main.h"
#include "software_rendering.h"

f32 scale = 0.01f;

void rgba_debug(u32 color, RGBA rgba) {
    SDL_Log("COLOR: %x", color);
    SDL_Log("R: %x (%d)", rgba.r, rgba.r);
    SDL_Log("G: %x (%d)", rgba.g, rgba.g);
    SDL_Log("B: %x (%d)", rgba.b, rgba.b);
    SDL_Log("A: %x (%d)", rgba.a, rgba.a);
    SDL_Log("-------------------------------------------");
}

RGBA color_converter(u32 hexValue) {
    RGBA rgba;

    rgba.r = ((hexValue >> 24) & 0xFF);
    rgba.g = ((hexValue >> 16) & 0xFF);
    rgba.b = ((hexValue >> 8) & 0xFF);
    rgba.a = (hexValue & 0xFF);

    return rgba;
}

u32 rgba_converter(RGBA rgba) {
    u32 color = 0xFF;

    color = (color << 8) | rgba.r;
    color = (color << 8) | rgba.g;
    color = (color << 8) | rgba.b;
    color = (color << 8) | rgba.a;

    return color;
}

u32 set_color(u32 base_color, u8 new_value, Color type_value) {
    RGBA rgba = color_converter(base_color);

    switch (type_value) {
        case RED: {
            rgba.r = clamp(0, new_value, 255);
        } break;
        case GREEN: {
            rgba.g = clamp(0, new_value, 255);
        } break;
        case BLUE: {
            rgba.b = clamp(0, new_value, 255);
        } break;
        case ALPHA: {
            rgba.a = clamp(0, new_value, 255);
        } break;
        default:
    }

    return rgba_converter(rgba);
}

void set_screen(Game *game, int width, int height) {
    game->screen_size = (v2i){SCREEN_WIDTH, SCREEN_HEIGHT};
    game->screen_center = mul_v2(v2i_to_v2(game->screen_size), .5f);

    SDL_RenderSetLogicalSize(game->renderer, game->screen_size.x, game->screen_size.y);
}

void clear_screen(Game *game, u32 color) {
    RGBA rgba = color_converter(color);

    // Set the drawing color
    SDL_SetRenderDrawColor(game->renderer, rgba.r, rgba.g, rgba.b, rgba.a);

    // Clear the screen (to the selected color)
    SDL_RenderClear(game->renderer);
}

inline void draw_number(Game *game, int number, v2 p, f32 size, u32 color) {
    b32 positive = number >= 0;
    u32 num = abs(number);
    int digit = num % 10;
    b32 first_digit = true;

    f32 square_size = size / 5.f;
    f32 half_square_size = size / 10.f;

    while (num || first_digit) {
        first_digit = false;

        switch(digit) {
            case 0: {
                draw_rect(game, (v2){p.x-square_size, p.y},     (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y},     (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x, p.y+square_size*2.f}, (v2){half_square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y-square_size*2.f}, (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 1: {
                draw_rect(game, (v2){p.x+square_size, p.y},     (v2){half_square_size, 2.5f*square_size}, color);
                p.x -= square_size*2.f;
            } break;

            case 2: {
                draw_rect(game, (v2){p.x, p.y+square_size*2.f}, (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y},                 (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y-square_size*2.f}, (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y+square_size}, (v2){half_square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-square_size, p.y-square_size}, (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 3: {
                draw_rect(game, (v2){p.x-half_square_size, p.y+square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-half_square_size, p.y},                 (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-half_square_size, p.y-square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y}, (v2){half_square_size, 2.5f*square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 4: {
                draw_rect(game, (v2){p.x+square_size, p.y},             (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x-square_size, p.y+square_size}, (v2){half_square_size, 1.5f*square_size}, color);
                draw_rect(game, (v2){p.x, p.y},                         (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 5: {
                draw_rect(game, (v2){p.x, p.y+square_size*2.f}, (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y},                 (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y-square_size*2.f}, (v2){1.5f*square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-square_size, p.y+square_size}, (v2){half_square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y-square_size}, (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 6: {
                draw_rect(game, (v2){p.x+half_square_size, p.y+square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+half_square_size, p.y},                 (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+half_square_size, p.y-square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-square_size, p.y}, (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y-square_size},          (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 7: {
                draw_rect(game, (v2){p.x+square_size, p.y},             (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x-half_square_size, p.y+square_size*2.f}, (v2){square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 8: {
                draw_rect(game, (v2){p.x-square_size, p.y},     (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y},     (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x, p.y+square_size*2.f}, (v2){half_square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y-square_size*2.f}, (v2){half_square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x, p.y},                         (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            case 9: {
                draw_rect(game, (v2){p.x-half_square_size, p.y+square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-half_square_size, p.y},                 (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x-half_square_size, p.y-square_size*2.f}, (v2){square_size, half_square_size}, color);
                draw_rect(game, (v2){p.x+square_size, p.y},                      (v2){half_square_size, 2.5f*square_size}, color);
                draw_rect(game, (v2){p.x-square_size, p.y+square_size},          (v2){half_square_size, half_square_size}, color);
                p.x -= square_size*4.f;
            } break;

            invalid_default_case;
        }

        num /= 10;
        digit = num % 10;
    }
    if (!positive) {
        draw_rect(game, (v2){p.x, p.y},                 (v2){1.5f*square_size, half_square_size}, color);
    }
}

v2 pixels_dp_to_world(Game *game, v2i pixels_coord) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    v2 result;
    result.x = (f32)pixels_coord.x;
    result.y = (f32)pixels_coord.y;

    result.x /= aspect_multiplier;
    result.x /= scale;

    result.y /= aspect_multiplier;
    result.y /= scale;

    return result;
}

v2 pixels_to_world(Game *game, v2i pixels_coord) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    v2 result;
    result.x = (f32)pixels_coord.x - game->screen_center.x;
    result.y = (f32)pixels_coord.y - game->screen_center.y;

    result.x /= aspect_multiplier;
    result.x /= scale;

    result.y /= aspect_multiplier;
    result.y /= scale;

    return result;
}

f32 calculate_aspect_multiplier(Game *game) {
    f32 aspect_multiplier = game->screen_size.y;
    f32 ratio = 16 / 9;

    if (game->screen_size.x / game->screen_size.y < ratio) {
        aspect_multiplier = game->screen_size.x / ratio;
    }

    return aspect_multiplier;
}

void draw_rect_in_pixels(Game *game, int x0, int y0, int x1, int y1, u32 color) {
    RGBA rgba = color_converter(color);

    // Set the drawing color
    SDL_SetRenderDrawColor(game->renderer, rgba.r, rgba.g, rgba.b, rgba.a);

    // Draw a rectangle of the selected color
    x0 = clamp(0, x0, game->screen_size.x);
    x1 = clamp(0, x1, game->screen_size.x);
    y0 = clamp(0, y0, game->screen_size.y);
    y1 = clamp(0, y1, game->screen_size.y);

    SDL_Rect rect = {x0, game->screen_size.y - y1, x1-x0, y1-y0};
    SDL_RenderFillRect(game->renderer, &rect);
}

void draw_rect(Game *game, v2 p, v2 half_size, u32 color) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size.x *= aspect_multiplier * scale;
    half_size.y *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p = add_v2(p, game->screen_center);

    int x0 = (int)(p.x - half_size.x);
    int y0 = (int)(p.y - half_size.y);
    int x1 = (int)(p.x + half_size.x);
    int y1 = (int)(p.y + half_size.y);

    draw_rect_in_pixels(game, x0, y0, x1, y1, color);
}

void draw_rotated_rect(Game *game, v2 p, v2 half_size, f32 angle, u32 color) {  // In degrees
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size.x *= aspect_multiplier * scale;
    half_size.y *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p = add_v2(p, game->screen_center);

    f32 x0 = p.x - half_size.x;
    f32 y0 = p.y - half_size.y;
    f32 x1 = p.x + half_size.x;
    f32 y1 = p.y + half_size.y;

    // draw_rect_in_pixels(game, x0, y0, x1, y1, color);
    RGBA rgba = color_converter(color);

    // Set the drawing color
    SDL_SetRenderDrawColor(game->renderer, rgba.r, rgba.g, rgba.b, rgba.a);

    // Draw a rectangle of the selected color
    int ix0 = clamp(0, (int)x0, game->screen_size.x);
    int ix1 = clamp(0, (int)x1, game->screen_size.x);
    int iy0 = clamp(0, (int)y0, game->screen_size.y);
    int iy1 = clamp(0, (int)y1, game->screen_size.y);

    SDL_Surface *surface = SDL_CreateRGBSurface(0, ix1-ix0, iy1-iy0, 32, RMASK, GMASK, BMASK, AMASK);
    int fillrect = SDL_FillRect(surface, NULL, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect rect = {ix0, game->screen_size.y - iy1, ix1-ix0, iy1-iy0};

    SDL_RenderCopyEx(
        game->renderer,
        texture,
        NULL,
        &rect,
        angle,
        &(SDL_Point){half_size.x, half_size.y},
        SDL_FLIP_NONE
    );
}

void clear_arena_screen(Game *game, v2 p, f32 left_most, f32 right_most, f32 half_size_y, u32 color) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size_y *= aspect_multiplier * scale;
    left_most   *= aspect_multiplier * scale;
    right_most  *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p = add_v2(p, game->screen_center);

    int x0 = (int)(p.x + left_most);
    int y0 = (int)(p.y - half_size_y);
    int x1 = (int)(p.x + right_most);
    int y1 = (int)(p.y + half_size_y);

    draw_rect_in_pixels(game, x0, 0, x1, y1, color);    // arena
}

void draw_arena_rects(Game *game, v2 p, f32 left_most, f32 right_most, f32 half_size_y, u32 clear_color) {
    f32 aspect_multiplier = calculate_aspect_multiplier(game);

    half_size_y *= aspect_multiplier * scale;
    left_most   *= aspect_multiplier * scale;
    right_most  *= aspect_multiplier * scale;

    p.x *= aspect_multiplier * scale;
    p.y *= aspect_multiplier * scale;

    p = add_v2(p, game->screen_center);

    int x0 = (int)(p.x + left_most);
    int y0 = (int)(p.y - half_size_y);
    int x1 = (int)(p.x + right_most);
    int y1 = (int)(p.y + half_size_y);

    draw_rect_in_pixels(game, 0, 0, x0, game->screen_size.y, clear_color);                      // left border
    draw_rect_in_pixels(game, x1, 0, game->screen_size.x, game->screen_size.y, clear_color);    // right border
    draw_rect_in_pixels(game, x0, y1, x1, game->screen_size.y, clear_color);                    // top border
}
