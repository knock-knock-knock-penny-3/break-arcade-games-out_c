#include "main.h"
#include "game.h"

v2 player_p;

void simulate_game(Game *game, Input *input, f64 dt) {
    f32 speed = 100;

    if (is_down(BUTTON_LEFT)) player_p.x -= speed * dt;
    if (is_down(BUTTON_RIGHT)) player_p.x += speed * dt;
    if (is_down(BUTTON_UP)) player_p.y += speed * dt;
    if (is_down(BUTTON_DOWN)) player_p.y -= speed * dt;

    clear_screen(game, 0xFF551100);
    draw_rect(game, player_p, (v2){1, 1}, 0xFF00FF00);
}
