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

inline v2 mul_v2(v2 a, f32 s) {
    return (v2){a.x * s, a.y * s};
}

u32 make_color_from_grey(u8 grey) {
    return (grey << 0) | (grey << 8) | (grey << 16) | (0xFF << 24);
}
