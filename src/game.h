#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

typedef struct {
    v2 p;
    int life;
    u32 color;
} Block;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);

#endif // BREAKOUT_GAME_H_INCLUDED
