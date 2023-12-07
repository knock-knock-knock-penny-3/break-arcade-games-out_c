#include "main.h"
#include "math.h"

// MATH
int clamp(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

f32 clampf(f32 min, f32 val, f32 max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}
inline f32 absf(f32 a) {
    if (a < 0) {
        return -a;
    }
    return a;
}

inline f32 min(f32 a, f32 b) {
    if (a < b) {
        return a;
    }
    return b;
}

inline f32 max(f32 a, f32 b) {
    if (a > b) {
        return a;
    }
    return b;
}

inline f32 square(f32 a) {
    return a * a;
}

inline f32 len_sq(v2 v) {
    return square(v.x) + square(v.y);
}

inline int trunc_f32(f32 a) {
    return (int)a;
}

inline int ceil_f32(f32 a) {
    return (int)(a + 1.f);
}

// VECTOR
inline v2 add_v2(v2 a, v2 b) {
    return (v2){a.x + b.x, a.y + b.y};
}

inline v2i add_v2i(v2i a, v2i b) {
    return (v2i){a.x + b.x, a.y + b.y};
}

inline v2 sub_v2(v2 a, v2 b) {
    return (v2){a.x - b.x, a.y - b.y};
}

inline v2i sub_v2i(v2i a, v2i b) {
    return (v2i){a.x - b.x, a.y - b.y};
}

inline v2 mul_v2(v2 a, f32 s) {
    return (v2){a.x * s, a.y * s};
}

inline v2 div_v2(v2 a, f32 s) {
    return (v2){a.x / s, a.y / s};
}

inline v2i div_v2i(v2i a, int s) {
    return (v2i){a.x / s, a.y / s};
}

inline v2 v2i_to_v2(v2i a) {
    return (v2){(f32)a.x, (f32)a.y};
}

inline v2i v2_to_v2i(v2 a) {
    return (v2i){(int)a.x, (int)a.y};
}

inline f32 deg_to_rad(f32 angle) {
    return M_PI / 180.f * angle;
}

inline f32 rad_to_deg(f32 angle) {
    return angle / M_PI * 180.f;
}

inline f32 get_vector_rotation(v2 v) {
    return rad_to_deg(atan2(v.y, v.x));
}

// MATRIX
inline v2 mul_m2_v2(m2 m, v2 v) {
    return (v2){
        v.x * m._00 + v.y * m._01,
        v.x * m._10 + v.y * m._11
    };
}

// SHAPE
inline Rect2 make_rect_min_max(v2 min, v2 max) {
    Rect2 result;
    result.p[0] = min;
    result.p[1] = (v2){max.x, min.y};
    result.p[2] = max;
    result.p[3] = (v2){min.x, max.y};

    return result;
}

inline Rect2 make_rect_center_half_size(v2 c, v2 h) {
    return make_rect_min_max(sub_v2(c, h), add_v2(c, h));
}

// COLOR
u32 make_color_from_grey(u8 grey) {
    return (grey << 0) | (grey << 8) | (grey << 16) | (0xFF << 24);
}

u32 make_color(u8 r, u8 g, u8 b) {
    return (b << 0) | (g << 8) | (r << 16) | (0xFF << 24);
}

// RANDOM
u32 random_state = 12345;

inline u32 random_u32() {
    u32 result = random_state;

    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    random_state = result;

    return result;
}

inline b32 random_b32() {
    return random_u32() % 2;
}

inline b32 random_choice(int chance) {
    return random_u32() % chance == 0;
}

inline int random_int_in_range(int min, int max) { //inclusive
    int range = max - min + 1;
    int result = random_u32() % range;
    result += min;
    return result;
}

inline f32 random_unilateral() {
    return (f32)random_u32() / (f32)MAX_U32;
}

inline f32 random_bilateral() {
    return random_unilateral() * 2.f - 1.f;
}

inline f32 random_f32_in_range(f32 min, f32 max) {
    return random_unilateral() * (max - min) + min;
}

// UTILS
inline f32 lerp(f32 a, f32 t, f32 b) {
    return (1-t)*a + t*b;
}

inline f32 map_into_range_normalized(f32 min, f32 val, f32 max) {
    f32 range = max - min;
    return clampf(0.f, (val - min) / range, 1.f);
}

inline u8 map_color_into_range_normalized(u8 min, f32 val, u8 max) {
    u8 range = max - min;
    return clamp(0x00, (u8)(val * range + min), 0xFF);
}
