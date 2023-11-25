#ifndef BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED
#define BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED

typedef enum {
    RED,
    GREEN,
    BLUE,
    ALPHA
} Color;

// PROTOTYPES
void set_screen(Game *, int, int);
void clear_screen(Game *, u32);
void draw_rect_in_pixels(Game *, int, int, int, int, u32);
void draw_rect(Game *, v2, v2, u32);
v2 pixels_to_world(Game *, v2i);
v2 pixels_dp_to_world(Game *, v2i);
f32 calculate_aspect_multiplier(Game *);
void clear_arena_screen(Game *, v2, f32, f32, f32, u32);
void draw_arena_rects(Game *, v2, f32, f32, f32, u32);
extern void draw_number(Game *, int, v2, f32, u32);
//RGBA color_converter(u32);
//u32 rgba_converter(RGBA);
u32 set_color(u32, u8, Color);

#endif // BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED
