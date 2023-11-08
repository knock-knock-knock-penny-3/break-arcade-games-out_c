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
    int mouse_x, mouse_y;
    Button buttons[BUTTON_COUNT];
} Input;

#endif // BREAKOUT_PLATFORM_COMMON_H_INCLUDED
