#ifndef BREAKOUT_MATH_H_INCLUDED
#define BREAKOUT_MATH_H_INCLUDED

typedef struct {
    union {
        struct {
            f32 x, y;
        };
        f32 e[2];
    };
} v2;

// PROTOTYPES
int clamp(int, int, int);

#endif // BREAKOUT_MATH_H_INCLUDED
