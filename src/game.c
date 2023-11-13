#include "main.h"
#include "game.h"

Ball balls[16];
int next_ball;

v2 player_p;
v2 player_dp;
v2 player_half_size;

Block blocks[256];
int num_blocks;
int blocks_destroyed;

v2 arena_half_size;
b32 first_ball_movement = true;

b32 initialized = false;

Power_Block power_blocks[16];
int next_power_block;
v2 power_block_half_size;
f32 invincibility_t; // in seconds
f32 comet_t; // in seconds
int number_of_triple_shots;
f32 strong_blocks_t; // in seconds

Level current_level;
#if DEVELOPMENT
b32 slowmotion = false;
f32 dt_multiplier = 1.f;
b32 advance_level = false;
#endif

internal Ball* get_next_available_ball() {
    for_each_ball {
        if (!(ball->flags & BALL_ACTIVE)) {
            zero_struct(*ball);
            return ball;
        }
    }

    invalid_code_path;
    return 0;
}

internal void spawn_triple_shot_balls() {
    Ball *ball;

    for (int i = 0; i < 2; i++) {
        ball = get_next_available_ball();
        ball->base_speed = 75;
//        ball->p.x = balls[0].p.x;
//        ball->p.y = balls[0].p.y;
        ball->p.x = player_p.x;
        ball->p.y = player_p.y + player_half_size.y;
        ball->dp.x = 45.f * (f32)(-1 * i%2);
        ball->dp.y = ball->base_speed;
        ball->half_size = (v2){.75, .75};
        ball->speed_multiplier = balls[0].speed_multiplier;
        ball->flags = BALL_ACTIVE | BALL_DESTROYED_ON_DP_Y_DOWN;
    }

}

internal void spawn_power_block(Power_Block_Kind kind, v2 p) {
    Power_Block *power_block = power_blocks + next_power_block++;
    if (next_power_block >= array_count(power_blocks)) next_power_block = 0;
    power_block->p = p;
    power_block->kind = kind;
}

internal void block_destroyed(Block *block) {
    test_for_win_condition();

    if (block->power_block) {
        spawn_power_block(block->power_block, block->p);
    }
}

