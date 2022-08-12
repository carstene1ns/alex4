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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "port.h"
#include "map.h"

// maximum number of particles at anytime
#define MAX_PARTICLES	256

// the particle struct
typedef struct {
	double x, y;
	double dx, dy;
	int color;
	int life;
	int count;
	int bmp;
} Tparticle;

// the particles themselves
extern Tparticle particle[MAX_PARTICLES];

// functions
void reset_particles(Tparticle *p, int max);
Tparticle *get_free_particle(Tparticle *p, int max);
void set_particle(Tparticle *p, int x, int y, double dx, double dy, int color, int life, int bmp);
void draw_particle(BITMAP *bmp, Tparticle *p, int ox, int oy);
void update_particle(Tparticle *p);
void update_particle_with_map(Tparticle *p, Tmap *m);
void create_burst(Tparticle *ps, int x, int y, int spread, int num, int life, int bmp);

#endif
