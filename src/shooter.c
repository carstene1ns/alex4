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
 
 
 
#include <allegro.h>
#include <string.h>
#include <aldumb.h>

#include "main.h"
#include "hisc.h"
#include "timer.h"
#include "shooter.h"

#include "../data/a45.h"

// data file
DATAFILE *s_data = NULL;

// buffer
BITMAP *s_buffer = NULL;

// explosions frames
BITMAP *s_expl_frame[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// objects
Tspace_object s_player;
Tspace_object s_powerup[32];
Tspace_object s_enemy[32];
Tspace_object s_p_bullet[32];
Tspace_object s_e_bullet[32];
Tspace_object s_explosion[32];
Tspace_object s_sign;

// music
int s_got_sound = 0;
static AL_DUH_PLAYER *s_dp = NULL;
static DUH_SIGRENDERER *s_sr = NULL;
static DUH *s_duh = NULL;
float s_music_vol = 1.0;
int s_fade_music = 0;

// color map for blending explosions
COLOR_MAP s_blend_map;

// shooter data
Tshooter_data s_var;

// stars
Tspace_star s_star[64];

// the level
Tshooter_event s_level[] = {
//	  when	enemy		movement	  num	  x		 y	    vx	   vy     d1   d2
	{	1,	ENEMY03,	SM_SIN_WAVE,	5,	160,	20,		-0.5,  10,	  2,	24	},
	{	4,	ENEMY03,	SM_COS_WAVE,	5,	160,	75,		-0.5,  10,	  2,	24	},
	{	6,	ENEMY02,	SM_STRAIGHT,	1,	160,	10,		-0.5,   0,	  0,	0	},
	{	7,	ENEMY01,	SM_STRAIGHT,	1,	160,	75,		-0.5,   0,	  0,	0	},
	{	8,	ENEMY02,	SM_STRAIGHT,	1,	160,	40,		-0.5,   0,	  0,	0	},
	{  12,	ENEMY03,	SM_SIN_WAVE,	6,	160,	40,		-0.8,  10,	  4,	20	},
	{  14,	ENEMY07,	SM_STRAIGHT,	1,	160,	40,		-0.5,   0,	  0,	0	},
	{  16,	ENEMY02,	SM_STRAIGHT,	1,	160,	86,		-0.5,   0,	  0,	0	},
	{  16,	ENEMY05,	SM_STRAIGHT,	1,	202,	92,		-0.5,   0,	  0,	0	},
	{  17,	ENEMY06,	SM_STRAIGHT,	1,	180,	 0,		-0.5,   0,	  0,	0	},
	{  19,	ENEMY03,	SM_SIN_WAVE,	4,	160,	20,		-0.5,  10,	  2,	24	},
	{  19,	ENEMY03,	SM_SIN_WAVE,	4,	160,	75,		-0.5,  10,	  2,	24	},
	{  21,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  22,	ENEMY07,	SM_GO_DELAY,	1,	160,	80,		-1,	    0,	100,   100	},
	{  24,	ENEMY03,	SM_SIN_WAVE,	6,	160,	40,		-0.5,  25,	  4,	20	},
	{  27,	ENEMY01,	SM_STRAIGHT,	1,	160,	10,		-0.5,   0,	  0,	0	},
	{  28,	ENEMY02,	SM_STRAIGHT,	1,	160,	75,		-0.5,   0,	  0,	0	},
	{  29,	ENEMY01,	SM_STRAIGHT,	1,	160,	40,		-0.5,   0,	  0,	0	},
	{  30,	ENEMY02,	SM_STRAIGHT,	1,	160,	70,		-0.5,   0,	  0,	0	},
	{  31,	ENEMY02,	SM_STRAIGHT,	1,	160,	 5,		-0.5,   0,	  0,	0	},
	{  32,	ENEMY01,	SM_STRAIGHT,	1,	160,	45,		-0.5,   0,	  0,	0	},
	{  33,	ENEMY02,	SM_STRAIGHT,	1,	200,	75,		-0.5,   0,	  0,	0	},

	{  36,	ENEMY03,	SM_SIN_WAVE,	8,	160,	50,		-0.5,  40,	  1,	32	},
	{  36,	ENEMY03,	SM_SIN_WAVE,	8,	176,	50,		-0.5, -40,	  1,	32	},
	{  45,	ENEMY06,	SM_STRAIGHT,	1,	160,	 0,		-0.5,   0,	  0,	0	},
	{  45,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  47,	ENEMY03,	SM_SIN_WAVE,	8,	160,	50,		-0.5, -40,	  1,	32	},
	{  55,	ENEMY06,	SM_STRAIGHT,	1,	160,	 0,		-0.5,   0,	  0,	0	},
	{  55,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  56,	ENEMY06,	SM_STRAIGHT,	1,	160,	 0,		-0.5,   0,	  0,	0	},
	{  56,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  57,	ENEMY06,	SM_STRAIGHT,	1,	160,	 0,		-0.5,   0,	  0,	0	},
	{  57,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  62,	ENEMY07,	SM_GO_DELAY,	1,	160,	40,		-1,	    0,	 60,   100	},
	{  63,	ENEMY07,	SM_GO_DELAY,	1,	160,	60,		-1,	    0,	100,   100	},
	{  64,	ENEMY07,	SM_GO_DELAY,	1,	160,	20,		-1,	    0,	 80,   100	},
	{  66,	ENEMY14,	SM_SIN_WAVE,	1,	160,    40,		-0.5,  10,	  4,	0	},
	{  68,	ENEMY01,	SM_STRAIGHT,	1,	160,	50,		-0.5,   0,	  0,	0	},
	{  69,	ENEMY02,	SM_STRAIGHT,	1,	160,	10,		-0.5,   0,	  0,	0	},
	{  70,	ENEMY01,	SM_STRAIGHT,	1,	160,	80,		-0.5,   0,	  0,	0	},
	{  70,	ENEMY06,	SM_STRAIGHT,	1,	160,	 0,		-0.5,   0,	  0,	0	},
	{  71,	ENEMY02,	SM_STRAIGHT,	1,	160,	60,		-0.5,   0,	  0,	0	},
	{  72,	ENEMY02,	SM_STRAIGHT,	1,	160,    -5,		-0.5,   0,	  0,	0	},
	{  73,	ENEMY01,	SM_STRAIGHT,	1,	190,	50,		-0.5,   0,	  0,	0	},
	{  73,	ENEMY05,	SM_STRAIGHT,	1,	160,	92,		-0.5,   0,	  0,	0	},
	{  77,	ENEMY03,	SM_SIN_WAVE,	8,	160,	50,		-0.5,  30,	  2,	32	},
	{  77,	ENEMY03,	SM_SIN_WAVE,	8,	176,	50,		-0.5, -30,	  2,	32	},

	{  84,	ENEMY09,	SM_STRAIGHT,	1,	 60,	150,	 0,    -1,	  0,	0	},
	{  84,	ENEMY09,	SM_STRAIGHT,	1,	 80,	140,	 0,    -0.5,  0,	0	},
	{  84,	ENEMY09,	SM_STRAIGHT,	1,	100,	170,	 0,    -1,	  0,	0	},
	{  84,	ENEMY09,	SM_STRAIGHT,	1,	120,	130,	 0,    -1,	  0,	0	},
	{  84,	ENEMY09,	SM_STRAIGHT,	1,	 70,	120,	 0,    -1,	  0,	0	},
	{  84,	ENEMY09,	SM_STRAIGHT,	1,	110,	160,	 0,    -1,	  0,	0	},
	{  85,	ENEMY09,	SM_STRAIGHT,	1,	 65,	150,	 0,    -0.5,  0,	0	},
	{  85,	ENEMY09,	SM_STRAIGHT,	1,	 85,	170,	 0,    -1,	  0,	0	},
	{  85,	ENEMY09,	SM_STRAIGHT,	1,	105,	140,	 0,    -1,	  0,	0	},
	{  85,	ENEMY09,	SM_STRAIGHT,	1,	125,	120,	 0,    -1,	  0,	0	},
	{  85,	ENEMY09,	SM_STRAIGHT,	1,	 20,	160,	 0,    -0.5,  0,	0	},
	{  86,	ENEMY09,	SM_STRAIGHT,	1,	 75,	130,	 0,    -1,	  0,	0	},
	{  86,	ENEMY09,	SM_STRAIGHT,	1,	115,	160,	 0,    -1,	  0,	0	},

	{  88,	ENEMY07,	SM_GO_DELAY,	1,	160,	40,		-1,	    0,	 60,   100	},
	{  89,	ENEMY07,	SM_GO_DELAY,	1,	160,	60,		-1,	    0,	100,   100	},
	{  90,	ENEMY07,	SM_GO_DELAY,	1,	160,	20,		-1,	    0,	 80,   100	},
	{  92,	ENEMY07,	SM_GO_DELAY,	1,	160,	20,		-1,	    0,	 60,   100	},
	{  93,	ENEMY07,	SM_GO_DELAY,	1,	160,	60,		-1,	    0,	100,   100	},
	{  94,	ENEMY07,	SM_GO_DELAY,	1,	160,	40,		-1,	    0,	 80,   100	},
	{  95,	ENEMY07,	SM_GO_DELAY,	1,	160,	55,		-1,	    0,	 65,   100	},
	{  96,	ENEMY07,	SM_GO_DELAY,	1,	160,	45,		-1,	    0,	100,   100	},
	{  97,	ENEMY07,	SM_GO_DELAY,	1,	160,	25,		-1,	    0,	 80,   100	},
	{  98,	ENEMY07,	SM_GO_DELAY,	1,	160,	50,		-1,	    0,	 55,   100	},
	{  99,	ENEMY07,	SM_GO_DELAY,	1,	160,	15,		-1,	    0,	110,   100	},
	{ 100,	ENEMY07,	SM_GO_DELAY,	1,	160,	70,		-1,	    0,	 80,   100	},
	{ 101,	ENEMY04,	SM_GO_DELAY,	1,	160,	20,		-1,	    0,	100,   150	},

	{ 105,	ENEMY12,	SM_STRAIGHT,	1,	160,   -53,		-0.5,   0,	  0,	0	},
	{ 105,	ENEMY11,	SM_STRAIGHT,	1,	160,	73,		-0.5,   0,	  0,	0	},
	{ 105,	ENEMY07,	SM_GO_DELAY,	1,	200,	45,		-1,	    0,	 80,   100	},
	{ 107,	ENEMY12,	SM_STRAIGHT,	1,	162,   -20,		-0.5,   0,	  0,	0	},
	{ 109,	ENEMY11,	SM_STRAIGHT,	1,	170,    30,		-0.5,   0,	  0,	0	},
	{ 111,	ENEMY12,	SM_STRAIGHT,	1,	162,   -25,		-0.5,   0,	  0,	0	},
	{ 111,	ENEMY14,	SM_STRAIGHT,	1,	160,    80,		-0.5,   0,	  0,	0	},
	{ 113,	ENEMY11,	SM_STRAIGHT,	1,	160,    35,		-0.5,   0,	  0,	0	},
	{ 115,	ENEMY12,	SM_STRAIGHT,	1,	170,   -15,		-0.5,   0,	  0,	0	},
	{ 117,	ENEMY11,	SM_STRAIGHT,	1,	162,    25,		-0.5,   0,	  0,	0	},
	{ 117,	ENEMY14,	SM_STRAIGHT,	1,	160,     8,		-0.5,   0,	  0,	0	},
	{ 119,	ENEMY12,	SM_SIN_WAVE,	1,	160,   -60,		-0.5,   20,	  2,	0	},
	{ 119,	ENEMY11,	SM_SIN_WAVE,	1,	160,	80,		-0.5,   20,	  2,	0	},
	{ 121,	ENEMY12,	SM_SIN_WAVE,	1,	160,   -40,		-0.5,   20,	  1,	0	},
	{ 121,	ENEMY11,	SM_SIN_WAVE,	1,	160,   100,		-0.5,   20,	  1,	0	},
	{ 123,	ENEMY12,	SM_SIN_WAVE,	1,	160,   -80,		-0.5,   15,	  2,	0	},
	{ 123,	ENEMY11,	SM_SIN_WAVE,	1,	160,	60,		-0.5,   15,	  2,	0	},
	{ 125,	ENEMY12,	SM_STRAIGHT,	1,	160,   -53,		-0.5,   0,	  0,	0	},
	{ 125,	ENEMY11,	SM_STRAIGHT,	1,	160,	73,		-0.5,   0,	  0,	0	},

	{ 128,	ENEMY13,	SM_SIN_WAVE,	5,	160,	20,		-0.8,  10,	  2,	32	},
	{ 129,	ENEMY13,	SM_COS_WAVE,	5,	160,	75,		-0.5,  10,	  2,	32	},
	{ 131,	ENEMY07,	SM_GO_DELAY,	1,	200,	45,		-1,	    0,	 80,   100	},
	{ 133,	ENEMY13,	SM_COS_WAVE,	5,	160,	75,		-0.5,   5,	  3,	32	},
	{ 135,	ENEMY13,	SM_SIN_WAVE,	5,	160,	20,		-0.8,  15,	  1,	32	},

	{ 139,	ENEMY08,	SM_SIN_WAVE,	1,	160,	20,		-0.5,   5,	  1,	0	},
	{ 140,	ENEMY08,	SM_COS_WAVE,	1,	190,	60,		-0.5,   4,	  3,	0	},
	{ 140,	ENEMY08,	SM_SIN_WAVE,	1,	160,	40,		-0.5,   6,	  2,	0	},
	{ 142,	ENEMY08,	SM_COS_WAVE,	1,	160,	80,		-0.5,   3,	  4,	0	},
	{ 143,	ENEMY08,	SM_COS_WAVE,	1,	160,	70,		-0.5,   5,	  1,	0	},
	{ 143,	ENEMY08,	SM_COS_WAVE,	1,	190,	20,		-0.5,   2,	  4,	0	},
	{ 144,	ENEMY08,	SM_COS_WAVE,	1,	160,	60,		-0.5,   3,	  3,	0	},
	{ 144,	ENEMY08,	SM_SIN_WAVE,	1,	190,	30,		-0.5,   4,	  2,	0	},

	{ 146,	ENEMY03,	SM_SIN_WAVE,	4,	160,	20,		-0.5,  10,	  2,	24	},
	{ 146,	ENEMY03,	SM_SIN_WAVE,	4,	160,	75,		-0.5,  10,	  2,	24	},

	{ 149,	ENEMY08,	SM_SIN_WAVE,	1,	160,	60,		-0.5,   5,	  1,	0	},
	{ 150,	ENEMY08,	SM_COS_WAVE,	1,	190,	20,		-0.5,   4,	  3,	0	},
	{ 150,	ENEMY08,	SM_SIN_WAVE,	1,	160,	80,		-0.5,   6,	  2,	0	},
	{ 152,	ENEMY08,	SM_COS_WAVE,	1,	160,	40,		-0.5,   3,	  4,	0	},
	{ 153,	ENEMY08,	SM_COS_WAVE,	1,	160,	20,		-0.5,   5,	  1,	0	},
	{ 153,	ENEMY08,	SM_COS_WAVE,	1,	190,	70,		-0.5,   2,	  4,	0	},
	{ 154,	ENEMY08,	SM_COS_WAVE,	1,	160,	30,		-0.5,   3,	  3,	0	},
	{ 154,	ENEMY08,	SM_SIN_WAVE,	1,	190,	60,		-0.5,   4,	  2,	0	},

	{ 156,	ENEMY12,	SM_SIN_WAVE,	1,	160,   -50,		-0.5,   12,	  2,	0	},
	{ 156,	ENEMY11,	SM_SIN_WAVE,	1,	160,	70,		-0.5,  -12,	  2,	0	},
	{ 157,	ENEMY12,	SM_SIN_WAVE,	1,	190,   -40,		-0.5,   12,	  2,	0	},
	{ 157,	ENEMY11,	SM_SIN_WAVE,	1,	190,	80,		-0.5,  -12,	  2,	0	},
	{ 158,	ENEMY12,	SM_SIN_WAVE,	1,	210,   -60,		-0.5,   12,	  3,	0	},
	{ 158,	ENEMY11,	SM_SIN_WAVE,	1,	210,	60,		-0.5,  -12,	  3,	0	},

	{ 159,	ENEMY12,	SM_SIN_WAVE,   10,	190,   -60,		-0.5,   20,	  0,	24	},
	{ 159,	ENEMY11,	SM_SIN_WAVE,   10,	190,	80,		-0.5,   20,	  0,	24	},
	{ 165,	ENEMY12,	SM_SIN_WAVE,	1,	210,   -40,		-0.5,   12,	  3,	0	},
	{ 165,	ENEMY11,	SM_SIN_WAVE,	1,	210,	80,		-0.5,  -12,	  3,	0	},

	{ 170,	ENEMY09,	SM_STRAIGHT,	1,	 60,	150,	 0,    -1,	  0,	0	},
	{ 170,	ENEMY09,	SM_STRAIGHT,	1,	 80,	140,	 0,    -0.5,  0,	0	},
	{ 170,	ENEMY09,	SM_STRAIGHT,	1,	100,	170,	 0,    -1,	  0,	0	},
	{ 171,	ENEMY09,	SM_STRAIGHT,	1,	120,	130,	 0,    -1,	  0,	0	},
	{ 171,	ENEMY09,	SM_STRAIGHT,	1,	 70,	120,	 0,    -1,	  0,	0	},
	{ 171,	ENEMY09,	SM_STRAIGHT,	1,	110,	160,	 0,    -1,	  0,	0	},
	{ 172,	ENEMY09,	SM_STRAIGHT,	1,	 65,	150,	 0,    -0.5,  0,	0	},
	{ 172,	ENEMY09,	SM_STRAIGHT,	1,	 85,	170,	 0,    -1,	  0,	0	},
	{ 173,	ENEMY09,	SM_STRAIGHT,	1,	105,	140,	 0,    -1,	  0,	0	},
	{ 173,	ENEMY09,	SM_STRAIGHT,	1,	125,	120,	 0,    -1,	  0,	0	},
	{ 173,	ENEMY09,	SM_STRAIGHT,	1,	 20,	160,	 0,    -0.5,  0,	0	},
	{ 174,  ENEMY09,	SM_STRAIGHT,	1,	 75,	130,	 0,    -1,	  0,	0	},
	{ 174,	ENEMY09,	SM_STRAIGHT,	1,	115,	160,	 0,    -1,	  0,	0	},

	{ 176,	ENEMY07,	SM_GO_DELAY,	1,	160,	45,		-1,	    0,	100,   100	},
	{ 177,	ENEMY07,	SM_GO_DELAY,	1,	160,	25,		-1,	    0,	 80,   100	},
	{ 178,	ENEMY07,	SM_GO_DELAY,	1,	160,	50,		-1,	    0,	 55,   100	},
	{ 179,	ENEMY07,	SM_GO_DELAY,	1,	160,	15,		-1,	    0,	110,   100	},
	{ 180,	ENEMY07,	SM_GO_DELAY,	1,	160,	70,		-1,	    0,	 80,   100	},
	{ 181,	ENEMY07,	SM_GO_DELAY,	1,	160,	20,		-1,	    0,	100,    80	},
	{ 182,	ENEMY04,	SM_GO_DELAY,	1,	160,	25,		-1,	    0,	130,   150	},

	// boss 1
	{ 188,	ENEMY15,	SM_BOSS_1  ,	1,	160,	42,		-0.5,  20,	 90,	0	},
	{ 188,	ENEMY16,	SM_BOSS_1  ,	1,	173,	20,		-0.5,  20,	103,	0	},

	// restart
	{ 189,	-1,			0,				0,	  0,	 0,		 0,		0,	  0,	0	},
	{9999,	0,			0,				0,	  0,	 0,		 0,		0,	  0,	0	}
};



// PIXEL PERFECT COLLISION ROUTINES COURTESY OF IVAN BALDO'S PPCOL

#define pp_check_bb_collision_general(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
															((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))
#define pp_check_bb_collision(mask1,mask2,x1,y1,x2,y2) pp_check_bb_collision_general(x1,y1,mask1->w,mask1->h,x2,y2,mask2->w,mask2->h)

int s_check_pp_collision(BITMAP *spr1, BITMAP *spr2, int x1, int y1, int x2, int y2) {
	int dx1, dx2, dy1, dy2; //We will use this deltas...
	int fx,fy,sx1,sx2; //Also we will use this starting/final position variables...
	int maxw, maxh; //And also this variables saying what is the maximum width and height...
	int depth; //This will store the color depth value...
	char CHARVAR; //We will use these to store the transparent color for the sprites...
	
	if( !pp_check_bb_collision(spr1, spr2, x1,y1, x2,y2) ) return 0; //If theres not a bounding box collision, it is impossible to have a pixel perfect collision right? So, we return that theres not collision...
	
	//First we need to see how much we have to shift the coordinates of the sprites...
	if(x1>x2) {
		dx1=0;      //don't need to shift sprite 1.
		dx2=x1-x2;  //shift sprite 2 left. Why left? Because we have the sprite 1 being on the right of the sprite 2, so we have to move sprite 2 to the left to do the proper pixel perfect collision...
	} else {
		dx1=x2-x1;  //shift sprite 1 left.
		dx2=0;      //don't need to shift sprite 2.
	}
	if(y1>y2) {
		dy1=0;
		dy2=y1-y2;  //we need to move this many rows up sprite 2. Why up? Because we have sprite 1 being down of sprite 2, so we have to move sprite 2 up to do the proper pixel perfect collision detection...
	} else {
		dy1=y2-y1;  //we need to move this many rows up sprite 1.
		dy2=0;
	}
	
	//Then, we have to see how far we have to go, we do this seeing the minimum height and width between the 2 sprites depending in their positions:
	if(spr1->w-dx1 > spr2->w-dx2) {
		maxw=spr2->w-dx2;
	} else {
		maxw=spr1->w-dx1;
	}
	if(spr1->h-dy1 > spr2->h-dy2) {
		maxh=spr2->h-dy2;
	} else {
		maxh=spr1->h-dy1;
	}
	maxw--;
	maxh--;
	
	fy=dy1;
	fx=dx1;
	dy1+=maxh;
	dy2+=maxh;
	sx1=dx1+maxw;
	sx2=dx2+maxw;
	
	depth=bitmap_color_depth(spr1); //Get the bitmap depth...
	
	if(depth==8) {
		CHARVAR=bitmap_mask_color(spr1); //Get the transparent color of the sprites...
		for(; dy1>=fy; dy1--,dy2--) { //Go through lines...
			for(dx1=sx1,dx2=sx2; dx1>=fx; dx1--,dx2--) { //Go through the X axis...
				if((spr1->line[dy1][dx1]!=CHARVAR) && (spr2->line[dy2][dx2]!=CHARVAR)) return 1; //Both sprites don't have transparent color in that position, so, theres a collision and return collision detected!
			}
		}
	}
	
	//If we have reached here it means that theres not a collision:
	return 0; //Return no collision.
}

//////////// END PPCOL


// stops any mod playing
static void s_stop_music(void) {
	al_stop_duh(s_dp);
	s_dp = NULL;
}


// starts the mod at position x
static void s_start_music(int startorder) {
	const int n_channels = 2; /* stereo */

	s_stop_music();

	{
		s_music_vol = (float)(get_config_int("sound", "music_volume", 255)) / 255.0;
		s_sr = dumb_it_start_at_order(s_duh, n_channels, startorder);
		s_dp = al_duh_encapsulate_sigrenderer(s_sr, 
			s_music_vol,
			get_config_int("dumb", "buffer_size", 4096),
			get_config_int("dumb", "sound_freq", 44100));
		if (!s_dp) duh_end_sigrenderer(s_sr); // howto.txt doesn't mention that this is necessary! dumb.txt does ...
	}
}





// draws any object at specified position
void s_draw_object(BITMAP *bmp, Tspace_object *o) {
	if (o->type != SO_EXPLOSION) {
		if (!o->hit || o->type == SO_PLAYER_BULLET)
			draw_sprite(bmp, s_data[o->image].dat, (int)o->x, (int)o->y);
		else
			draw_character(bmp, s_data[o->image].dat, (int)o->x, (int)o->y, 4);
	}
	else {
		int c = (o->energy + 8) >> 3;
		if (c > 1) circle(bmp, (int)o->x + 15, (int)o->y + 15, o->image + 9, c);
		if (c > 2) circle(bmp, (int)o->x + 15, (int)o->y + 15, o->image + 8, c - 1);
		if (c > 3) circle(bmp, (int)o->x + 15, (int)o->y + 15, o->image + 7, c - 2);
		draw_trans_sprite(bmp, s_expl_frame[o->image], (int)o->x, (int)o->y);
	}

}


// draws the status bar
void s_draw_status_bar(BITMAP *bmp, int x, int y) {
	int i;
	char score_str[32];
	char padding_str[32];
	int padding;

	// draw bar
	blit(s_data[STATUSBAR].dat, bmp, 0, 0, 0, 109, 160, 11);

	// draw lives
	for(i = 0; i < s_var.lives; i ++) 
		draw_sprite(bmp, s_data[ST_LIFE].dat, x + 3 + i * 8, y + 3); 
	
	// draw stars
	for(i = 0; i < 5; i ++)
		draw_sprite(bmp, s_data[(i < s_var.power_gauge ? ST_STAR_ON : ST_STAR_OFF)].dat, x + 36 + i * 8, y + 3); 
	
	// draw score
	sprintf(score_str, "%d", s_var.show_score);
	padding = 11 - strlen(score_str);
	for(i = 0; i < padding; i ++)
		padding_str[i] = '0';
	padding_str[i] = '\0';
	strcat(padding_str, score_str);
	textprintf_right(bmp, s_data[SPACE_FONT].dat, x + 160, y + 2, 3, "%s", padding_str);
	if (s_var.score) textprintf_right(bmp, s_data[SPACE_FONT].dat, x + 160, y + 2, 4, "%s", score_str);
}


void s_draw_frame(BITMAP *bmp) {
	int i;

	// blit bg
	blit(s_data[BG1].dat, bmp, 0, 0, -(game_count % 160), 0, 160, 120);
	blit(s_data[BG1].dat, bmp, 0, 0, 160 - (game_count % 160), 0, 160, 120);

	// draw stars
	for(i = 0; i < 64; i ++)
		_putpixel(bmp, s_star[i].x, s_star[i].y, s_star[i].color);
	

	// draw powerups
	for(i = 0; i < 32; i ++) if (s_powerup[i].alive) s_draw_object(bmp, &s_powerup[i]);

	// draw enemies
	for(i = 0; i < 32; i ++) if (s_enemy[i].alive) s_draw_object(bmp, &s_enemy[i]);

	// draw player
	if (s_player.alive)	s_draw_object(bmp, &s_player);

	// draw bullets
	for(i = 0; i < 32; i ++) if (s_p_bullet[i].alive) s_draw_object(bmp, &s_p_bullet[i]);
	for(i = 0; i < 32; i ++) if (s_e_bullet[i].alive) s_draw_object(bmp, &s_e_bullet[i]);

	// draw sign
	if (s_sign.alive) draw_sprite(bmp, (s_sign.d1 ? get_gameover_sign() : get_letsgo_sign()), (int)s_sign.x, (int)s_sign.y);

	// draw explosions
	color_map = &s_blend_map;
	drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
	for(i = 0; i < 32; i ++) if (s_explosion[i].alive) s_draw_object(bmp, &s_explosion[i]);
	solid_mode();

	// draw status bar
	s_draw_status_bar(bmp, 0, 109);

}


// creates a new random star
void s_make_random_star(Tspace_star *s) {
	s->x = rand()%160;
	s->y = rand()%120;
	s->color = rand()%2 + 2;
	s->dx = (s->color == 2 ? - 2 : -3); 
}


// inits the player
void s_setup_player() {
	s_player.x = -30;
	s_player.y = 30;
	s_player.vx = 0;
	s_player.vy = 0;
	s_player.alive = 1;
}


// returns a free object from an array
Tspace_object *s_get_free_object(Tspace_object *array, int max) {
	int i = 0;
	while(array[i].alive && i < max) i ++;
	if (i == max) return NULL;

	// reset some values
	array[i].hit = 0;
	array[i].fire_counter = -1;
	array[i].movement_type = 0;

	return &array[i];
}


// creates an enemy bullet
Tspace_object *s_make_enemy_bullet(int x, int y, double vx, double vy) {
	Tspace_object *bullet;
	bullet = s_get_free_object(s_e_bullet, 32);
	if (bullet != NULL) {
		bullet->type = SO_ENEMY_BULLET;
		bullet->alive = 1;
		bullet->vx = vx;
		bullet->vy = vy;
		bullet->image = BULLET04;
		bullet->flags = SF_NORMAL;
		bullet->energy = 1;
		bullet->bx = bullet->by = 2;
		bullet->bw = ((BITMAP *)s_data[bullet->image].dat)->w - 5;
		bullet->bh = ((BITMAP *)s_data[bullet->image].dat)->h - 5;
		bullet->x = x - bullet->bw/2;
		bullet->y = y - bullet->bh/2;
	}

	return bullet;
}

// updates an object
void s_update_object(Tspace_object *o) {

	// first, update movement and position
	switch(o->movement_type) {
		case SM_STRAIGHT:
			o->x += o->vx;
			o->y += o->vy;
			break;
		case SM_GO_DELAY:
			if (o->x > o->d1 || o->d2 == 0) {
				o->x += o->vx;
				o->y += o->vy;
			}
			else {
				o->d2 --;
			}
			break;
		case SM_SIN_WAVE:
			o->x += o->vx;
			o->y = o->d2 + fixtoi(fmul(ftofix(o->vy), fsin(itofix(o->t))));
			o->t += o->d1;
			break;
		case SM_COS_WAVE:
			o->x += o->vx;
			o->y = o->d2 + fixtoi(fmul(ftofix(o->vy), fcos(itofix(o->t))));
			o->t += o->d1;
			break;
		case SM_BOSS_1:
			if (o->x > o->d1) {
				o->x += o->vx;
				o->t = 0;
				o->ty = o->y;
				o->tx = o->x;
			}
			else {
				o->y = o->ty + fixtoi(fmul(ftofix(o->vy), fsin(itofix(o->t))));
				o->x = o->tx - fixtoi(10 * fsin(itofix(o->t >> 1)));
				o->t ++;
			}
			break;
		default:
			o->x += o->vx;
			o->y += o->vy;
	}

	// check boundaries, has the object left the screen?
	if (o->x > 160 && o->vx > 0) o->alive = 0;
	else if (o->y > 120 && o->vy > 0) o->alive = 0;
	else if (o->x < -40 && o->vx < 0) o->alive = 0;
	else if (o->y < -40 && o->vy < 0) o->alive = 0;

	if (o->alive == 0 && o->movement_type == SM_BOSS_1) {
		s_var.guardian_active --;
	}

	// is the object hit by player?
	if (o->hit) {
		o->hit = 0;
		if (o->type == SO_PLAYER_BULLET) {
			o->alive = 0;
		}
	}

	// is the object firing?
	if (o->fire_counter > 0) {
		o->fire_counter --;
	}
	else if (o->fire_counter == 0) {
		o->fire_counter = 100 + rand()%50;

		if (o->image == ENEMY07) {
			s_make_enemy_bullet((int)o->x, (int)o->y + o->bh/2 + 2, -1.8, 0);
		}
		else if (o->image == ENEMY04) {
			s_make_enemy_bullet((int)o->x, (int)o->y + o->bh/2, -1.5, -0.3);
			s_make_enemy_bullet((int)o->x, (int)o->y + o->bh/2 + 2, -1.8, 0);
			s_make_enemy_bullet((int)o->x, (int)o->y + o->bh/2 + 4, -1.5, 0.3);
		}
		else if (o->image == ENEMY05) {
			s_make_enemy_bullet((int)o->x + o->bw/2, (int)o->y, -0.5, -1.8);
		}
		else if (o->image == ENEMY06) {
			s_make_enemy_bullet((int)o->x + o->bw/2, (int)o->y + o->bh, -0.5, 1.8);
		}
		else if (o->image == ENEMY16) {
			s_make_enemy_bullet((int)o->x - 3, (int)o->y + o->bh - 16, -0.9,-0.5);
			s_make_enemy_bullet((int)o->x - 3, (int)o->y + o->bh - 16, -1.2, 0.0);
			s_make_enemy_bullet((int)o->x - 3, (int)o->y + o->bh - 16, -0.9, 0.5);
		}
		else if (o->image == ENEMY15) {
			int i;
			for(i = 0; i < 256; i += 32)
				s_make_enemy_bullet((int)o->x + o->bw / 2, (int)o->y + o->bh, fixtof(1 * fcos(itofix(i - 6))), fixtof(1 * fsin(itofix(i - 6))));
			o->fire_counter = 200 + rand()%50;
		}
	
	}

	// update explosions
	if (o->type == SO_EXPLOSION) {
		o->image ++;
		o->energy --;
		if (o->energy <= 0) o->alive = 0;
	}

}

// creates a powerup
Tspace_object *s_make_powerup(int x, int y) {
	Tspace_object *o;
	o = s_get_free_object(s_powerup, 32);
	if (o == NULL) return NULL;

	o->type = SO_POWERUP;
	o->alive = 1;
	o->vx = -0.5;
	o->vy = 0;
	o->x = x - 8;
	o->y = y - 8;
	o->image = POWERUP;

	// setup basic bb
	o->bx = o->by = 3;
	o->bw = ((BITMAP *)s_data[o->image].dat)->w - 7;
	o->bh = ((BITMAP *)s_data[o->image].dat)->h - 7;

	return o;
}

// creates an explosion
Tspace_object *s_make_explosion(int x, int y) {
	Tspace_object *o;
	o = s_get_free_object(s_explosion, 32);
	if (o == NULL) return NULL;

	o->type = SO_EXPLOSION;
	o->alive = 1;
	o->vx = -0.5;
	o->vy = 0;
	o->x = x - 16;
	o->y = y - 16;
	o->image = 0;
	o->energy = 32;

	// setup basic bb
	o->bx = o->by = 0;
	o->bw = ((BITMAP *)s_data[o->image].dat)->w - 1;
	o->bh = ((BITMAP *)s_data[o->image].dat)->h - 1;

	return o;
}



// creates an enemy
Tspace_object *s_make_enemy(int type, int y) {
	Tspace_object *o;
	o = s_get_free_object(s_enemy, 32);
	if (o == NULL) return NULL;

	o->type = SO_ENEMY;
	o->alive = 1;
	o->vx = -0.5;
	o->vy = 0;
	o->x = 160;
	o->image = type;
	o->fire_counter = -1;

	// setup basic bb
	o->bx = o->by = 0;
	o->bw = ((BITMAP *)s_data[o->image].dat)->w - 1;
	o->bh = ((BITMAP *)s_data[o->image].dat)->h - 1;

	// set special stuff
	// depending on enemy type
	switch(type) {
		case ENEMY01:		// small asteroid
			o->energy = 20;
			o->flags = SF_WAVE;
			break;
		case ENEMY02:		// big asteroid
			o->energy = 30;
			o->flags = SF_WAVE;
			break;
		case ENEMY03:		// squid
			o->energy = 3;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			break;
		case ENEMY04:		// space thug
			o->energy = 16;
			o->flags = SF_ION | SF_WAVE;
			o->fire_counter = 20;
			o->bx += 5; o->bw -= 5;
			break;
		case ENEMY05:		// up shooter
			o->energy = 6;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->fire_counter = 20;
			break;
		case ENEMY06:		// down shooter
			o->energy = 6;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->fire_counter = 20;
			break;
		case ENEMY07:		// single shooter
			o->energy = 10;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->fire_counter = 20;
			o->bx += 5; o->bw -= 5;
			break;
		case ENEMY08:		// mine
			o->energy = 12;
			o->flags = SF_ION | SF_WAVE;
			o->bx += 3; o->bw -= 6;
			o->by += 3; o->bh -= 6;
			break;
		case ENEMY09:		// up rocket (^)
			o->energy = 4;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->bx += 2; o->bw -= 4;
			o->by += 2; o->bh -= 4;
			break;
		case ENEMY10:		// down rocket (v)
			o->energy = 4;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->bx += 2; o->bw -= 4;
			o->by += 2; o->bh -= 4;
			break;
		case ENEMY11:		// up pillar (^)
			o->energy = 4;
			o->flags = 0;
			break;
		case ENEMY12:		// down pillar (v)
			o->energy = 4;
			o->flags = 0;
			break;
		case ENEMY13:		// helmet squid
			o->energy = 8;
			o->flags = SF_ION | SF_WAVE;
			break;
		case ENEMY14:		// star container
			o->energy = 4;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			break;
		case ENEMY15:		// saucer, lower part
			o->energy = 100;
			o->flags = SF_NORMAL | SF_ION | SF_WAVE;
			o->fire_counter = 40;
			break;
		case ENEMY16:		// saucer, upper part
			o->energy = 80;
			o->flags = SF_ION | SF_WAVE;
			o->fire_counter = 20;
			break;
		default:
			break;
	}

	// set common stuff like energy etc
	o->energy = o->energy * s_var.difficulty;
	o->score = o->energy * o->energy * 1000;
	o->y = y;


	return o;
}


// creates a wave of an event
void s_make_enemy_wave(Tshooter_event *e) {
	int i;
	Tspace_object *o;

	for(i = 0; i < e->num; i ++) {
		o = s_get_free_object(s_enemy, 32);
		if (o != NULL) {
			s_make_enemy(e->enemy_type, e->y);
			o->movement_type = e->movement_type;
			switch(o->movement_type) {
				case SM_STRAIGHT:
					o->x = e->x + i * e->d2;
					o->vx = e->vx;
					o->vy = e->vy;
					break;
				case SM_GO_DELAY:
					o->x = e->x + i * e->d2;
					o->vx = e->vx;
					o->vy = e->vy;
					o->d1 = e->d1;
					o->d2 = e->d2;
					break;
				case SM_SIN_WAVE:
					o->x = e->x + i * e->d2;
					o->y = e->y;
					o->vx = e->vx;
					o->vy = e->vy;
					o->t = 2 * i * ((BITMAP *)s_data[o->image].dat)->w;
					o->d2 = e->y;
					o->d1 = e->d1;
					break;
				case SM_COS_WAVE:
					o->x = e->x + i * e->d2;
					o->y = e->y;
					o->vx = e->vx;
					o->vy = e->vy;
					o->t = 2 * i * ((BITMAP *)s_data[o->image].dat)->w;
					o->d2 = e->y;
					o->d1 = e->d1;
					break;
				case SM_BOSS_1:
					o->x = e->x;
					o->vx = e->vx;
					o->vy = e->vy;
					o->d1 = e->d1;
					o->d2 = e->d2;
					s_var.guardian_active ++;
					break;
			}
		}
	}
}


// kills the player
void s_kill_player() {
	s_player.alive = 0;
	s_make_explosion((int)s_player.x + s_player.bx + s_player.bw/2, 
		(int)s_player.y + s_player.by + s_player.bh/2);
	s_var.player_wait = 100;
	// play sound
	play_sound_id_ex(SMPL_SHOOT, 100, 600, 0);
	//play_sound_id(SMPL_A_DIE);
}


// collision checking (and acting upon collision)
// projectile goes in b, object in a
void s_check_collision(Tspace_object *array_a, int num_a, Tspace_object *array_b, int num_b) {
	int a, b;

	for(a = 0; a < num_a; a ++) {
		if (array_a[a].alive) {
			for(b = 0; b < num_b; b ++) {
				if (array_b[b].alive && !array_b[b].hit) {
					// do bb collision 
					if (s_check_pp_collision(s_data[array_a[a].image].dat, s_data[array_b[b].image].dat, 
						(int)array_a[a].x, (int)array_a[a].y,
						(int)array_b[b].x, (int)array_b[b].y)) {

						// handle all collision types
						if (array_a[a].type == SO_ENEMY && array_b[b].type == SO_PLAYER_BULLET) {
							// change bullet into hit image
							array_b[b].image = BULLET_HIT;
							array_b[b].hit = 1;
							array_b[b].y += - 5 + array_b[b].bh/2;
							array_b[b].x += 3;

							// check if enemy is harmed by bullet 
							if (array_a[a].flags & array_b[b].flags) {
								// decrese enemy energy
								array_a[a].energy -= array_b[b].energy;
								array_a[a].hit = 1;
								
								// kill if needed
								if (array_a[a].energy <= 0) {
									int cx = (int)array_a[a].x + array_a[a].bx + array_a[a].bw/2;
									int cy = (int)array_a[a].y + array_a[a].by + array_a[a].bh/2;
									array_a[a].alive = 0;

									if (array_a[a].movement_type == SM_BOSS_1) {
										s_var.guardian_active --;
										play_sound_id(SMPL_E_DIE);
									}

									// play sound
									play_sound_id_ex(SMPL_SHOOT, 100, 600, 0);

									// place explosion
									s_make_explosion(cx, cy);

									// generate power up
									if (rand()%100 > 85 || array_a[a].image == ENEMY14)
										s_make_powerup(cx, cy);

									// increase player score
									s_var.score += array_a[a].score;

									// special case?
									if (array_a[a].image == ENEMY08) { // mine
										double vx = array_a[a].vx; 
										// spread bullets!
										s_make_enemy_bullet(cx, cy, vx-1, -0.5);
										s_make_enemy_bullet(cx, cy, vx-0.5,  1);
										s_make_enemy_bullet(cx, cy, vx+0.5, -1);
										s_make_enemy_bullet(cx, cy, vx+1,  0.5);
									}
								}
							}
							else {
								play_sound_id(SMPL_HIT);
							}
						}

						else if (array_a[a].type == SO_PLAYER && array_b[b].type == SO_ENEMY) {
							// kill player
							s_kill_player();
						}
						
						else if (array_a[a].type == SO_PLAYER && array_b[b].type == SO_ENEMY_BULLET) {
							// kill player
							s_kill_player();
						}
						
						else if (array_a[a].type == SO_PLAYER && array_b[b].type == SO_POWERUP) {
							// upgrade player
							s_var.power_gauge ++;
							if (s_var.power_gauge >= 6) {
								s_var.power_gauge = 0;
								if (s_var.power_level == 7) { // allready full
									s_var.score += 1000000;
								}
								else {  // increase power
									s_var.power_level = MIN(s_var.power_level ++, 7);
								}
								play_sound_id(SMPL_HEART);	

							}
							else 
								play_sound_id(SMPL_STAR);

							// remove power up
							array_b[b].alive = 0;
						}
					}
				}
			}
		}
	}
}


// creates a player bullet
Tspace_object *s_make_player_bullet(int type) {
	Tspace_object *bullet;
	bullet = s_get_free_object(s_p_bullet, 32);
	if (bullet != NULL) {
		bullet->alive = 1;
		bullet->type = SO_PLAYER_BULLET;
		switch(type) {
		case BULLET01:
			bullet->image = BULLET01;
			bullet->flags = SF_NORMAL;
			bullet->vx = 3;
			bullet->vy = 0;
			bullet->energy = 1;
			break;
		case BULLET05:
			bullet->image = BULLET05;
			bullet->flags = SF_NORMAL;
			bullet->vx = 2;
			bullet->vy = -1.5;
			bullet->energy = 1;
			break;
		case BULLET06:
			bullet->image = BULLET06;
			bullet->flags = SF_NORMAL;
			bullet->vx = 2;
			bullet->vy = 1.5;
			bullet->energy = 1;
			break;
		case BULLET02:
			bullet->image = BULLET02;
			bullet->flags = SF_ION;
			bullet->vx = 3;
			bullet->vy = 0;
			bullet->energy = 2;
			break;
		case BULLET07:
			bullet->image = BULLET07;
			bullet->flags = SF_ION;
			bullet->vx = 2;
			bullet->vy = -2;
			bullet->energy = 2;
			break;
		case BULLET08:
			bullet->image = BULLET08;
			bullet->flags = SF_ION;
			bullet->vx = 2;
			bullet->vy = 2;
			bullet->energy = 2;
			break;
		case BULLET03:
			bullet->image = BULLET03;
			bullet->flags = SF_WAVE;
			bullet->energy = 3;
			bullet->vx = 3;
			bullet->vy = 0;
			break;
		}
		bullet->bx = bullet->by = 0;
		bullet->bw = ((BITMAP *)s_data[bullet->image].dat)->w - 1;
		bullet->bh = ((BITMAP *)s_data[bullet->image].dat)->h - 1;
		bullet->x = s_player.x + 6;
		bullet->y = s_player.y + 20 - bullet->bh/2;
		
	}
	return bullet;
}


// updats the player object
void s_update_player() {
	static int shoot_delay = 0;
	int bx1 = 0, by1 = -6, bx2 = 130, by2 = 85;

	// handle input
	poll_control(s_var.ctrl);
	if (is_up(s_var.ctrl) && s_player.y > by1) s_player.vy = -1;
	if (is_down(s_var.ctrl) && s_player.y < by2) s_player.vy = 1;
	if (is_left(s_var.ctrl) && s_player.x > bx1) s_player.vx = -1;
	if (is_right(s_var.ctrl) && s_player.x < bx2) s_player.vx = 1;
	
	// fire bullets
	if ((is_fire(s_var.ctrl) || is_jump(s_var.ctrl)) && shoot_delay == 0) {
		switch(s_var.power_level) {
			case 1:
				s_make_player_bullet(BULLET01);	
				break;
			case 2:
				s_make_player_bullet(BULLET02);	
				break;
			case 3:
				s_make_player_bullet(BULLET02);	
				s_make_player_bullet(BULLET06);	
				break;
			case 4:
				s_make_player_bullet(BULLET02);	
				s_make_player_bullet(BULLET05);	
				s_make_player_bullet(BULLET06);	
				break;
			case 5:
				s_make_player_bullet(BULLET03);	
				s_make_player_bullet(BULLET05);	
				s_make_player_bullet(BULLET06);	
				break;
			case 6:
				s_make_player_bullet(BULLET03);	
				s_make_player_bullet(BULLET05);	
				s_make_player_bullet(BULLET08);	
				break;
			default: // 7 +
				s_make_player_bullet(BULLET03);	
				s_make_player_bullet(BULLET07);	
				s_make_player_bullet(BULLET08);	
				break;
		}

		shoot_delay = 20;

		// play sound
		play_sound_id(SMPL_SPIT);
	}

	if (shoot_delay) shoot_delay --;
	if (!is_fire(s_var.ctrl) && !is_jump(s_var.ctrl)) shoot_delay = 0;

	// update position
	s_player.x += s_player.vx;
	s_player.y += s_player.vy;

	// check bounding box
	if (s_player.x < bx1) { s_player.vx = 1; }
	if (s_player.x > bx2) { s_player.x = bx2; s_player.vx = 0; };
	if (s_player.y < by1) { s_player.y = by1; s_player.vy = 0; }
	if (s_player.y > by2) { s_player.y = by2; s_player.vy = 0; }

	// update velocities
	s_player.vx *= 0.8;
	s_player.vy *= 0.8;
}


// resets AL objects
void s_reset_all_objects() {
	int i;

	for(i = 0; i < 32; i ++) {
		s_powerup[i].alive = 0;
		s_p_bullet[i].alive = 0;
		s_e_bullet[i].alive = 0;
		s_enemy[i].alive = 0;
		s_explosion[i].alive = 0;
	}

	// reset guardian counter
	s_var.guardian_active = 0;
}

// shows a sign (game over, lets go)
void s_activate_sign(int game_over, double vy) {
	BITMAP *b;

	s_sign.alive = 1;
	s_sign.type = SO_SIGN;
	s_sign.d1 = game_over;
	b = (s_sign.d1 ? get_gameover_sign() : get_letsgo_sign());
	s_sign.x = 80 - b->w/2;
	s_sign.y = 120;
	s_sign.vx = 0;
	s_sign.vy = vy;
}


// the player can enter his/hers name
void s_get_player_name(char *name) {
	clear_to_color(s_buffer, 1);
	blit(s_data[BG1].dat, s_buffer, 0, 0, 0, 0, 160, 120);
	textout_outline_center(s_buffer, "Congratulations,", 80, 8);
	textout_outline_center(s_buffer, "You've got", 80, 19);
	textout_outline_center(s_buffer, "a high score!", 80, 30);
	textout_outline_center(s_buffer, "Enter your name:", 80, 55);
	blit_to_screen(s_buffer);
	fade_in_pal_black(100, s_dp);
	get_string(s_buffer, name, 10, s_data[SPACE_FONT].dat, 50, 80, 4, s_var.ctrl);
}

// runs the shooter
void s_run_shooter() {
	int i;
	int playing = 1;
	int quit = 0;

	// setup player
	s_setup_player();

	// start music
	s_start_music(0);
	s_fade_music = 0;

	// int gfx
	s_draw_frame(s_buffer);
	blit_to_screen(s_buffer);
	fade_in_pal(100);


	// Go!
	cycle_count = 0;
	game_count = 0;
	s_activate_sign(0, -1);
	while(playing || s_sign.alive) {
		
		//  do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (s_got_sound) {
				if (s_fade_music) {
					s_music_vol = s_music_vol * 0.98;
					al_duh_set_volume(s_dp, s_music_vol);
				}
				al_poll_duh(s_dp);
			}

			// move stars
			for(i = 0; i < 64; i ++) {
				s_star[i].x += s_star[i].dx;
				if (s_star[i].x < 0) {
					s_make_random_star(&s_star[i]);
					s_star[i].x = 159;
				}
			}

			// update player
			if (s_player.alive)	s_update_player();

			// update enemies
			for(i = 0; i < 32; i ++) if (s_enemy[i].alive) s_update_object(&s_enemy[i]);

			// update player bullets
			for(i = 0; i < 32; i ++) if (s_p_bullet[i].alive) s_update_object(&s_p_bullet[i]);

			// update enemy bullets
			for(i = 0; i < 32; i ++) if (s_e_bullet[i].alive) s_update_object(&s_e_bullet[i]);

			// update powerups
			for(i = 0; i < 32; i ++) if (s_powerup[i].alive) s_update_object(&s_powerup[i]);

			// update explosions
			for(i = 0; i < 32; i ++) if (s_explosion[i].alive) s_update_object(&s_explosion[i]);

			// update sign
			if (s_sign.alive) s_update_object(&s_sign);

			// check collisions
			s_check_collision(s_enemy, 32, s_p_bullet, 32);
			s_check_collision(&s_player, 1, s_powerup, 32);
			if (!key[KEY_SPACE]) s_check_collision(&s_player, 1, s_e_bullet, 32);
			s_check_collision(&s_player, 1, s_enemy, 32);

			// update level
			if (!s_var.guardian_active)
				s_var.level_counter ++;

			if (s_var.level_counter >= 80) {
				s_var.level_counter  = 0;
				while(s_level[s_var.event_counter].when == s_var.level_offset) {
					if (s_level[s_var.event_counter].enemy_type < 0) {  // restart level
						s_var.event_counter = 0;
						s_var.level_offset = 0;
						s_var.difficulty ++;
					}
					else { // launch wave and continue
						s_make_enemy_wave(&s_level[s_var.event_counter]);
						s_var.event_counter ++;
					}
				}
				s_var.level_offset ++;
			}

			// misc stuff
			if (key[KEY_ESC]) {
				if (s_got_sound) al_pause_duh(s_dp);
				if (do_pause_menu(s_buffer) < 0) {
					playing = 0;
					quit = 1;
					s_sign.alive = 0;
				}
				if (s_got_sound) al_resume_duh(s_dp);
				cycle_count = 0;
			}
			if (key[KEY_F12]) {
				//al_pause_duh(dp);
				take_screenshot(s_buffer);
				while(key[KEY_F12]);
				cycle_count = 0;
				//al_resume_duh(dp);
			}
		
			// update player score
			if (s_var.score - s_var.show_score > 100000) s_var.show_score += 100000;
			else if (s_var.score - s_var.show_score > 25000) s_var.show_score += 25000;
			else if (s_var.score - s_var.show_score > 10000) s_var.show_score += 10000;
			else if (s_var.score - s_var.show_score > 5000) s_var.show_score += 5000;
			else if (s_var.show_score < s_var.score) s_var.show_score += 1000;

			// update player wait
			if (s_var.player_wait == 50) {
				s_var.lives --;
				s_var.power_gauge = 0;
				if (s_var.lives == 0) {
					s_activate_sign(1, -1);
					// start music fade
					s_fade_music = 1;

					// game over!
					playing = 0;
					s_var.player_wait = 999999;  // never come back!
				}
				else {
					// fade out
					fade_out_pal_black(100, s_dp);
					s_var.show_score = s_var.score;
					// remove all objects
					s_reset_all_objects();
					// rewind level counter
					s_var.level_counter = -40;
					s_var.level_offset = MAX(s_var.level_offset - 8, 0);
					while(s_level[s_var.event_counter].when >= s_var.level_offset && s_var.event_counter > 0) s_var.event_counter --;
					if (s_level[s_var.event_counter].when < s_var.level_offset) s_var.event_counter ++;
					s_var.level_offset = s_level[s_var.event_counter].when;

					// fade back
					s_draw_frame(s_buffer);
					blit_to_screen(s_buffer);
					fade_in_pal_black(100, s_dp);
					s_activate_sign(0, -1);
				}
			}
			if (s_var.player_wait == 1) s_setup_player();	
			if (s_var.player_wait) s_var.player_wait --;
			
			cycle_count --;
		}
		
		
		// let other processes play
		yield_timeslice();
		
		// draw 
		frame_count ++;
		s_draw_frame(s_buffer);
		blit_to_screen(s_buffer);
	}
	
	s_stop_music();

	// fade out
	fade_out_pal_black(100, s_dp);
	if (!quit) {
		Thisc post;

		// try for high score
		post.level = 0;
		post.name[0] = '\0';
		post.score = s_var.score;
		if (qualify_hisc_table(get_space_hisc(), post)) {
			s_get_player_name(post.name);
			enter_hisc_table(get_space_hisc(), post);
			sort_hisc_table(get_space_hisc());
			show_scores(1, get_space_hisc());
		}
	}

	// restore palette
	clear(screen);
	fade_in_pal_black(1, s_dp);
}

