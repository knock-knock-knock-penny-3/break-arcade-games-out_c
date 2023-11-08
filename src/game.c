#include "main.h"
#include "game.h"

v2 ball_p;
v2 ball_dp;

b32 initialized = false;

void simulate_game(Game *game, Input *input, f64 dt) {
    if (!initialized) {
        initialized = true;
        ball_dp.x = 40;
        ball_dp.y = -30;
    }

    v2 mouse_world = pixels_to_world(game, input->mouse);

    ball_p = add_v2(ball_p, mul_v2(ball_dp, dt));

    clear_screen(game, 0xFF551100);
    draw_rect(game, ball_p, (v2){.75, .75}, 0xFF00FFFF);
    draw_rect(game, (v2){mouse_world.x, -40}, (v2){10, 2}, 0xFF00FF00);
}
