#ifndef BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED
#define BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED

// PROTOTYPES
void clear_screen(Game *, u32);
void draw_rect_in_pixels(Game *, int, int, int, int, u32);
void draw_rect(Game *, v2, v2, u32);
v2 pixels_to_world(Game *, v2i);
v2 pixels_dp_to_world(Game *, v2i);
f32 calculate_aspect_multiplier(Game *);
void clear_screen_and_draw_rect(Game *, v2, v2, u32, u32);
//RGBA color_converter(u32);

#endif // BREAKOUT_SOFTWARE_RENDERING_H_INCLUDED
