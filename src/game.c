#include "main.h"
#include "game.h"

int px = 20;
int py = 20;
int step = 20;

void simulate_game(Game *game, Input *input) {
    clear_screen(game, 0xFF551100);

    if (pressed(BUTTON_LEFT)) px -= step;
    if (pressed(BUTTON_RIGHT)) px += step;
    if (pressed(BUTTON_UP)) py += step;
    if (pressed(BUTTON_DOWN)) py -= step;

    draw_rect_in_pixels(game, px, py, px+30, py+30, 0xFFFFFF00);
}
