#ifndef BREAKOUT_MAIN_H_INCLUDED
#define BREAKOUT_MAIN_H_INCLUDED

#include "SDL2/SDL.h"

typedef int b32;

#define true 1
#define false 0

#define global_variable static
#define internal static

static int running = true;

#define WINDOW_TITLE    "Breakout"
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720

#endif // BREAKOUT_MAIN_H_INCLUDED
