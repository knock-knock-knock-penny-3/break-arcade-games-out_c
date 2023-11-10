#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

typedef struct {
    v2 p;
    v2 half_size;
    f32 ball_speed_multiplier;
    int life;
    u32 color;
} Block;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);
void set_slowmotion(b32);

#endif // BREAKOUT_GAME_H_INCLUDED
