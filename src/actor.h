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
 
 
 
#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "allegro.h"
#include "map.h"

// different modes that an actor can hold
// they are all exclusive
#define AC_NORM		1
#define AC_EAT		2
#define AC_FULL		3
#define AC_SPIT		4
#define AC_DEAD		5
#define AC_BALL		6
#define AC_FLAT		7

// different flags that show attributes
// of an actor
#define ACF_JUMPABLE		0x0001
#define ACF_FLATABLE		0x0002
#define ACF_HURTS			0x0004
#define ACF_SHOOTABLE		0x0008
#define ACF_BOUNCEABLE		0x0010
#define ACF_ROLLABLE		0x0020
#define ACF_ROLLABLE_BACK	0x0040

// no more than 64 actors can be on one map
#define MAX_ACTORS	64

// this is the actor struct
typedef struct {
	int active;
	int status;
	int type;

	int x, y, dy, dx;

	int w, h;
	int ox, oy;
	int tx;

	int direction;
	int frames[16];
	int flat_frame;
	int frame;
	int num_frames;
	int anim_counter;
	int anim_max;
	int hit_offset;

	int counter;

	unsigned int flags;
	int mode;
	int toggle;
	int ymax;
	int energy;
	int is_hit;
	int sound;

	DATAFILE *data;
} Tactor;


// functions
Tactor *make_actor(Tactor *a, int x, int y, DATAFILE *data);
void draw_actor(BITMAP *bmp, Tactor *a, int x, int y);
void animate_actor(Tactor *a);
void update_actor_with_map(Tactor *a, Tmap *m);
void kill_actor(Tactor *a);


#endif