#include "main.h"
#include "game.h"

Ball ball;
Ball powerup_balls[2];

v2 player_p;
v2 player_dp;
v2 player_half_size;

Block blocks[256];
int num_blocks;
int blocks_destroyed;

v2 arena_half_size;
b32 first_ball_movement = true;

b32 initialized = false;

Powerup powerups[16];
int next_powerup;
v2 powerup_half_size;
f32 invincibility_time; // in seconds
//int number_of_triple_shots;

Level current_level;
#if DEVELOPMENT
b32 slowmotion = false;
f32 dt_multiplier = 1.f;
b32 advance_level = false;
#endif

internal void spawn_powerup(Powerup_Kind kind, v2 p) {
    Powerup *powerup = powerups + next_powerup++;
    if (next_powerup >= array_count(powerups)) next_powerup = 0;
    powerup->p = p;
    powerup->kind = kind;
}

internal void block_destroyed(Block *block) {
    test_for_win_condition();

    if (block->powerup) {
        spawn_powerup(block->powerup, block->p);
    }
}

internal b32 do_ball_block_collision(Ball *ball, Block *block) {
    v2 t;
    f32 diff_y = ball->desired_p.y - ball->p.y;
    if (diff_y != 0) {
        f32 collision_point_y;
        if (diff_y > 0) {
            collision_point_y = block->p.y - block->half_size.y - ball->half_size.y;
        } else {
            collision_point_y = block->p.y + block->half_size.y + ball->half_size.y;
        }
        t.y = (collision_point_y - ball->p.y) / diff_y;
        if (t.y >= 0.f && t.y <= 1.f) {
            f32 target_x = lerp(ball->p.x, t.y, ball->desired_p.x);
            if (target_x + ball->half_size.x > block->p.x - block->half_size.x &&
                target_x - ball->half_size.x < block->p.x + block->half_size.x) {
                ball->desired_p.y = lerp(ball->p.y, t.y, ball->desired_p.y);
                if (block->ball_speed_multiplier > ball->speed_multiplier) {
                    ball->speed_multiplier = block->ball_speed_multiplier;
                }
                if (ball->dp.y > 0) {
                    ball->dp.y = ball->base_speed * ball->speed_multiplier;
                } else {
                    ball->dp.y = -ball->base_speed * ball->speed_multiplier;
                }
                block->life--;
                block_destroyed(block);
            }
        }
    }
    f32 diff_x = ball->desired_p.x - ball->p.x;
    if (diff_x != 0) {
        f32 collision_point_x;
        if (diff_x > 0) {
            collision_point_x = block->p.x - block->half_size.x - ball->half_size.x;
        } else {
            collision_point_x = block->p.x + block->half_size.x + ball->half_size.x;
        }
        t.x = (collision_point_x - ball->p.x) / diff_x;
        if (t.x >= 0.f && t.x <= 1.f) {
            f32 target_y = lerp(ball->p.y, t.x, ball->desired_p.y);
            if (target_y + ball->half_size.y > block->p.y - block->half_size.y &&
                target_y - ball->half_size.y < block->p.y + block->half_size.y) {
                ball->desired_p.x = lerp(ball->p.x, t.x, ball->desired_p.x);
                ball->dp.x *= -1;
                if (block->ball_speed_multiplier > ball->speed_multiplier) {
                    ball->speed_multiplier = block->ball_speed_multiplier;
                }
                if (ball->dp.y > 0) {
                    ball->dp.y = -ball->base_speed * ball->speed_multiplier;
                } else {
                    ball->dp.y = ball->base_speed * ball->speed_multiplier;
                }
                block->life--;
                block_destroyed(block);
            }
        }
    }
}

internal void simulate_level(Game *game) {

}

