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

#ifndef MAIN_H
#define MAIN_H

#include "alex4.h"
#include "port.h"
#include "map.h"
#include "hisc.h"
#include "actor.h"
#include "control.h"

// functions
char *get_init_string();
void textout_outline_center(BITMAP *bmp, const char *txt, int cx, int y);
int do_pause_menu(BITMAP *bg);
void set_map(Tmap *m);
void new_game(int reset_player_data);
void new_level(const char *fname, int level_id, int draw);
Tactor *get_alex();
void draw_frame(BITMAP *bmp, int draw_status_bar);
void blit_to_screen(BITMAP *bmp);
int get_string(BITMAP *bmp, char *string, int max_size, int pos_x, int pos_y, int colour, Tcontrol *pad);
BITMAP *get_gameover_sign();
BITMAP *get_letsgo_sign();
Thisc *get_space_hisc();
void show_scores(int space, Thisc *table);

#endif
