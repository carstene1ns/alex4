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
 
 
 


#include "particle.h"

// pointer to datafile
DATAFILE *data;


// set datafile to use
void set_datafile(DATAFILE *d) {
	data = d;
}


// inits variables in a particle
void set_particle(Tparticle *p, int x, int y, double dx, double dy, int color, int life, int bmp) {
	p->x = x;
	p->y = y;
	p->dx = dx;
	p->dy = dy;
	p->color = color;
	p->life = life;
	p->bmp = bmp;
	p->count = 0;
}

// draws particle depending on map offset (ox and oy)
void draw_particle(BITMAP *bmp, Tparticle*p, int ox, int oy) {
	int x = p->x - ox;
	int y = p->y - oy;

	// is the particle inside the screen
	if (x < -16 || x > bmp->w + 16 || y < -16 || y > bmp->h +16) return;

	// draw it
	if (p->bmp == -1)
		putpixel(bmp, x, y, p->color);
	else {
		BITMAP *b = data[p->bmp + (p->color == -1 ? p->count>>1 : 0)].dat;
		draw_sprite(bmp, b, x - b->w/2, y - b->h/2);
	}
}

// updates the particle data (position etc)
void update_particle(Tparticle* p) {
	p->x += p->dx;
	p->y += p->dy;

	if (p->bmp == -1) {
		p->dy += 0.1;
	}

	p->life --;
	p->count ++;
}

// resets all particles in an array
void reset_particles(Tparticle *p, int max) {
	int i;
	for(i = 0; i < max; i ++) {
		p[i].life = 0;
	}
}

// returns an unused particle from an array
Tparticle *get_free_particle(Tparticle *p, int max) {
	int i = 0;
	while(p[i].life && i<max) i++;
	if (i == max) return NULL;
	return &p[i];
}

// updates particle with map
void update_particle_with_map(Tparticle *p, Tmap *m) {
	update_particle(p);

	/* bouncing algo removed 
	p->y -= p->dy;
	p->x -= p->dx;
	if (is_ground(m, (int)p->x, (int)p->y)) {
		p->dy = - 0.9 * p->dy;
		//p->dx = - p->dx;
	}
	*/

}


// creates a little fireworks "bang"
void create_burst(Tparticle *ps, int x, int y, int spread, int num, int life, int bmp) {
	int i;
	Tparticle *p;
	for(i = 0; i < num; i ++) {
		p = get_free_particle(ps, MAX_PARTICLES);
		if (p != NULL) {
			set_particle(p, x + rand()%spread - spread/2, y + rand()%spread - spread/2, ((double)(rand()%100))/100.0 - 0.5, ((double)(rand()%100))/100.0 - 0.5, rand()%4 + 1, (life ? life : 70 - rand()%50), bmp);
		}
	}
}
