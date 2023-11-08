#include "main.h"
#include "game.h"

v2 player_p;
int step = 20;

void simulate_game(Game *game, Input *input) {
    clear_screen(game, 0xFF551100);

    if (pressed(BUTTON_LEFT)) player_p.x -= step;
    if (pressed(BUTTON_RIGHT)) player_p.x += step;
    if (pressed(BUTTON_UP)) player_p.y += step;
    if (pressed(BUTTON_DOWN)) player_p.y -= step;

    draw_rect(game, player_p, (v2){10, 10}, 0xFF00FF00);
}
