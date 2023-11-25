#include "main.h"
#include "game.h"

Ball balls[16];
int next_ball;

Particle particles[1024];
int next_particle;

v2 player_target_p;
v2 player_target_dp;
v2 player_half_size;
v2 base_player_half_size;
int player_life;

v2 player_visual_p;
v2 player_visual_dp;
f32 player_squeeze_factor;
f32 player_squeeze_factor_d;

f32 arena_left_wall_visual_p;
f32 arena_left_wall_visual_dp;
f32 arena_right_wall_visual_p;
f32 arena_right_wall_visual_dp;
f32 arena_top_wall_visual_p;
f32 arena_top_wall_visual_dp;

int score;
int touchless_bonus;

Block blocks[1024];
int num_blocks;
int blocks_destroyed;

b32 first_ball_movement = true;

b32 initialized = false;

Power_Block power_blocks[16];
int next_power_block;
v2 power_block_half_size;
f32 invincibility_t;        // in seconds
f32 comet_t;                // in seconds
int number_of_triple_shots;
f32 strong_blocks_t;        // in seconds
f32 inverted_controls_t;    // in seconds
f32 slow_player_t;          // in seconds

Level_State level_state;

Level current_level;
#if DEVELOPMENT
b32 slowmotion = false;
f32 dt_multiplier = 1.f;
b32 advance_level = false;
#endif

internal void spawn_particle(v2 p, v2 half_size, f32 life, u32 color) {
    Particle *particle = particles + next_particle++;
    if (next_particle >= array_count(particles)) next_particle = 0;

    particle->p = p;
    particle->half_size = half_size;
    particle->life = life;
    particle->color = color;
}

internal int random_powerup() {
    return random_int_in_range(1, POWERUP_LAST);
}

internal int random_powerdown() {
    return random_int_in_range(POWERUP_LAST + 1, POWER_COUNT - 1);
}

internal void reset_and_reverse_ball_dp_y(Ball *ball) {
    if (ball->dp.y > 0) ball->dp.y = -ball->base_speed * ball->speed_multiplier;
    else ball->dp.y = ball->base_speed * ball->speed_multiplier;
}

internal f32 calculate_speed_adjustment(Ball *ball) {
    f32 time_to_player = 2.f;
    f32 dist_to_player = ball->p.y - player_visual_p.y;
    f32 result = (dist_to_player / time_to_player) / ball->base_speed;

    return result;
}

internal void process_ball_when_dp_y_down(Ball *ball) {
    if (ball->flags & BALL_DESTROYED_ON_DP_Y_DOWN) {
        ball->flags &= ~BALL_ACTIVE;
    }

    // If we're supposed to be a fixed speed ball, calculate and set the dp
    if (ball->flags & BALL_FIXED_SPEED) {
        if (balls[0].dp.y < 0) {
            ball->dp.y *= calculate_speed_adjustment(ball);
        }
    }
}

internal Block* get_next_available_block() {
    Block *result = blocks + num_blocks++;
    if (num_blocks >= array_count(blocks)) {
        num_blocks = 0;
    }
    return result;
}

internal Ball* get_next_available_ball_and_zero() {
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
        ball = get_next_available_ball_and_zero();
        ball->base_speed = 75;
//        ball->p.x = balls[0].p.x; // Better solution?
//        ball->p.y = balls[0].p.y; // New balls spawns from main ball
        ball->p.x = player_visual_p.x;
        ball->p.y = player_visual_p.y + player_half_size.y;
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

    for (int i = 0; i < array_count(block->neighbours); i++) {
        if (!block->neighbours[i]) break;
        if (block->neighbours[i]->life) {
            block->neighbours[i]->life = 0;
            test_for_win_condition();
        }
    }
}

