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
 
 
 
#ifndef _SHOOTER_H_
#define _SHOOTER_H_


#include "control.h"


// space objects
#define SO_PLAYER			1
#define SO_POWERUP			2
#define SO_ENEMY			3
#define SO_PLAYER_BULLET	4
#define SO_ENEMY_BULLET		5
#define SO_EXPLOSION		6
#define SO_SIGN				7

// bullet flags
#define SF_NORMAL		0x01
#define SF_ION			0x02
#define SF_WAVE			0x04

// movement types
#define SM_STRAIGHT		1
#define SM_GO_DELAY		2
#define SM_SIN_WAVE		3
#define SM_COS_WAVE		4
#define SM_BOSS_1		5



// any kind of object
typedef struct {
	int type;
	int energy;
	int image;
	int flags;
	int alive;
	int hit;
	int fire_counter;
	int score;

	// movement
	int movement_type;
	double x, y;
	double vx, vy;
	int t;
	int d1, d2;
	int tx, ty;

	// bounding box specifics
	int bx, by, bw, bh;
} Tspace_object;


// collection of shooter variables
typedef struct {
	// level related
	int level_offset;
	int level_counter;
	int event_counter;
	int guardian_active;
	int difficulty;

	// player related
	long unsigned int score;
	long unsigned int show_score;
	int lives;
	int power_gauge;
	int power_level;

	int player_wait;

	// common
	Tcontrol *ctrl;

} Tshooter_data;


// a level event
typedef struct {
	int when;
	int enemy_type;
	int movement_type;
	int num;
	int x, y;
	double vx, vy;
	int d1, d2;		// variable data
} Tshooter_event;


// a star
typedef struct {
	int x, y, dx, color;
} Tspace_star;

// functions
int start_shooter(Tcontrol *c, int with_sound);


#endif