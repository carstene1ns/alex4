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
 *    Written by Johan Peitz                                  *
 *    http://www.freelunchdesign.com                          *
 **************************************************************
 *    This source code is released under the The GNU          *
 *    General Public License (GPL). Please refer to the       *
 *    document license.txt in the source directory or         *
 *    http://www.gnu.org for license information.             *
 **************************************************************/
 
 
 
#ifndef _HISC_H_
#define _HISC_H_

#include "allegro.h"

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
void reset_hisc_table(Thisc *table, char *name, int hi, int lo);
int load_hisc_table(Thisc *table, PACKFILE *fp);
void save_hisc_table(Thisc *table, PACKFILE *fp);

void draw_hisc_post(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level);
void draw_hisc_table(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level);

#endif