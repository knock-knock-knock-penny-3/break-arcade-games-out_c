#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

#define BALL_ACTIVE                 0x1
#define BALL_DESTROYED_ON_DP_Y_DOWN 0x2
#define BALL_RIVAL_A                0x4
#define BALL_RIVAL_B                0x8

#define BALL_FIXED_SPEED 0x10

#define BLOCK_RIVAL_A               0x1
#define BLOCK_RIVAL_B               0x2

#define RIVAL_A_COLOR   0xFF8934FF
#define RIVAL_B_COLOR   0x3478FFFF

typedef enum {
    L01_NORMAL,
    L02_WALL,
    L05_PONG,
    L06_INVADERS,

    L_COUNT,

    L03_STADIUM,
    L04_CHESS,

    L_NEG = -1, //@HACK TO HAVE NEGATIVE VALUES (https://stackoverflow.com/a/38010712)
} Level;

typedef enum {
    POWER_INACTIVE,

    // Powerups
    POWER_INVINCIBILITY,
    POWER_COMET,
//    POWER_INCREASE_BALL_SIZE,
    POWER_TRIPLE_SHOT,

    POWERUP_LAST = POWER_TRIPLE_SHOT,

    // Powerdowns
    POWER_INSTAKILL,
    POWER_STRONG_BLOCKS,
    POWER_INVERTED_CONTROLS,
    POWER_SLOW_PLAYER,

    POWER_COUNT,
} Power_Block_Kind;

typedef struct {
    Power_Block_Kind kind;
    v2 p;
} Power_Block;

typedef struct Block {
    u32 flags;

    v2 p;
    v2 relative_p;
    v2 half_size;
    f32 ball_speed_multiplier;
    int life;
    u32 color;

    Power_Block_Kind power_block;

    struct Block *neighbours[4];
} Block;

typedef struct {
    v2 p;
    v2 dp;
    v2 half_size;
    f32 angle;

    f32 life;
    f32 life_d;
    f32 max_life;
    u32 color;
} Particle;

typedef struct {
    u32 flags;

    v2 p;
    v2 dp;
    f32 half_size;
    u32 color;

    v2 collision_test_p;
    v2 desired_p;

    f32 base_speed;
    f32 speed_multiplier;

    f32 trail_spawner_t;
} Ball;

typedef struct {
    v2 enemy_p;
    v2 enemy_dp;
    v2 enemy_half_size;
} Level_Pong_State;

typedef struct {
    v2 enemy_p;
    f32 movement_t;
    f32 movement_target;
    b32 is_moving_right;
    b32 move_down;
    b32 do_invader_player_collision_test;
} Level_Invaders_State;

typedef struct {
    union {
        Level_Pong_State pong;
        Level_Invaders_State invaders;
    };
} Level_State;

typedef struct {
    u32 arena_color;
    u32 wall_color;
} Level_Info;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);
void set_slowmotion(b32);
extern void start_game(Game *, Level);
extern void test_for_win_condition();

#endif // BREAKOUT_GAME_H_INCLUDED
