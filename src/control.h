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
 
 
 
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdio.h>

// flags for each game key
#define	K_LEFT		0x0001
#define	K_RIGHT		0x0002
#define K_UP		0x0004
#define K_DOWN		0x0008
#define	K_FIRE		0x0010
#define	K_JUMP		0x0020

// struct holding all control data
typedef struct {
	int use_joy;
	int key_left,				// actual keys
		key_right,
		key_up,
		key_down,
		key_fire,
		key_jump;

	unsigned char flags;					// bit field w/ info on which keys that are down
} Tcontrol;


// functions
void init_control(Tcontrol *c);
void save_control(Tcontrol *c, PACKFILE *fp);
void load_control(Tcontrol *c, PACKFILE *fp);
void set_control(Tcontrol *c, int up, int down, int left, int right, int fire, int jump);
void poll_control(Tcontrol *c);
int check_control_key(Tcontrol *c, int key);
int is_up(Tcontrol *c);
int is_down(Tcontrol *c);
int is_left(Tcontrol *c);
int is_right(Tcontrol *c);
int is_fire(Tcontrol *c);
int is_jump(Tcontrol *c);
int is_any(Tcontrol *c);

#endif
