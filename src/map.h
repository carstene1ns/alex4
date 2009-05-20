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
 
 
 
#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include "allegro.h"

// some directions for expanding/shrinking the map
#define SM_UP		0x01
#define SM_DOWN		0x02
#define SM_RIGHT	0x04
#define SM_LEFT		0x08

// the items
#define MAP_EGG		1
#define MAP_DEAD	2
#define MAP_EXIT	3
#define MAP_1UP		4
#define MAP_SPIN	5
#define MAP_BRK		6
#define MAP_STAR	7
#define MAP_CHERRY	8
#define MAP_HEART	9
#define MAP_LOLA	10

// the enemies ...
#define MAP_ENEMY1	101
#define MAP_ENEMY2	102
#define MAP_ENEMY3	103
#define MAP_ENEMY4	104
#define MAP_ENEMY5	105
#define MAP_ENEMY6	106
// ... and the bosses
#define MAP_GUARD1  151
#define MAP_GUARD2  152

// water tiles
#define MAP_WATER	201
#define MAP_SURFACE	202

// win conditions
#define MAP_WIN_EXIT			1
#define MAP_WIN_KILL_GUARDIAN	2
#define MAP_WIN_KILL_ALL		4

// a position on the tile map
typedef struct {
	unsigned char tile;			// #id in tile set
	unsigned char mask;			// #id in mask set
	unsigned char type;			// what type has this tile?
	unsigned char item;			// does this slot have an item?
} Tmappos;

// map header (overall stuff)
typedef struct {
	char name[32];
	char dummy[27];
	char map_data_needs_to_be_destroyed;
	char win_conditions_fullfilled;
	char num_enemies;
	char boss_level;
	char win_conditions;
	int width, height;
	Tmappos *dat;
	int offset_x;
	int offset_y;
	DATAFILE *data;
	int start_x;
	int start_y;
} Tmap;


// functions
Tmap *create_map(int w, int h);
void destroy_map(Tmap *m);
Tmap *load_map(char *fname);
Tmap *load_map_from_memory(void *mem);
int save_map(Tmap *m, char *fname);
void change_map_size(Tmap *m, int dw, int dh, int dir_flags);

void draw_map(BITMAP *bmp, Tmap *m, int dx, int dy, int w, int h, int edit);

Tmappos *get_mappos(Tmap *m, int tx, int ty);

int set_tile(Tmap *m, int tx, int ty, int tile);
int get_tile(Tmap *m, int tx, int ty);

int is_ground(Tmap *m , int x, int y);
int adjust_ypos (Tmap *m, int x, int y, int ground, int dy);
int adjust_xpos (Tmap *m, int x, int y, int ground, int dx);



#endif