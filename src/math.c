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

inline v2 mul_v2(v2 a, f32 s) {
    return (v2){a.x * s, a.y * s};
}

inline v2 div_v2(v2 a, f32 s) {
    return (v2){a.x / s, a.y / s};
}

u32 make_color_from_grey(u8 grey) {
    return (grey << 0) | (grey << 8) | (grey << 16) | (0xFF << 24);
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
