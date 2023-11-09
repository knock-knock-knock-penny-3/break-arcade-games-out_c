#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

typedef struct {
    v2 p;
    v2 block_size;
    int life;
} Block;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);

#endif // BREAKOUT_GAME_H_INCLUDED
