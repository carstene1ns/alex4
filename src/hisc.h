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

#ifndef HISC_H
#define HISC_H

#include "sdl_port.h"
#include "hisc.h"

// highscore list length
#define MAX_SCORES      5

// a hiscore post
typedef struct {
   char name[10];
   unsigned int score;
   unsigned int level;
} Thisc;

// functions
Thisc* make_hisc_table();
void destroy_hisc_table(Thisc *);
int qualify_hisc_table(Thisc *table, Thisc post);
void sort_hisc_table(Thisc *table);
void enter_hisc_table(Thisc *table, Thisc post);
void reset_hisc_table(Thisc *table, const char *name, int hi, int lo);
bool load_hisc_table(Thisc *table, const char *filename);
void save_hisc_table(Thisc *table, const char *filename);

void draw_hisc_post(Thisc *table, BITMAP *bmp, int x, int y, int color, int show_level);
void draw_hisc_table(Thisc *table, BITMAP *bmp, int x, int y, int color, int show_level);

#endif
