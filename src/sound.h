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

#ifndef SOUND_H
#define SOUND_H

#include "sdl_port.h"
#include <SDL_mixer.h>

void play_sound_id(int id);
void play_sound_id_ex(int id, int vol, int freq, int loop);
void adjust_sound_id_ex(int id, int player_x, int x);
void stop_sound_id(int id);
void start_music(int startorder);
void stop_music(void);
void pause_music(bool p);

void load_sfx();
void free_sfx();
void load_music(int id);
void unload_music();

void set_sound_volume(int vol);
void set_music_volume(int vol);

// mod start patterns
#define MOD_INTRO_SONG  0x00
#define MOD_MENU_SONG   0x08
#define MOD_BOSS_SONG   0x0c
#define MOD_OUTRO_SONG  0x11
#define MOD_PLAYER_DIES 0x1a
#define MOD_LEVEL_DONE  0x1b
#define MOD_GAME_OVER   0x1c
#define MOD_LEVEL_SONG  0x1d

#endif