void create_block_block(int num_x, int num_y, f32 spacing) {
    f32 block_x_half_size = 4.f;
    f32 x_offset = (f32)num_x * block_x_half_size * (2.f + (spacing * 2.f)) * .5f - block_x_half_size * (1.f + spacing);
    f32 y_offset = -2.f;

    for (int y = 0; y < num_y; y++) {
        for (int x = 0; x < num_x; x++) {
            Block *block = blocks + num_blocks++;
            if (num_blocks >= array_count(blocks)) {
                num_blocks = 0;
            }

            block->life = 1;
            block->half_size = (v2){block_x_half_size, 2};

            block->p.x = x * block->half_size.x * (2.f + spacing * 2.f) - x_offset;
            block->p.y = y * block->half_size.y * (2.f + spacing * 2.f) - y_offset;
            block->color = make_color_from_grey(y * 255 / num_y);
            block->ball_speed_multiplier = 1 + (f32)y * 1.25f / num_y;

            if (y == 0) {
                block->powerup = POWERUP_INVINCIBILITY;
            }
        }
    }
}

inline void test_for_win_condition() {
    blocks_destroyed++;
    if (blocks_destroyed == num_blocks) {
        advance_level = true;
    }
}

inline void start_game(Level level) {
    advance_level = false;

    if (level >= L_COUNT) level = 0;
    else if (level < 0) level = L_COUNT - 1;

    current_level = level;

    first_ball_movement = true;
    ball.base_speed = -50;
    ball.p.x = 0;
    ball.p.y = 40;
    ball.dp.x = 0;
    ball.dp.y = ball.base_speed;
    ball.half_size = (v2){.75, .75};
    ball.speed_multiplier = 1.f;

    player_p.y = -40;
    player_half_size = (v2){10, 2};

    arena_half_size = (v2){85, 45};

    num_blocks = 0;
    blocks_destroyed = 0;
    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        block->life = 0;
    }

    switch (level) {
        case L01_NORMAL: {
            create_block_block(19, 9, .1f);
        } break;

        case L02_WALL: {
            int num_x = 20;
            int num_y = 9;
            f32 block_x_half_size = 4.f;
            f32 x_offset = (f32)num_x * block_x_half_size * 2.f *.5f - block_x_half_size*.5f;
            f32 y_offset = -4.f;
            for (int y = 0; y < num_y; y++) {
                for (int x = 0; x < num_x; x++) {
                    Block *block = blocks + num_blocks++;
                    if (num_blocks >= array_count(blocks)) {
                        num_blocks = 0;
                    }

                    block->life = 1;
                    block->half_size = (v2){block_x_half_size, 2};

                    if (y % 2) block->p.x = x*block->half_size.x*2.0f - x_offset;
                    else block->p.x = x*block->half_size.x*2.0f - x_offset + block->half_size.x;
                    block->p.y = y*block->half_size.y*2.0f - y_offset;
                    block->color = make_color_from_grey(y*255/num_y);
                    block->ball_speed_multiplier = 1+ (f32)y*1.25f/(f32)num_y;
                }

            }
        } break;

        case L03_STADIUM: {
            int num_x = 21;
            int num_y = 6;
            f32 block_x_half_size = 4.f;
            f32 x_offset = (f32)num_x * block_x_half_size- block_x_half_size;
            f32 y_offset = 0.f;
            for (int y = 0; y < num_y; y++) {
                for (int x = 0; x < num_x; x++) {
                    Block *block = blocks+num_blocks++;
                    if (num_blocks >= array_count(blocks)) {
                        num_blocks = 0;
                    }

                    block->life = 1;
                    block->half_size = (v2){block_x_half_size, 2};

                    block->p.x = x*block->half_size.x*2.f - x_offset;
                    block->p.y = y*block->half_size.y*4.f - y_offset;
                    block->color = make_color_from_grey(y*255/num_y);
                    block->ball_speed_multiplier = 1+ (f32)y*1.25f/(f32)num_y;
                }

            }
        } break;

        case L04_PONG: {

        } break;

        invalid_default_case;
    }
}

