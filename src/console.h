#ifndef BREAKOUT_CONSOLE_H_INCLUDED
#define BREAKOUT_CONSOLE_H_INCLUDED

typedef struct {
    int val;
    u32 color;
} Message;

// PROTOTYPES
void print_int(int, u32);
void draw_messages(Game *);

#endif // BREAKOUT_CONSOLE_H_INCLUDED
