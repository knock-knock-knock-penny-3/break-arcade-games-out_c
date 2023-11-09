#include "main.h"
#include "game.h"

v2 ball_p;
v2 ball_dp;
v2 ball_half_size;

v2 player_p;
v2 player_dp;
v2 player_half_size;

v2 block_half_size;

Block blocks[256];
int next_block;

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

        arena_half_size = (v2){85, 45};

        block_half_size = (v2){4, 2};

#define num_y 10
#define num_x 10
        f32 x_offset = block_half_size.x * num_x;
        f32 y_offset = block_half_size.y * num_y - 25;
        for (int y = 0; y < num_y; y++) {
            for (int x = 0; x < num_x; x++) {
                Block *block = blocks + next_block++;
                if (next_block >= array_count(blocks)) {
                    next_block = 0;
                }

                block->life = 1;
                block->p.x = x * block_half_size.x * 2 - x_offset;
                block->p.y = y * block_half_size.y * 2 - y_offset;
                block->color = make_color_from_grey((x + y) * 2);
            }
        }
    }

    f32 player_new_x = pixels_to_world(game, input->mouse).x;
    player_dp.x = (player_new_x - player_p.x) / dt;
    player_p.x = player_new_x;

    ball_p = add_v2(ball_p, mul_v2(ball_dp, dt));

    if (ball_dp.y < 0 && aabb_vs_aabb(player_p, player_half_size, ball_p, ball_half_size)) {
        // player collision with ball
        ball_p.y = player_p.y + player_half_size.y;
        ball_dp.x += player_dp.x * .5f;
        ball_dp.y *= -1;
    } else if (ball_p.x + ball_half_size.x > arena_half_size.x) {
        ball_p.x = arena_half_size.x - ball_half_size.x;
        ball_dp.x *= -1;
    } else if (ball_p.x - ball_half_size.x < -arena_half_size.x) {
        ball_p.x = -arena_half_size.x + ball_half_size.x;
        ball_dp.x *= -1;
    }

    if (ball_p.y + ball_half_size.y > arena_half_size.y) {
        ball_p.y = arena_half_size.y - ball_half_size.y;
        ball_dp.y *= -1;
    }

    clear_screen_and_draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100, 0xFF220500);

    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        if (!block->life) continue;
        draw_rect(game, block->p, block_half_size, block->color);
    }

    draw_rect(game, ball_p, ball_half_size, 0xFF00FFFF);
    draw_rect(game, player_p, player_half_size, 0xFF00FF00);
}
