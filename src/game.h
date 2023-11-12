#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

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
    v2 p;
    v2 dp;
    v2 half_size;
    f32 base_speed;
    f32 speed_multiplier;
} Ball;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);
void set_slowmotion(b32);
extern void start_game(Level);

#endif // BREAKOUT_GAME_H_INCLUDED
