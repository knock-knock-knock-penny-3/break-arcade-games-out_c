#ifndef BREAKOUT_GAME_H_INCLUDED
#define BREAKOUT_GAME_H_INCLUDED

typedef struct {
    v2 p;
    v2 half_size;
    f32 ball_speed_multiplier;
    int life;
    u32 color;
} Block;

typedef enum {
    GM_NORMAL,
    GM_WALL,
    GM_CONSTRUCTION,
    GM_SPACED,
    GM_POWERUPS,
    GM_PONG,

    GM_COUNT,
} Game_Modes;

typedef enum {
    POWERUP_INACTIVE,
    POWERUP_INVINCIBILITY,
    POWERUP_TRIPLE_SHOT,
} Powerup_Kind;

typedef struct {
    Powerup_Kind kind;
    v2 p;
} Powerup;

//typedef struct {
//    Powerup powerups[16];
//    int next_power_up;
//
//    f32 invincibility_time;
//    int number_of_triple_shots;
//} GM_Powerups_State;

typedef struct {
    union {
//        GM_Powerups_State;
        struct {
            Powerup powerups[16];
            int next_powerup;

            f32 invincibility_time;
            int number_of_triple_shots;
        };
    };
} Game_Mode_State;

// PROTOTYPES
void simulate_game(Game *, Input *, f64);
void set_slowmotion(b32);
extern void start_game(Game_Modes);

#endif // BREAKOUT_GAME_H_INCLUDED
