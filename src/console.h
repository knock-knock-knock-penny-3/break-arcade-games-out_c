#ifndef BREAKOUT_CONSOLE_H_INCLUDED
#define BREAKOUT_CONSOLE_H_INCLUDED

typedef struct {
    int val;
    u32 color;

    f32 timer;
} Message;

// PROTOTYPES
void print_int(int, u32);
void draw_messages(Game *, f32);

#endif // BREAKOUT_CONSOLE_H_INCLUDED
