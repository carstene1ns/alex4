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
 
 
 


#include "bullet.h"
#include "particle.h"
#include "timer.h"
#include "../data/data.h"


// sets values on a bullet
void set_bullet(Tbullet *b, int x, int y, double dx, double dy, BITMAP *bmp, int bad) {
	b->x = x;
	b->y = y;
	b->dx = dx;
	b->dy = dy;
	b->exist = 160;
	b->bmp = bmp;
	b->bad = bad;
	b->animate = 0;
	b->bmp2 = NULL;
	b->gravity = 0;
}

// draws the bukllet on specified bitmap 
// pass along the map offset to get it right
void draw_bullet(BITMAP *bmp, Tbullet *b, int ox, int oy) {
	int x = b->x - ox;
	int y = b->y - oy;

	// is the bullet inside the screen
	if (x < -b->bmp->w || x > bmp->w + b->bmp->w || y < -b->bmp->h || y > bmp->h + b->bmp->h) return;

	// draw it
	if (!b->animate) {
		if (b->dx < 0)
			draw_sprite(bmp, b->bmp, x, y);
		else
			draw_sprite_h_flip(bmp, b->bmp, x, y);
	}
	else 
		draw_sprite(bmp, (logic_count & 4 ? b->bmp : b->bmp2), x, y);
}


// updates the bullet one tick
void update_bullet(Tbullet *b) {
	b->x += b->dx;
	b->y += b->dy;
	if (b->gravity) b->dy += 0.1;
	if (b->exist > 0) b->exist --;
}

// reset bullet array
void reset_bullets(Tbullet *b, int max) {
	int i;
	for(i = 0; i < max; i ++) {
		b[i].exist = 0;
	}
}


// returns an unused bullet from an array
Tbullet *get_free_bullet(Tbullet *b, int max) {
	int i = 0;
	while(b[i].exist && i<max) i++;
	if (i == max) return NULL;
	return &b[i];
}


// checks a bullet with the map
void update_bullet_with_map(Tbullet *b, Tmap *m) {
	update_bullet(b);
	
	if (is_ground(m, (int)b->x + b->bmp->w/2, (int)b->y + b->bmp->h/2)) {
		create_burst(particle, (int)b->x + b->bmp->w/2, (int)b->y + b->bmp->h/2, 4, 16, 0, -1);
		b->exist = 0;
	}
}

