#ifndef BREAKOUT_MATH_H_INCLUDED
#define BREAKOUT_MATH_H_INCLUDED

// PROTOTYPES
int clamp(int, int, int);
extern v2 add_v2(v2, v2);
extern v2 sub_v2(v2, v2);
extern v2i sub_v2i(v2i, v2i);
extern v2 mul_v2(v2, f32);
extern v2 div_v2(v2, f32);
u32 make_color_from_grey(u8);
u32 make_color(u8, u8, u8);
extern f32 lerp(f32, f32, f32);
extern f32 absf(f32);
extern f32 min(f32, f32);
extern f32 max(f32, f32);
extern f32 square(f32);
extern u32 random_u32();
extern b32 random_b32();
extern int random_int_in_range(int, int);
extern b32 random_choice(int);
extern f32 len_sq(v2);

#endif // BREAKOUT_MATH_H_INCLUDED
