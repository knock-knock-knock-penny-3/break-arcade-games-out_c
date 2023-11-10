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
        ball_p.x = 75;
        ball_p.y = 40;
        ball_dp.y = -30;
        ball_half_size = (v2){.75, .75};

        player_p.y = -40;
        player_half_size = (v2){10, 2};

        arena_half_size = (v2){85, 45};

        block_half_size = (v2){4, 2};

#define num_y 8
#define num_x 20
        f32 x_offset = block_half_size.x * (num_x - 1);
        f32 y_offset = block_half_size.y * (num_y - 1) - 25;
        for (int y = 0; y < num_y; y++) {
            for (int x = 0; x < num_x; x++) {
                Block *block = blocks + next_block++;
                if (next_block >= array_count(blocks)) {
                    next_block = 0;
                }

                block->life = 1;
                block->p.x = x * block_half_size.x * 2 - x_offset;
                block->p.y = y * block_half_size.y * 2 - y_offset;
                block->color = make_color_from_grey((x + y) * 5);
            }
        }
    }

    v2 player_desired_p;
    player_desired_p.x = pixels_to_world(game, input->mouse).x;
    player_desired_p.y = player_p.y;
    v2 ball_desired_p = add_v2(ball_p, mul_v2(ball_dp, dt));

    if (ball_dp.y < 0 && is_colliding(player_p, player_half_size, ball_desired_p, ball_half_size)) {
        // player collision with ball
        ball_desired_p.y = player_p.y + player_half_size.y;
        ball_dp.x += clamp(-100, player_dp.x, 100);
        ball_dp.y *= -1;
    } else if (ball_desired_p.x + ball_half_size.x > arena_half_size.x) {
        // Left border
        ball_desired_p.x = arena_half_size.x - ball_half_size.x;
        ball_dp.x *= -1;
    } else if (ball_desired_p.x - ball_half_size.x < -arena_half_size.x) {
        // Right border
        ball_desired_p.x = -arena_half_size.x + ball_half_size.x;
        ball_dp.x *= -1;
    }

    if (ball_desired_p.y + ball_half_size.y > arena_half_size.y) {
        // Top border
        ball_desired_p.y = arena_half_size.y - ball_half_size.y;
        ball_dp.y *= -1;
    } else if (ball_desired_p.y - ball_half_size.y < -arena_half_size.y) {
        //@DEBUG: Bottom border
        ball_desired_p.y = -arena_half_size.y + ball_half_size.y;
        ball_dp.y *= -1;
    }

    clear_screen_and_draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100, 0xFF220500);

    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        if (!block->life) continue;

        // This is the ball/block collision
        v2 t;
        f32 diff_y = ball_desired_p.y - ball_p.y;
        if (diff_y != 0) {
            f32 collision_point_y;
            if (diff_y > 0) {
                collision_point_y = block->p.y - block_half_size.y - ball_half_size.y;
            } else {
                collision_point_y = block->p.y + block_half_size.y + ball_half_size.y;
            }
            t.y = (collision_point_y - ball_p.y) / diff_y;
            if (t.y >= 0.f && t.y <= 1.f) {
                f32 target_x = lerp(ball_p.x, t.y, ball_desired_p.x);
                if (target_x + ball_half_size.x > block->p.x - block_half_size.x &&
                    target_x - ball_half_size.x < block->p.x + block_half_size.x) {
                    ball_desired_p.y = lerp(ball_p.y, t.y, ball_desired_p.y);
                    ball_dp.y *= -1;
                    block->life--;
                }
            }
        }
        f32 diff_x = ball_desired_p.x - ball_p.x;
        if (diff_x != 0) {
            f32 collision_point_x;
            if (diff_x > 0) {
                collision_point_x = block->p.x - block_half_size.x - ball_half_size.x;
            } else {
                collision_point_x = block->p.x + block_half_size.x + ball_half_size.x;
            }
            t.x = (collision_point_x - ball_p.x) / diff_x;
            if (t.x >= 0.f && t.x <= 1.f) {
                f32 target_y = lerp(ball_p.y, t.x, ball_desired_p.y);
                if (target_y + ball_half_size.y > block->p.y - block_half_size.y &&
                    target_y - ball_half_size.y < block->p.y + block_half_size.y) {
                    ball_desired_p.x = lerp(ball_p.x, t.x, ball_desired_p.x);
                    ball_dp.x *= -1;
                    block->life--;
                }
            }
        }

        draw_rect(game, block->p, block_half_size, block->color);
    }

    ball_p = ball_desired_p;
    player_dp.x = (player_desired_p.x - player_p.x) / dt;
    player_p = player_desired_p;

    draw_rect(game, ball_p, ball_half_size, 0xFF00FFFF);
    draw_rect(game, player_p, player_half_size, 0xFF00FF00);
}
