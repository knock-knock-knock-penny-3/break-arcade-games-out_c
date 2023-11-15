#include "main.h"
#include "math.h"

int clamp(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

inline v2 add_v2(v2 a, v2 b) {
    return (v2){a.x + b.x, a.y + b.y};
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

u32 make_color_from_grey(u8 grey) {
    return (grey << 0) | (grey << 8) | (grey << 16) | (0xFF << 24);
}

u32 make_color(u8 r, u8 g, u8 b) {
    return (b << 0) | (g << 8) | (r << 16) | (0xFF << 24);
}

inline f32 lerp(f32 a, f32 t, f32 b) {
    return (1-t)*a + t*b;
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
