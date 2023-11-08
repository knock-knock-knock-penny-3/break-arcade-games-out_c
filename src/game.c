#include "main.h"
#include "game.h"

v2 ball_p;
v2 ball_dp;
v2 ball_half_size;

v2 player_p;
v2 player_dp;
v2 player_half_size;

v2 arena_half_size;

b32 initialized = false;

void simulate_game(Game *game, Input *input, f64 dt) {
    if (!initialized) {
        initialized = true;
        ball_p.y = 40;
        ball_dp.y = -30;
        ball_half_size = (v2){.75, .75};

        player_p.y = -40;
        player_half_size = (v2){10, 2};

        arena_half_size = (v2){80, 50};
    }

    f32 player_new_x = pixels_to_world(game, input->mouse).x;
    player_dp.x = (player_new_x - player_p.x) / dt;
    player_p.x = player_new_x;

    ball_p = add_v2(ball_p, mul_v2(ball_dp, dt));

    if (ball_dp.y < 0 && aabb_vs_aabb(player_p, player_half_size, ball_p, ball_half_size)) {
        ball_dp.x += player_dp.x;
        ball_dp.y *= -1;
    } else if (ball_p.x + ball_half_size.x > arena_half_size.x) {
        ball_p.x = arena_half_size.x - ball_half_size.x;
        ball_dp.x *= -1;
    } else if (ball_p.x - ball_half_size.x < -arena_half_size.x) {
        ball_p.x = -arena_half_size.x + ball_half_size.x;
        ball_dp.x *= -1;
    }

    clear_screen(game, 0xFF220500);
    draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100);
    draw_rect(game, ball_p, ball_half_size, 0xFF00FFFF);
    draw_rect(game, player_p, player_half_size, 0xFF00FF00);
}
