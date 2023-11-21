#include "main.h"
#include "console.h"

#if DEVELOPMENT
Message messages[16];
int current_message;

void print_int(int number, u32 color) {
    Message *message = messages + current_message++;
    if (current_message >= array_count(messages)) current_message = 0;

    message->val = number;
    message->color = color;
}

void draw_messages(Game *game) {
    v2 p = (v2){game->arena_half_size.x, -game->arena_half_size.y};

    for (int i = 0; i < array_count(messages); i++) {
        Message *message = messages + i;

        draw_number(game, message->val, p, 2.f, message->color);
        p.y += 3.f;
    }
}
#else
#define draw_messages(...)
#define print_int(...)
#endif // DEVELOPMENT