internal b32 do_ball_block_collision(Ball *ball, Block *block) {
    v2 collision_point, diff, t, target;

    diff.y = ball->desired_p.y - ball->p.y;
    if (diff.y != 0) {
        if (ball->dp.y > 0) collision_point.y = block->p.y - block->half_size.y - ball->half_size.y;
        else collision_point.y = block->p.y + block->half_size.y + ball->half_size.y;

        t.y = (collision_point.y - ball->p.y) / diff.y;
        if (t.y >= 0.f && t.y <= 1.f) {
            target.x = lerp(ball->p.x, t.y, ball->desired_p.x);
            if (target.x + ball->half_size.x > block->p.x - block->half_size.x &&
                target.x - ball->half_size.x < block->p.x + block->half_size.x) {
                ball->desired_p.y = lerp(ball->p.y, t.y, ball->desired_p.y);
                if (block->ball_speed_multiplier > ball->speed_multiplier) ball->speed_multiplier = block->ball_speed_multiplier;

                if (ball->dp.y > 0) {
                    if (comet_t <= 0) {
                        if (ball->flags & BALL_DESTROYED_ON_DP_Y_DOWN) {
                            ball->flags &= ~BALL_ACTIVE;
                        }
                        ball->dp.y = -ball->base_speed * ball->speed_multiplier;
                    }
                }
                else ball->dp.y = ball->base_speed * ball->speed_multiplier;

                if (strong_blocks_t <= 0) {
                    block->life--;
                    if (!block->life) block_destroyed(block);
                }
                return true;
            }
        }
    }
    diff.x = ball->desired_p.x - ball->p.x;
    if (diff.x != 0) {
        if (ball->dp.x > 0) collision_point.x = block->p.x - block->half_size.x - ball->half_size.x;
        else collision_point.x = block->p.x + block->half_size.x + ball->half_size.x;

        t.x = (collision_point.x - ball->p.x) / diff.x;
        if (t.x >= 0.f && t.x <= 1.f) {
            target.y = lerp(ball->p.y, t.x, ball->desired_p.y);
            if (target.y + ball->half_size.y > block->p.y - block->half_size.y &&
                target.y - ball->half_size.y < block->p.y + block->half_size.y) {
                ball->desired_p.x = lerp(ball->p.x, t.x, ball->desired_p.x);
                if (block->ball_speed_multiplier > ball->speed_multiplier) ball->speed_multiplier = block->ball_speed_multiplier;

                ball->dp.x *= -1;
                if (ball->dp.y > 0) ball->dp.y = -ball->base_speed * ball->speed_multiplier;
                else ball->dp.y = ball->base_speed * ball->speed_multiplier;

                if (strong_blocks_t <= 0) {
                    block->life--;
                    if (!block->life) block_destroyed(block);
                }
                return true;
            }
        }
    }

    return false;
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
                block->power_block = POWER_STRONG_BLOCKS;
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

    zero_array(balls);
    zero_array(power_blocks);

    first_ball_movement = true;
    balls[0].base_speed = 50;
    balls[0].p.x = 0.f;
    balls[0].p.y = 40;
    balls[0].dp.x = 0.f;
    balls[0].dp.y = -balls[0].base_speed;
    balls[0].half_size = (v2){.75, .75};
    balls[0].speed_multiplier = 1.f;
    balls[0].flags |= BALL_ACTIVE;
    balls[0].desired_p = balls[0].p; //@Hack

    player_p.y = -40;
    player_half_size = (v2){10, 2};

    arena_half_size = (v2){85, 45};

    invincibility_t = 0.f;
    comet_t = 0.f;
    number_of_triple_shots = 0;
    strong_blocks_t = 0.f;

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

        power_block_half_size = (v2){2, 2};
    }

    v2 player_desired_p;
    player_desired_p.x = pixels_to_world(game, input->mouse).x;
    player_desired_p.y = player_p.y;

    // Update balls
    for_each_ball {
        if (!(ball->flags & BALL_ACTIVE)) continue;
        ball->desired_p = add_v2(ball->p, mul_v2(ball->dp, dt));

    #if DEVELOPMENT
        if (slowmotion) {
            ball->desired_p = add_v2(ball->p, mul_v2(div_v2(ball->dp, 10), dt));
        }
    #endif

        if (ball->dp.y < 0 && is_colliding(player_p, player_half_size, ball->desired_p, ball->half_size)) {
            // Ball collision with player
            ball->desired_p.y = player_p.y + player_half_size.y;
            ball->dp.x = (ball->p.x - player_p.x) * 7.5f;
            ball->dp.x += clamp(-25, player_dp.x * .5f, 25);
            ball->dp.y *= -1;
            first_ball_movement = false;

            if (number_of_triple_shots) {
                number_of_triple_shots--;
                spawn_triple_shot_balls();
            }
        } else if (ball->desired_p.x + ball->half_size.x > arena_half_size.x) {
            // Ball collision with left border
            ball->desired_p.x = arena_half_size.x - ball->half_size.x;
            ball->dp.x *= -1;
        } else if (ball->desired_p.x - ball->half_size.x < -arena_half_size.x) {
            // Ball collision with right border
            ball->desired_p.x = -arena_half_size.x + ball->half_size.x;
            ball->dp.x *= -1;
        }

        if (ball->desired_p.y + ball->half_size.y > arena_half_size.y) {
            // Ball collision with top border
            ball->desired_p.y = arena_half_size.y - ball->half_size.y;
            if (ball->flags & BALL_DESTROYED_ON_DP_Y_DOWN) {
                ball->flags &= ~BALL_ACTIVE;
            }
            ball->dp.y *= -1;
        }

        if (ball->desired_p.y - ball->half_size.y < -50) {
            // Ball falling down
            if (invincibility_t <= 0) start_game(current_level); // LOST
            else ball->dp.y *= -1.f;                                 // INVINCIBILITY
        }
    }

    clear_screen_and_draw_rect(game, (v2){0, 0}, arena_half_size, 0xFF551100, 0xFF220500);

    for (Block *block = blocks; block != blocks + array_count(blocks); block++) {
        if (!block->life) continue;

        if (!first_ball_movement) {
            for_each_ball {
                if (!(ball->flags & BALL_ACTIVE)) continue;
                do_ball_block_collision(ball, block);
            }
        }

        draw_rect(game, block->p, block->half_size, block->color);
    }

    for (Power_Block *power_block = power_blocks; power_block != power_blocks + array_count(power_blocks); power_block++) {
        if (power_block->kind == POWER_INACTIVE) continue;

        power_block->p.y -= 15 * dt;

        if (is_colliding(player_p, player_half_size, power_block->p, power_block_half_size)) {
            switch (power_block->kind) {
                case POWER_INVINCIBILITY: {
                    invincibility_t += 5.f;
                } break;

                case POWER_COMET: {
                    comet_t += 5.f;
                } break;

                case POWER_TRIPLE_SHOT: {
                    number_of_triple_shots++;
                } break;

                case POWER_INSTAKILL: {
                    start_game(current_level);
                } break;

                case POWER_STRONG_BLOCKS: {
                    strong_blocks_t += 5.f;
                } break;

                invalid_default_case;
            }
            power_block->kind = POWER_INACTIVE;
        }

        draw_rect(game, power_block->p, power_block_half_size, 0xFFFFFF00);
    }

    // Render balls
    for_each_ball {
        if (!(ball->flags & BALL_ACTIVE)) continue;
        ball->p = ball->desired_p;
        draw_rect(game, ball->p, ball->half_size, 0xFF00FFFF);
    }

    {
        player_dp.x = (player_desired_p.x - player_p.x) / dt;
        player_p = player_desired_p;

        simulate_level(game);

        if (invincibility_t > 0) {
            invincibility_t -= dt;
            draw_rect(game, player_p, player_half_size, 0xFFFFFFFF);
        }
        else draw_rect(game, player_p, player_half_size, 0xFF00FF00);
    }

    if (comet_t > 0) comet_t -= dt;
    if (strong_blocks_t > 0) strong_blocks_t -= dt;

    if (advance_level) start_game(current_level + 1);

#if DEVELOPMENT
    if pressed(BUTTON_LEFT) start_game(current_level - 1);
    if pressed(BUTTON_RIGHT) start_game(current_level + 1);
    if is_down(BUTTON_UP) invincibility_t = max(1.f, invincibility_t + dt);
    if pressed(BUTTON_DOWN) dt_multiplier = 10.f;
    if released(BUTTON_DOWN) dt_multiplier = 1.f;
#endif
}

void set_slowmotion(b32 sl) {
    slowmotion = sl;
}
