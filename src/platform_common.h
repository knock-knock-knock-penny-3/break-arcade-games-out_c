#ifndef BREAKOUT_PLATFORM_COMMON_H_INCLUDED
#define BREAKOUT_PLATFORM_COMMON_H_INCLUDED

typedef struct {
    b32 is_down, changed;
} Button;

enum {
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_COUNT,
};

typedef struct {
    v2i mouse_p;
    v2i mouse_dp;
    Button buttons[BUTTON_COUNT];
} Input;

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (input->buttons[b].is_down)

#endif // BREAKOUT_PLATFORM_COMMON_H_INCLUDED