// simple blending (used w/ explosions)
void s_blend_frame(BITMAP *frame) {
	int x, y;
	int c;

	for(y = 1; y < frame->h - 1; y ++) {
		for(x = 1; x < frame->w - 1; x ++) {
			c = _getpixel(frame, x - 1, y - 1);
			c += _getpixel(frame, x, y - 1);
			c += _getpixel(frame, x + 1, y - 1);
			c += _getpixel(frame, x - 1, y);
			c += _getpixel(frame, x, y);
			c += _getpixel(frame, x + 1, y);
			c += _getpixel(frame, x - 1, y + 1);
			c += _getpixel(frame, x, y + 1);
			c += _getpixel(frame, x + 1, y + 1);
			_putpixel(frame, x, y, c / 9);
		}
	}
}

// generates the explosion frames
int s_generate_explosions() {
	int i;
	int x, y;
	unsigned char c;
	BITMAP *tmp;

	tmp = create_bitmap(32, 32);
	if (tmp == NULL) return -1;
	clear(tmp);

	// draw initial explosion
	circlefill(tmp, 15, 15, 10, 31);

	for(i = 0; i < 32; i ++) {
		s_expl_frame[i] = create_bitmap(32, 32);
		if (!s_expl_frame[i]) return -1;

		// draw new circles in the first third
		if (i < 10 && i&1) {
			x = 10 + rand()%12;
			y = 10 + rand()%12;
			c = rand()%7 + 3;
			circlefill(tmp, x, y, c, 44);		
			circle(tmp, x, y, c, 3);		
		}

		s_blend_frame(tmp);

		// copy tmp frame to animation frame and reduce colors
		clear(s_expl_frame[i]);
		for(y = 0; y < 32; y ++) {
			for(x = 0; x < 32; x ++) {
				c = MIN(_getpixel(tmp, x , y) >> 3, 4);
				if (c > 1) _putpixel(s_expl_frame[i], x , y, c);
			}
		}
	}

	return 0;
}

