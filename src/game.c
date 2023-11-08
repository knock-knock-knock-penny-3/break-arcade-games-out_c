#include "main.h"
#include "game.h"

int p = 20;
int s = 20;

void simulate_game(Game *game, Input *input) {
    clear_screen(game, 0xFF551100);

    if (input->buttons[BUTTON_LEFT].is_down && input->buttons[BUTTON_LEFT].changed) {
        p += s;
    }

    draw_rect_in_pixels(game, p, 20, p+30, 50, 0xFFFFFF00);
}
