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
 
 
 
#ifndef _BULLET_H_
#define _BULLET_H_

#include "allegro.h"
#include "map.h"
#include "particle.h"

// the bullet struct
typedef struct {
	double x, y;
	double dx, dy;
	int exist;
	BITMAP *bmp, *bmp2;
	int w, h;
	int bad;
	int animate;
	int gravity;
} Tbullet;

// max bullets at any time
#define MAX_BULLETS	64

// the bullets themselves
Tbullet bullet[MAX_BULLETS];

// functions
void reset_bullets(Tbullet *b, int max);
Tbullet *get_free_bullet(Tbullet *b, int max);
void set_bullet(Tbullet *b, int x, int y, double dx, double dy, BITMAP *bmp, int bad);
void draw_bullet(BITMAP *bmp, Tbullet *b, int ox, int oy);
void update_bullet(Tbullet *b);
void update_bullet_with_map(Tbullet *b, Tmap *m);

#endif