void simulate_game(Game *game, Input *input, f64 dt) {
    dt *= dt_multiplier;

    if (!initialized) {
        initialized = true;
        current_level = 0;
        start_game(current_level);

        powerup_half_size = (v2){2, 2};
    }

    v2 player_desired_p;
    player_desired_p.x = pixels_to_world(game, input->mouse).x;
    player_desired_p.y = player_p.y;
    ball.desired_p = add_v2(ball.p, mul_v2(ball.dp, dt));
#if DEVELOPMENT
    if (slowmotion) {
        ball.desired_p = add_v2(ball.p, mul_v2(div_v2(ball.dp, 10), dt));
    }
#endif

    if (ball.dp.y < 0 && is_colliding(player_p, player_half_size, ball.desired_p, ball.half_size)) {
        // player collision with ball
        ball.desired_p.y = player_p.y + player_half_size.y;
        ball.dp.x = (ball.p.x - player_p.x) * 7.5f;
        ball.dp.x += clamp(-25, player_dp.x * .5f, 25);
        ball.dp.y *= -1;
        first_ball_movement = false;
    } else if (ball.desired_p.x + ball.half_size.x > arena_half_size.x) {
        // Left border
        ball.desired_p.x = arena_half_size.x - ball.half_size.x;
        ball.dp.x *= -1;
    } else if (ball.desired_p.x - ball.half_size.x < -arena_half_size.x) {
        // Right border
        ball.desired_p.x = -arena_half_size.x + ball.half_size.x;
        ball.dp.x *= -1;
    }

    if (ball.desired_p.y + ball.half_size.y > arena_half_size.y) {
        // Top border
        ball.desired_p.y = arena_half_size.y - ball.half_size.y;
        ball.dp.y *= -1;
    }

    clear_screen_and_draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100, 0xFF220500);

    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        if (!block->life) continue;

        if (!first_ball_movement) {
            do_ball_block_collision(&ball, block);
        }

        draw_rect(game, block->p, block->half_size, block->color);
    }

    for (Powerup *powerup = powerups; powerup != powerups + array_count(powerups); powerup++) {
        if (powerup->kind == POWERUP_INACTIVE) continue;

        powerup->p.y -= 15 * dt;

        if (is_colliding(player_p, player_half_size, powerup->p, powerup_half_size)) {
            switch (powerup->kind) {
                case POWERUP_INVINCIBILITY: {
                    invincibility_time = 5.f;
                } break;

                invalid_default_case;
            }
            powerup->kind = POWERUP_INACTIVE;
        }

        draw_rect(game, powerup->p, powerup_half_size, 0xFFFFFF00);
    }

    ball.p = ball.desired_p;
    player_dp.x = (player_desired_p.x - player_p.x) / dt;
    player_p = player_desired_p;

    simulate_level(game);

    draw_rect(game, ball.p, ball.half_size, 0xFF00FFFF);

    if (invincibility_time > 0) {
        invincibility_time -= dt;
        draw_rect(game, player_p, player_half_size, 0xFFFFFFFF);
    }
    else draw_rect(game, player_p, player_half_size, 0xFF00FF00);

    if (ball.p.y - ball.half_size.y < -50) {
        // Bottom border
#if DEVELOPMENT
        // Invincibility
        SDL_Log("%f", invincibility_time);
        if (invincibility_time > 0) {
            ball.p.y = -arena_half_size.y + ball.half_size.y;
            ball.dp.y *= -1;
        } else {
#endif
            current_level = 0;
            start_game(current_level);
#if DEVELOPMENT
        }
#endif
    }

#if DEVELOPMENT
    if pressed(BUTTON_LEFT) start_game(current_level - 1);
    if pressed(BUTTON_RIGHT) start_game(current_level + 1);
    if is_down(BUTTON_UP) invincibility_time = max(1.f, invincibility_time + dt);
    if pressed(BUTTON_DOWN) dt_multiplier = 10.f;
    if released(BUTTON_DOWN) dt_multiplier = 1.f;
#endif

    if (advance_level) start_game(current_level + 1);
}

void set_slowmotion(b32 sl) {
    slowmotion = sl;
}
