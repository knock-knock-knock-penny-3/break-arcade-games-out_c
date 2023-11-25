#ifndef BREAKOUT_UTILS_H_INCLUDED
#define BREAKOUT_UTILS_H_INCLUDED

#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;

typedef int16_t s16;
typedef uint16_t u16;

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef int b32;

#define MAX_U32 UINT32_MAX

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

#define true 1
#define false 0

#define global_variable static
#define internal static

global_variable b32 running = true;

typedef struct {
    v2i screen_size;
    v2 screen_center;

    v2 arena_half_size;
    v2 arena_center;

    SDL_Renderer *renderer;
} Game;

typedef struct {
    int r, g, b, a;
} RGBA;

#define array_count(a) (sizeof(a) / sizeof(a[0]))

#if DEVELOPMENT
#define assert(c) {if (!c) {*(int*)0=0;}else{}}
#else
#define assert(c)
#endif

#define invalid_default_case default: {assert(0);}
#define invalid_code_path assert(0);

internal void zero_size(void* mem, u64 size) {
    u8* dest = (u8*)mem;
    for (u64 i = 0; i < size; i++) {
        *dest++ = 0;
    }
}
#define zero_array(a) zero_size(a, sizeof(a))
#define zero_struct(s) zero_size(&(s), sizeof(s))

#endif // BREAKOUT_UTILS_H_INCLUDED
