#include "main.h"
#include "game.h"

v2 ball_p;
v2 ball_dp;
v2 ball_half_size;
f32 ball_base_speed;
f32 ball_speed_multiplier;

v2 player_p;
v2 player_dp;
v2 player_half_size;

Block blocks[256];
int next_block;

v2 arena_half_size;

b32 initialized = false;
b32 first_ball_movement = true;
b32 slowmotion = false;

void simulate_game(Game *game, Input *input, f64 dt) {
    if (!initialized) {
        initialized = true;
        ball_base_speed = -50;
        ball_p.y = 40;
        ball_dp.y = ball_base_speed;
        ball_half_size = (v2){.75, .75};
        ball_speed_multiplier = 1.f;

        player_p.y = -40;
        player_half_size = (v2){10, 2};

        arena_half_size = (v2){85, 45};

#define num_y 8
#define num_x 18
        f32 gap = 2.1f;
        f32 x_offset;
        f32 y_offset;

        for (int y = 0; y < num_y; y++) {
            for (int x = 0; x < num_x; x++) {
                Block *block = blocks + next_block++;

                block->half_size = (v2){4, 2};
                x_offset = block->half_size.x * (num_x - 1) * gap * .5f;
                y_offset = block->half_size.y * (num_y - 1) * gap * .5f - 15;

                if (next_block >= array_count(blocks)) {
                    next_block = 0;
                }

                block->life = 1;
                block->p.x = x * block->half_size.x * gap - x_offset;
                block->p.y = y * block->half_size.y * gap - y_offset;
                block->color = make_color_from_grey(y * 255 / num_y);
                block->ball_speed_multiplier = 1 + (f32)y * 1.25f / num_y;
            }
        }
    }
//    SDL_Log("base\tmul\tspeed\n%f\t%f\t%f", ball_base_speed, ball_speed_multiplier, ball_dp.y);

    v2 player_desired_p;
    player_desired_p.x = pixels_to_world(game, input->mouse).x;
    player_desired_p.y = player_p.y;
    v2 ball_desired_p = add_v2(ball_p, mul_v2(ball_dp, dt));
    //@DEBUG
    if (slowmotion) {
        ball_desired_p = add_v2(ball_p, mul_v2(div_v2(ball_dp, 10), dt));
    }

    if (ball_dp.y < 0 && is_colliding(player_p, player_half_size, ball_desired_p, ball_half_size)) {
        // player collision with ball
        ball_desired_p.y = player_p.y + player_half_size.y;
        ball_dp.x = (ball_p.x - player_p.x) * 7.5f;
        ball_dp.x += clamp(-25, player_dp.x * .5f, 25);
        ball_dp.y *= -1;
        first_ball_movement = false;
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
        // Bottom border
        //@DEBUG: Invincibility
//        ball_desired_p.y = -arena_half_size.y + ball_half_size.y;
//        ball_dp.y *= -1;
    }

    clear_screen_and_draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100, 0xFF220500);

    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        if (!block->life) continue;

        if (!first_ball_movement) {
            // This is the ball/block collision
            v2 t;
            f32 diff_y = ball_desired_p.y - ball_p.y;
            if (diff_y != 0) {
                f32 collision_point_y;
                if (diff_y > 0) {
                    collision_point_y = block->p.y - block->half_size.y - ball_half_size.y;
                } else {
                    collision_point_y = block->p.y + block->half_size.y + ball_half_size.y;
                }
                t.y = (collision_point_y - ball_p.y) / diff_y;
                if (t.y >= 0.f && t.y <= 1.f) {
                    f32 target_x = lerp(ball_p.x, t.y, ball_desired_p.x);
                    if (target_x + ball_half_size.x > block->p.x - block->half_size.x &&
                        target_x - ball_half_size.x < block->p.x + block->half_size.x) {
                        ball_desired_p.y = lerp(ball_p.y, t.y, ball_desired_p.y);
                        if (block->ball_speed_multiplier > ball_speed_multiplier) {
                            ball_speed_multiplier = block->ball_speed_multiplier;
                        }
                        if (ball_dp.y > 0) {
                            ball_dp.y = ball_base_speed * ball_speed_multiplier;
                        } else {
                            ball_dp.y = -ball_base_speed * ball_speed_multiplier;
                        }
                        block->life--;
                    }
                }
            }
            f32 diff_x = ball_desired_p.x - ball_p.x;
            if (diff_x != 0) {
                f32 collision_point_x;
                if (diff_x > 0) {
                    collision_point_x = block->p.x - block->half_size.x - ball_half_size.x;
                } else {
                    collision_point_x = block->p.x + block->half_size.x + ball_half_size.x;
                }
                t.x = (collision_point_x - ball_p.x) / diff_x;
                if (t.x >= 0.f && t.x <= 1.f) {
                    f32 target_y = lerp(ball_p.y, t.x, ball_desired_p.y);
                    if (target_y + ball_half_size.y > block->p.y - block->half_size.y &&
                        target_y - ball_half_size.y < block->p.y + block->half_size.y) {
                        ball_desired_p.x = lerp(ball_p.x, t.x, ball_desired_p.x);
                        ball_dp.x *= -1;
                        if (block->ball_speed_multiplier > ball_speed_multiplier) {
                            ball_speed_multiplier = block->ball_speed_multiplier;
                        }
                        if (ball_dp.y > 0) {
                            ball_dp.y = -ball_base_speed * ball_speed_multiplier;
                        } else {
                            ball_dp.y = ball_base_speed * ball_speed_multiplier;
                        }
                        block->life--;
                    }
                }
            }
        }

        draw_rect(game, block->p, block->half_size, block->color);
    }

    ball_p = ball_desired_p;
    player_dp.x = (player_desired_p.x - player_p.x) / dt;
    player_p = player_desired_p;

    draw_rect(game, ball_p, ball_half_size, 0xFF00FFFF);
    draw_rect(game, player_p, player_half_size, 0xFF00FF00);
}

void set_slowmotion(b32 sl) {
    slowmotion = sl;
}
