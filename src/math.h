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

typedef struct {
    union {
        struct {
            int x, y;
        };
        int e[2];
    };
} v2i;

// PROTOTYPES
int clamp(int, int, int);
extern v2 add_v2(v2, v2);
extern v2i sub_v2i(v2i, v2i);
extern v2 mul_v2(v2, f32);
extern v2 div_v2(v2, f32);
u32 make_color_from_grey(u8);
extern f32 lerp(f32, f32, f32);
extern f32 absf(f32);
extern f32 min(f32, f32);
extern f32 max(f32, f32);

#endif // BREAKOUT_MATH_H_INCLUDED
