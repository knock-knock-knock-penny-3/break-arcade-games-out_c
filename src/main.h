#ifndef BREAKOUT_MAIN_H_INCLUDED
#define BREAKOUT_MAIN_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_rotozoom.h"
#include "utils.h"
#include "math.h"
#include "software_rendering.h"
#include "platform_common.h"
#include "game.h"
#include "collision.h"
#include "console.h"

#if DEVELOPMENT
#define WINDOW_TITLE    "Breakout - DEVELOPMENT"
#else
#define WINDOW_TITLE    "Breakout"
#endif
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720

#endif // BREAKOUT_MAIN_H_INCLUDED
