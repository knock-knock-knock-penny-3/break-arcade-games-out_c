#include "main.h"
#include "game.h"

v2 player_p;

void simulate_game(Game *game, Input *input, f64 dt) {
    f32 speed = 100;

    clear_screen(game, 0xFF551100);

    v2 mouse_world = pixels_to_world(game, input->mouse);

    draw_rect(game, (v2){mouse_world.x, -40}, (v2){10, 2}, 0xFF00FF00);
}
