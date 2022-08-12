/**************************************************************
 *         _____    __                       _____            *
 *        /  _  \  |  |    ____  ___  ___   /  |  |           *
 *       /  /_\  \ |  |  _/ __ \ \  \/  /  /   |  |_          *
 *      /    |    \|  |__\  ___/  >    <  /    ^   /          *
 *      \____|__  /|____/ \___  >/__/\_ \ \____   |           *
 *              \/            \/       \/      |__|           *
 *                                                            *
 **************************************************************
 *    (c) Free Lunch Design 2003                              *
 *    by Johan Peitz - http://www.freelunchdesign.com         *
 *    SDL2 port by carstene1ns - https:/f4ke.de/dev/alex4     *
 **************************************************************
 *    This source code is released under the The GNU          *
 *    General Public License (GPL). Please refer to the       *
 *    document license.txt in the source directory or         *
 *    http://www.gnu.org for license information.             *
 **************************************************************/

#ifndef SDL_PORT_H
#define SDL_PORT_H

#include <SDL.h>
#include <SDL_mixer.h>
#include "options.h"

// types
typedef struct {
	SDL_Texture *tex;
	int w;
	int h;
} BITMAP;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

// key
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#define KEY_LCONTROL SDL_SCANCODE_LCTRL
#define KEY_ALT SDL_SCANCODE_LALT
#define KEY_ESC SDL_SCANCODE_ESCAPE
#define KEY_ENTER SDL_SCANCODE_RETURN
#define KEY_SPACE SDL_SCANCODE_SPACE
#define KEY_BACKSPACE SDL_SCANCODE_BACKSPACE
#define KEY_1 SDL_SCANCODE_1
#define KEY_2 SDL_SCANCODE_2
#define KEY_3 SDL_SCANCODE_3
#define KEY_4 SDL_SCANCODE_4
#define KEY_F1 SDL_SCANCODE_F1
#define KEY_F12 SDL_SCANCODE_F12
#define KEY_DEL SDL_SCANCODE_DELETE
#define KEY_INSERT SDL_SCANCODE_INSERT

void update_platform_controls();
extern const Uint8 *key;

// init
void init_platform();
void make_window(Toptions *o);
void uninit_platform();

#define SOUND_FREQ MIX_DEFAULT_FREQUENCY

#endif
