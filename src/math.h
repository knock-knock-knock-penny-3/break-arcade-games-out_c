#ifndef BREAKOUT_MATH_H_INCLUDED
#define BREAKOUT_MATH_H_INCLUDED

// PROTOTYPES
int clamp(int, int, int);
f32 clampf(f32, f32, f32);
extern v2 add_v2(v2, v2);
extern v2i add_v2i(v2i, v2i);
extern v2 sub_v2(v2, v2);
extern v2i sub_v2i(v2i, v2i);
extern v2 mul_v2(v2, f32);
extern v2 div_v2(v2, f32);
extern v2i div_v2i(v2i, int);
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
extern f32 random_unilateral();
extern f32 random_bilateral();
extern f32 random_f32_in_range(f32, f32);
extern f32 len_sq(v2);
extern v2 v2i_to_v2(v21);
extern v2i v2_to_v2i(v2);
extern f32 map_into_range_normalized(f32, f32, f32);
extern u8 map_color_into_range_normalized(u8, f32, u8);

#endif // BREAKOUT_MATH_H_INCLUDED
