#include "main.h"
#include "game.h"

void simulate_game(Game *game, b32 character) {
    clear_screen(game, 0xFF551100);

    if (character) {
        draw_rect_in_pixels(game, 20, 20, 50, 50, 0xFFFFFF00);
    }
}