// inits the shooter
int s_init_shooter() {
	int i;
	
	log2file("\nStarting shooter");

	// load data
	log2file(" loading shooter data");
	packfile_password(get_init_string());
	s_data = load_datafile("data/a45.dat");
	if (!s_data) {
		log2file(" *** failed");
		return -1;
	}
	packfile_password(NULL);

	// init stars
	log2file(" init stars");
	for(i = 0; i < 64; i ++) {
		s_make_random_star(&s_star[i]);
	}

	// generate explosions
	log2file(" generating explosions");
	if (s_generate_explosions() < 0) {
		log2file(" *** failed");
		return -1;
	}
	
	// init objects
	log2file(" init objects");
	s_reset_all_objects();
	s_sign.alive = 0;

	// init player
	log2file(" init player");
	s_player.image = SHIP;
	s_player.alive = 1;
	s_player.type = SO_PLAYER;
	s_player.bx = 0;
	s_player.by = 6;
	s_player.bw = ((BITMAP *)s_data[s_player.image].dat)->w - 1;
	s_player.bh = ((BITMAP *)s_data[s_player.image].dat)->h - 10;

	// getting memory for buffer
	log2file(" reserving buffer memory");
	s_buffer = create_bitmap(160, 120);
	if (!s_buffer) {
		log2file(" *** failed");
		return -1;
	}

	// reset shooter data
	log2file(" resetting shooter data");
	s_var.level_offset = 0;
	s_var.level_counter = 0;
	s_var.event_counter = 0;
	s_var.lives = 3;
	s_var.power_gauge = 0;
	s_var.power_level = 1;
	s_var.score = s_var.show_score = 0;
	s_var.player_wait = 0;
	s_var.guardian_active = 0;
	s_var.difficulty = 1;

	// create blend map
	log2file(" creating blend map");
	s_blend_map.data[0][0] = 0;
	s_blend_map.data[0][1] = 1;
	s_blend_map.data[0][2] = 2;
	s_blend_map.data[0][3] = 3;
	s_blend_map.data[0][4] = 4;
	
	s_blend_map.data[1][0] = 1;
	s_blend_map.data[1][1] = 1;
	s_blend_map.data[1][2] = 2;
	s_blend_map.data[1][3] = 3;
	s_blend_map.data[1][4] = 4;
	
	s_blend_map.data[2][0] = 2;
	s_blend_map.data[2][1] = 2;
	s_blend_map.data[2][2] = 3;
	s_blend_map.data[2][3] = 4;
	s_blend_map.data[2][4] = 4;
	
	s_blend_map.data[3][0] = 3;
	s_blend_map.data[3][1] = 3;
	s_blend_map.data[3][2] = 4;
	s_blend_map.data[3][3] = 4;
	s_blend_map.data[3][4] = 4;
	
	s_blend_map.data[4][0] = 4;
	s_blend_map.data[4][1] = 4;
	s_blend_map.data[4][2] = 4;
	s_blend_map.data[4][3] = 4;
	s_blend_map.data[4][4] = 4;


	// lock onto music
    s_duh = (DUH *)s_data[SPACEMOD].dat;

	// done
	log2file(" init OK!");
	return 0;
}

// unints the shooter, frees memory etc
void s_uninit_shooter() {
	int i;

	log2file("\nClosing down shooter");

	log2file(" freeing explosion frames");
	for(i = 0; i < 32; i ++)
		if (s_expl_frame[i] != NULL) destroy_bitmap(s_expl_frame[i]);

	log2file(" freeing buffer");
	if (s_buffer != NULL) destroy_bitmap(s_buffer);

	log2file(" unloading datafile");
	if (s_data != NULL) unload_datafile(s_data);
}


// starts the shooter
int start_shooter(Tcontrol *c, int with_sound) {
	s_got_sound = with_sound;

	// init shooter
	if (s_init_shooter() < 0) {
		log2file(" *** init failed");
		s_uninit_shooter();
		return -1;
	}

	// get variables
	s_var.ctrl = c;

	// start the shooter
	s_run_shooter();

	// clean up
	s_uninit_shooter();

	return 0;	
}
