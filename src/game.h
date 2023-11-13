#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

#define BALL_ACTIVE                 0x1
#define BALL_DESTROYED_ON_DP_Y_DOWN 0x2

#define for_each_ball for (Ball *ball = balls; ball != balls + array_count(balls); ball++)

typedef enum {
    L01_NORMAL,
    L02_WALL,
    L03_STADIUM,
    L04_PONG,

    L_COUNT,

    L_NEG = -1, //@HACK TO HAVE NEGATIVE VALUES (https://stackoverflow.com/a/38010712)
} Level;

typedef enum {
    POWERUP_INACTIVE,
    POWERUP_INVINCIBILITY,
    POWERUP_TRIPLE_SHOT,
    POWERUP_COMET,
} Powerup_Kind;

typedef struct {
    Powerup_Kind kind;
    v2 p;
} Powerup;

typedef struct {
    v2 p;
    v2 half_size;
    f32 ball_speed_multiplier;
    int life;
    u32 color;

    Powerup_Kind powerup;
} Block;

typedef struct {
    u32 flags;

    v2 p;
    v2 dp;
    v2 half_size;

    v2 desired_p;

    f32 base_speed;
    f32 speed_multiplier;
} Ball;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);
void set_slowmotion(b32);
extern void start_game(Level);
extern void test_for_win_condition();

#endif // BREAKOUT_GAME_H_INCLUDED