internal b32 do_ball_block_collision(Ball *ball, Block *block) {
    if (block->flags & BLOCK_RIVAL_A &&
        ball->flags & BALL_RIVAL_B) return false;
    if (block->flags & BLOCK_RIVAL_B &&
        ball->flags & BALL_RIVAL_A) return false;

    v2 collision_point, diff, t, target;

    diff.y = ball->collision_test_p.y - ball->p.y;
    if (diff.y != 0) {
        if (ball->dp.y > 0) collision_point.y = block->p.y - block->half_size.y - ball->half_size.y;
        else collision_point.y = block->p.y + block->half_size.y + ball->half_size.y;

        t.y = (collision_point.y - ball->p.y) / diff.y;
        if (t.y >= 0.f && t.y <= 1.f) {
            target.x = lerp(ball->p.x, t.y, ball->collision_test_p.x);
            if (target.x + ball->half_size.x > block->p.x - block->half_size.x &&
                target.x - ball->half_size.x < block->p.x + block->half_size.x) {
                ball->desired_p.y = lerp(ball->p.y, t.y, ball->collision_test_p.y);
                if (block->ball_speed_multiplier > ball->speed_multiplier) ball->speed_multiplier = block->ball_speed_multiplier;

                if (ball->dp.y > 0) {
                    if (comet_t <= 0) {
                        reset_and_reverse_ball_dp_y(ball);
                        process_ball_when_dp_y_down(ball);
                    }
                }
                else reset_and_reverse_ball_dp_y(ball);

                if (strong_blocks_t <= 0) {
                    block->life--;
                    if (!block->life) block_destroyed(block);
                }
                return true;
            }
        }
    }
    diff.x = ball->collision_test_p.x - ball->p.x;
    if (diff.x != 0) {
        if (ball->dp.x > 0) collision_point.x = block->p.x - block->half_size.x - ball->half_size.x;
        else collision_point.x = block->p.x + block->half_size.x + ball->half_size.x;

        t.x = (collision_point.x - ball->p.x) / diff.x;
        if (t.x >= 0.f && t.x <= 1.f) {
            target.y = lerp(ball->p.y, t.x, ball->collision_test_p.y);
            if (target.y + ball->half_size.y > block->p.y - block->half_size.y &&
                target.y - ball->half_size.y < block->p.y + block->half_size.y) {
                ball->desired_p.x = lerp(ball->p.x, t.x, ball->collision_test_p.x);
                ball->dp.x *= -1;
                if (block->ball_speed_multiplier > ball->speed_multiplier) ball->speed_multiplier = block->ball_speed_multiplier;

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

internal void create_invader(v2 p) {
    char *invader[] = {
        "  0     0",
        "   0   0",
        "  0000000",
        " 00 000 00",
        "00000000000",
        "0 0000000 0",
        "0 0     0 0",
        "   00 00 "
    };

    f32 block_half_size = .8f;
    p.x -= block_half_size * 11;
    f32 original_x = p.x;

    for (int i = 0; i < array_count(invader); i++) {
        char *at = invader[i];
        while (*at) {
            if (*at++ != ' ') {
                Block *block = get_next_available_block();
                block->life = 1;
                block->half_size = (v2){block_half_size, block_half_size};
                block->relative_p = p;
                block->color = make_color_from_grey(255);
                block->ball_speed_multiplier = 1 + (f32)(array_count(invader) - i) * .5f / array_count(invader);

                if (random_choice(20)) {
                    block->power_block = random_powerup();
                }
            }

            p.x += block_half_size * 2.f;
        }
        p.y -= block_half_size * 2.f;
        p.x = original_x;
    }
}

internal void calculate_all_neighbours() {
    for_each_block {
        if (!block->life) break;

        for (Block *test_block = blocks; test_block  != blocks + array_count(blocks); test_block ++) {
            if (!test_block->life) break;
            if (test_block == block) continue;
            if (block->relative_p.x != test_block->relative_p.x && block->relative_p.y != test_block->relative_p.y) continue;

            v2 diff = sub_v2(block->relative_p, test_block->relative_p);
            f32 len = len_sq(diff);
            f32 size = max(block->half_size.x, block->half_size.y);
            if (len < square(size * 2.2f)) {
                for (int i = 0; i < array_count(block->neighbours); i++) {
                    if (block->neighbours[i]) continue;
                    block->neighbours[i] = test_block;
                    break;
                }
            }
        }
    }
}

void create_block_block(int num_x, int num_y, v2 relative_spacing, f32 x_offset, f32 y_offset, v2 block_half_size, f32 base_speed_multiplier, int rivalry) {
    x_offset += (f32)num_x * block_half_size.x * (2.f + (relative_spacing.x * 2.f)) * .5f - block_half_size.x * (1.f + relative_spacing.x);
    y_offset += -2.f;

    for (int y = 0; y < num_y; y++) {
        for (int x = 0; x < num_x; x++) {
            Block *block = get_next_available_block();

            block->life = 1;
            block->half_size = block_half_size;

            block->relative_p.x = x * block->half_size.x * (2.f + relative_spacing.x * 2.f) - x_offset;
            block->relative_p.y = y * block->half_size.y * (2.f + relative_spacing.y * 2.f) - y_offset;

            u8 k = y * 255 / num_y;
            block->color = make_color(255, k, 128);
            block->ball_speed_multiplier = base_speed_multiplier + (f32)y * 1.25f / num_y;

            if (rivalry == 1) {
                block->flags |= BLOCK_RIVAL_A;
                block->color = RIVAL_A_COLOR;
            } else if (rivalry == 2) {
                block->flags |= BLOCK_RIVAL_B;
                block->color = RIVAL_B_COLOR;
            }
        }
    }
}

inline void test_for_win_condition() {
    blocks_destroyed++;
    score += player_life + touchless_bonus++;

    if (blocks_destroyed == num_blocks) {
        advance_level = true;
    }
}

internal void simulate_level(Game *game, Level level, f32 dt) {
    switch (level) {
        case L05_PONG: {
            Level_Pong_State *pong = &level_state.pong;

            v2 ddp = mul_v2(pong->enemy_dp, -15.f);
            if (balls[0].p.x > pong->enemy_p.x) ddp.x += 100.f;
            else if (balls[0].p.x < pong->enemy_p.x) ddp.x += -100.f;

            v2 desired_dp = add_v2(pong->enemy_dp, mul_v2(ddp, dt));
            v2 desired_p = add_v2(
                                add_v2(pong->enemy_p, mul_v2(desired_dp, dt)),
                                mul_v2(ddp, square(dt))
                            );

            if (desired_p.x > arena_right_wall_visual_p - pong->enemy_half_size.x) {
                desired_p.x = arena_right_wall_visual_p - pong->enemy_half_size.x;
                desired_dp = mul_v2(desired_dp, -.5f);
            } else if (desired_p.x < arena_left_wall_visual_p + pong->enemy_half_size.x) {
                desired_p.x = arena_left_wall_visual_p + pong->enemy_half_size.x;
                desired_dp = mul_v2(desired_dp, -.5f);
            }

            pong->enemy_dp = desired_dp;
            pong->enemy_p = desired_p;
        } break;

        case L06_INVADERS: {
            Level_Invaders_State *invaders = &level_state.invaders;
            invaders->do_invader_player_collision_test = false;

            invaders->movement_t += dt;
            if (invaders->movement_t >= invaders->movement_target) {
                invaders->movement_t -= invaders->movement_target;

                if (invaders->move_down) {
                    invaders->enemy_p.y -= 2.5f;
                    invaders->move_down = false;
                    invaders->do_invader_player_collision_test = true;
                } else if (invaders->is_moving_right) {
                    invaders->enemy_p.x += 2.5f;
                    if (invaders->enemy_p.x >= 25) {
                        invaders->is_moving_right = !invaders->is_moving_right;
                        invaders->move_down = true;
                        }
                } else {
                    invaders->enemy_p.x -= 2.5f;
                    if (invaders->enemy_p.x <= -25) {
                        invaders->is_moving_right = !invaders->is_moving_right;
                        invaders->move_down = true;
                    }
                }
            }
        } break;
    }
}

internal void simulate_block_for_level(Game *game, Block *block, Level level) {
    switch (level) {
        case L05_PONG: {
            block->p = add_v2(block->relative_p, level_state.pong.enemy_p);
        } break;

        case L06_INVADERS: {
            block->p = add_v2(block->relative_p, level_state.invaders.enemy_p);
            if (level_state.invaders.do_invader_player_collision_test) {
                if (block->p.y - block->half_size.y < player_target_p.y + player_half_size.y) {
                    lose_life(game);
                }
            }
        } break;

        default: {
            block->p = block->relative_p;
        }
    }
}

inline void lose_life(Game *game) {
    player_life--;
    if (!player_life) {
        start_game(game, current_level);
        return;
    }

    zero_array(balls);
    zero_array(power_blocks);

    first_ball_movement = true;
    init_ball(balls);

    reset_power();
}

inline void reset_power() {
    invincibility_t = 0.f;
    comet_t = 0.f;
    number_of_triple_shots = 0;
    strong_blocks_t = 0.f;
    inverted_controls_t = 0.f;
    slow_player_t = 0.f;
}

inline void init_ball(Ball *ball) {
    ball->base_speed = 50;
    ball->p.x = 0.f;
    ball->p.y = 40;
    ball->dp.x = 0.f;
    ball->dp.y = -ball->base_speed;
    ball->half_size = (v2){.75, .75};
    ball->speed_multiplier = 1.f;
    ball->flags |= BALL_ACTIVE;
    ball->desired_p = ball->p;
    ball->collision_test_p = ball->p;
}

inline void start_game(Game *game, Level level) {
    advance_level = false;

    if (level >= L_COUNT) level = 0;
    else if (level < 0) level = L_COUNT - 1;

    player_life = 3;
    current_level = level;

    zero_struct(level_state);

    zero_array(balls);
    next_ball = 0;
    zero_array(power_blocks);
    next_power_block = 0;
    zero_array(blocks);
    zero_array(particles);
    next_particle = 0;

    first_ball_movement = true;
    init_ball(balls);

    player_target_p.y = -40;
    player_half_size = (v2){10, 2};
    base_player_half_size = player_half_size;

    game->arena_half_size = (v2){85, 45};
    game->arena_center = (v2){0, 0};
    arena_left_wall_visual_p = -game->arena_half_size.x;
    arena_left_wall_visual_dp = 0.f;
    arena_right_wall_visual_p = game->arena_half_size.x;
    arena_right_wall_visual_dp = 0.f;
    arena_top_wall_visual_p = game->arena_half_size.y;
    arena_top_wall_visual_dp = 0.f;

    reset_power();

    num_blocks = 0;
    blocks_destroyed = 0;
    for_each_block {
        block->life = 0;
    }

    switch (level) {
        case L01_NORMAL: {
            create_block_block(16, 7, (v2){.1f, .1f}, 0.f, 0.f, (v2){4.8f, 2.4f}, 1.f, 0);
        } break;

        case L02_WALL: {

            int power_block = 1;

            int num_x = 20;
            int num_y = 9;
            f32 block_x_half_size = 4.f;
            f32 x_offset = (f32)num_x * block_x_half_size * 2.f *.5f - block_x_half_size*.5f;
            f32 y_offset = -4.f;
            for (int y = 0; y < num_y; y++) {
                for (int x = 0; x < num_x; x++) {
                    Block *block = get_next_available_block();

                    block->life = 1;
                    block->half_size = (v2){block_x_half_size, 2};

                    block->relative_p.x = x*block->half_size.x*2.0f - x_offset;
                    if (y % 2) {
                        if (x == 0) {
                            block->half_size.x *= .5f;
                            block->relative_p.x += block->half_size.x;
                        }
                    } else {
                        if (x == num_x - 1) block->half_size.x *= .5f;
                        block->relative_p.x += block->half_size.x;
                    }
                    block->relative_p.y = y*block->half_size.y*2.0f - y_offset;

                    u8 k = (u8)(y * 255 / num_y);
                    block->color = make_color(k/2, k, 128);
                    block->ball_speed_multiplier = 1+ (f32)y*1.25f/(f32)num_y;

                    if (y <= 2) {
                        if (random_choice(5)) block->power_block = random_powerup();
                    } else if (y >= 6) {
                        if (random_choice(6)) block->power_block = random_powerdown();
                    }
                }

            }
        } break;

        case L03_STADIUM: {
            create_block_block(8, 8, (v2){.1f, .1f}, 40.f, 0.f, (v2){4.f, 2.f}, 1.f, 1);
            create_block_block(8, 8, (v2){.1f, .1f}, -40.f, 0.f, (v2){4.f, 2.f}, 1.f, 2);

            init_ball(balls + 1);

            f32 middle_point_y = (game->arena_half_size.y + (player_target_p.y + player_half_size.y)) * .5f;
            balls[0].p.y = middle_point_y;
            balls[1].p.y = middle_point_y;

            balls[1].dp.y = balls[0].base_speed;
            balls[1].flags |= BALL_ACTIVE | BALL_RIVAL_B | BALL_FIXED_SPEED;
            balls[0].flags |= BALL_RIVAL_A | BALL_FIXED_SPEED;
        } break;

        case L04_CHESS: {
            create_block_block(20, 4, (v2){0.f, 1.f}, 0.f, 0.f, (v2){4.f, 2.f}, 1.f, 1);
            create_block_block(20, 4, (v2){0.f, 1.f}, 0.f, 4.f, (v2){4.f, 2.f}, 1.f, 2);

            create_block_block(20, 2, (v2){0.f, 0.f}, 0.f, -32.f, (v2){4.f, 2.f}, 1.f, 0);

            init_ball(balls + 1);

            balls[1].dp.y = balls[0].base_speed;
            balls[1].flags |= BALL_ACTIVE | BALL_RIVAL_B | BALL_FIXED_SPEED;
            balls[0].flags |= BALL_RIVAL_A | BALL_FIXED_SPEED;
        } break;

        case L05_PONG: {
            strong_blocks_t += 5000.f;

            int num_x = 12;
            int num_y = 3;
            v2 block_half_size = {1.5f, 1.5f};
            create_block_block(num_x, num_y, (v2){.05f, .05f}, 0.f, -30.f, block_half_size, 2.f, 0);
            level_state.pong.enemy_half_size.x = num_x * (block_half_size.x * 1.05f);

            for_each_block {
                if (!block->life) continue;

                if (random_choice(3)) {
                    block->power_block = random_powerdown();
                }
            }

        } break;

        case L06_INVADERS: {
            for (int i = 0; i < 3; i++) {
                f32 y = i * 20.f;
                create_invader((v2){-50, y});
                create_invader((v2){-25, y});
                create_invader((v2){0, y});
                create_invader((v2){25, y});
                create_invader((v2){50, y});
            }

            level_state.invaders.movement_target = 1.25f;
            level_state.invaders.enemy_p.x = -25.f;
            level_state.invaders.is_moving_right = true;

            calculate_all_neighbours();
        } break;

        invalid_default_case;
    }
}

void simulate_game(Game *game, Input *input, f64 dt) {
    dt *= dt_multiplier;

    if (!initialized) {
        initialized = true;
        current_level = 0;
        start_game(game, current_level);

        power_block_half_size = (v2){2, 2};
    }

    // player movement
    v2 player_desired_p;
    {
        f32 speed_multiplier = 1.f;
        if (slow_player_t > 0) speed_multiplier = .1f;

        f32 mouse_world_dp = speed_multiplier * clampf(-100.f, pixels_dp_to_world(game, input->mouse_dp).x, 100.f);

        if (inverted_controls_t <= 0)
            player_desired_p.x = player_target_p.x + mouse_world_dp;
        else
            player_desired_p.x = player_target_p.x - mouse_world_dp;

        // Wall collision
        f32 left_most_p = arena_left_wall_visual_p + base_player_half_size.x;
        if (player_desired_p.x < left_most_p) {
            player_squeeze_factor_d = (player_desired_p.x - left_most_p) * -1.f;
            player_desired_p.x = left_most_p;
            player_target_dp.x = 0.f;
        }

        f32 right_most_p = arena_right_wall_visual_p - base_player_half_size.x;
        if (player_desired_p.x > right_most_p) {
            player_squeeze_factor_d = (player_desired_p.x - right_most_p) * 1.f;
            player_desired_p.x = right_most_p;
            player_target_dp.x = 0.f;
        }

        player_desired_p.y = player_target_p.y;
        player_visual_p.y = player_target_p.y;

        f32 player_squeeze_factor_dd = 100.f * -player_squeeze_factor + 10.f * -player_squeeze_factor_d;
        player_squeeze_factor_d += player_squeeze_factor_dd * dt;
        player_squeeze_factor += player_squeeze_factor_dd * square(dt) * .5f + player_squeeze_factor_d * dt;

        // Spring effect
        v2 player_visual_ddp = {0};
        player_visual_ddp.x = 1500.f * (player_desired_p.x - player_visual_p.x) + 40.f * (0 - player_visual_dp.x);
        player_visual_dp = add_v2(player_visual_dp, mul_v2(player_visual_ddp, dt));
        player_visual_p = add_v2(player_visual_p, add_v2(
            mul_v2(player_visual_dp, dt),
            mul_v2(player_visual_ddp, square(dt) * .5f)
        ));

        // Deform effect
        player_half_size.x = base_player_half_size.x + absf(player_target_dp.x * 1.f * dt) - player_squeeze_factor;
        player_half_size.y = max(.5f, 2.f - absf(player_target_dp.x * .05f * dt) + player_squeeze_factor);
    }

    // Update balls
    for_each_ball {
        if (!(ball->flags & BALL_ACTIVE)) continue;
        ball->desired_p = add_v2(ball->p, mul_v2(ball->dp, dt));

    #if DEVELOPMENT
        if (slowmotion) {
            ball->desired_p = add_v2(ball->p, mul_v2(div_v2(ball->dp, 10), dt));
        }
    #endif

        if (ball->dp.y < 0 && is_colliding(player_visual_p, player_half_size, ball->desired_p, ball->half_size)) {
            // Ball collision with player
            reset_and_reverse_ball_dp_y(ball);
            ball->dp.x = (ball->p.x - player_visual_p.x) * 7.5f;
            ball->dp.x += clampf(-25.f, player_target_dp.x * .5f, 25.f);
            ball->desired_p.y = player_visual_p.y + player_half_size.y;
            first_ball_movement = false;
            touchless_bonus = 0;

            if (number_of_triple_shots) {
                number_of_triple_shots--;
                spawn_triple_shot_balls();
            }
        } else if (ball->desired_p.x + ball->half_size.x > arena_right_wall_visual_p) {
            // Ball collision with right border
            ball->desired_p.x = arena_right_wall_visual_p - ball->half_size.x;
            ball->dp.x = max(20, ball->dp.x);
            ball->dp.x *= -1;
            arena_right_wall_visual_dp = -30.f;
        } else if (ball->desired_p.x - ball->half_size.x < arena_left_wall_visual_p) {
            // Ball collision with left border
            ball->desired_p.x = arena_left_wall_visual_p + ball->half_size.x;
            ball->dp.x *= -1;
            ball->dp.x = max(20, ball->dp.x);
            arena_left_wall_visual_dp = 30.f;
        }

        if (ball->desired_p.y + ball->half_size.y > arena_top_wall_visual_p) {
            // Ball collision with top border
            ball->desired_p.y = arena_top_wall_visual_p - ball->half_size.y;
            ball->dp.y = max(20, ball->dp.y);
            reset_and_reverse_ball_dp_y(ball);
            arena_top_wall_visual_dp = -30.f;
            process_ball_when_dp_y_down(ball);
        }

        if (ball->desired_p.y - ball->half_size.y < -50) {
            // Ball falling down
            if (invincibility_t <= 0) lose_life(game);    // LOST
            else reset_and_reverse_ball_dp_y(ball);                 // INVINCIBILITY
        }

        ball->collision_test_p = ball->desired_p;
    }

    simulate_level(game, current_level, dt);

    clear_arena_screen(game, game->arena_center, arena_left_wall_visual_p, arena_right_wall_visual_p, arena_top_wall_visual_p, 0xFF551100);

    for_each_block {
        if (!block->life) continue;

        simulate_block_for_level(game, block, current_level);

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

        if (is_colliding(player_visual_p, player_half_size, power_block->p, power_block_half_size)) {
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
                    lose_life(game);
                } break;

                case POWER_STRONG_BLOCKS: {
                    strong_blocks_t += 5.f;
                } break;

                case POWER_INVERTED_CONTROLS: {
                    inverted_controls_t += 5.f;
                } break;

                case POWER_SLOW_PLAYER: {
                    slow_player_t += 5.f;
                } break;

                invalid_default_case;
            }
            power_block->kind = POWER_INACTIVE;
        }

        if (power_block->kind <= POWERUP_LAST) draw_rect(game, power_block->p, power_block_half_size, 0xFFFFFF00);
        else draw_rect(game, power_block->p, power_block_half_size, 0xFFFF0000);
    }

    // Render particles
    for (int i = 0; i < array_count(particles); i++) {
        Particle *particle= particles + i;
        if (particle->life <= 0.f) continue;

        u8 alpha = particle->life / .32f * 255 * .5f;
        draw_rect(game, particle->p, particle->half_size, set_alpha(particle->color, alpha));
        particle->life -= dt;
    }

    // Render balls
    for_each_ball {
        if (!(ball->flags & BALL_ACTIVE)) continue;

        ball->p = ball->desired_p;

        ball->trail_spawner_t -= dt;
        while (ball->trail_spawner_t <= 0.f) {
            ball->trail_spawner_t += .005f;

            u32 color = 0xFF00FFFF;
            if (comet_t > 0.f) color = 0xFFFF0000;
            else if (ball->flags & BALL_DESTROYED_ON_DP_Y_DOWN) color = 0xFFFFFF00;

            spawn_particle(ball->p, ball->half_size, .32f, color);
        }

        if (ball->flags & BALL_RIVAL_A) draw_rect(game, ball->p, ball->half_size, RIVAL_A_COLOR);
        else if (ball->flags & BALL_RIVAL_B) draw_rect(game, ball->p, ball->half_size, RIVAL_B_COLOR);
        else draw_rect(game, ball->p, ball->half_size, 0xFFFFFFFF);
    }

    // Player render
    {
        player_target_dp.x = (player_desired_p.x - player_visual_p.x) / dt;
        player_target_p = player_desired_p;

        if (invincibility_t > 0) {
            invincibility_t -= dt;
            draw_rect(game, player_visual_p, player_half_size, 0xFFFFFFFF);
        }
        else draw_rect(game, player_visual_p, player_half_size, 0xFF00FF00);
//        draw_rect(game, player_target_p, player_half_size, 0xFF00FF00); // player without spring effect
    }

    // Wall movements
    {
        f32 arena_left_wall_visual_ddp = 150.f * (-game->arena_half_size.x - arena_left_wall_visual_p) + 7.f * (-arena_left_wall_visual_dp);
        arena_left_wall_visual_dp += arena_left_wall_visual_ddp * dt;
        arena_left_wall_visual_p += arena_left_wall_visual_ddp * square(dt) * .5f + arena_left_wall_visual_dp * dt;

        f32 arena_right_wall_visual_ddp = 150.f * (game->arena_half_size.x - arena_right_wall_visual_p) + 7.f * (-arena_right_wall_visual_dp);
        arena_right_wall_visual_dp += arena_right_wall_visual_ddp * dt;
        arena_right_wall_visual_p += arena_right_wall_visual_ddp * square(dt) * .5f + arena_right_wall_visual_dp * dt;

        f32 arena_top_wall_visual_ddp = 150.f * (game->arena_half_size.y - arena_top_wall_visual_p) + 7.f * (-arena_top_wall_visual_dp);
        arena_top_wall_visual_dp += arena_top_wall_visual_ddp * dt;
        arena_top_wall_visual_p += arena_top_wall_visual_ddp * square(dt) * .5f + arena_top_wall_visual_dp * dt;

        draw_arena_rects(game, game->arena_center, arena_left_wall_visual_p, arena_right_wall_visual_p, arena_top_wall_visual_p, 0xFF220500);
    }

    if (comet_t > 0) comet_t -= dt;
    if (strong_blocks_t > 0) strong_blocks_t -= dt;
    if (inverted_controls_t > 0) inverted_controls_t -= dt;
    if (slow_player_t > 0) slow_player_t -= dt;

    if (advance_level) start_game(game, current_level + 1);

    for (int i = 0; i < player_life; i++) {
        draw_rect(game, (v2){-game->arena_half_size.x - 1.f + (i * 2.5f), game->arena_half_size.y + 2.5f}, (v2){1, 1}, 0xFFFFFFFF);
    }
    draw_number(game, score, (v2){-game->arena_half_size.x + 15.f, game->arena_half_size.y + 2.5f}, 2.5f, 0xFFFFFFFF);

#if DEVELOPMENT
    if pressed(BUTTON_LEFT) start_game(game, current_level - 1);
    if pressed(BUTTON_RIGHT) start_game(game, current_level + 1);
    if is_down(BUTTON_UP) invincibility_t = max(1.f, invincibility_t + dt);
    if pressed(BUTTON_DOWN) dt_multiplier = 10.f;
    if released(BUTTON_DOWN) dt_multiplier = 1.f;
#endif

    draw_messages(game, dt);
}

void set_slowmotion(b32 sl) {
    slowmotion = sl;
}
