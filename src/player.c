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
 
 
 


#include "player.h"
#include "timer.h"
#include "../data/data.h"

// draws the player depending on his state
void draw_player(BITMAP *bmp, Tplayer *p, int x, int y) {
	BITMAP *head, *body;

	// if flashing, just bail out
	if (p->wounded && (game_count % 2)) return;

	if (p->actor->status == AC_DEAD) {	
		head = p->actor->data[HERO_NORM].dat;
		body = p->actor->data[HERO_JUMP].dat;
		draw_sprite_v_flip(bmp, body, x, y - 32);
		draw_sprite_v_flip(bmp, head, x, y - 16);
	}
	else if (p->actor->status == AC_BALL) {
		if (!p->actor->direction) 
			rotate_sprite(bmp, p->actor->data[HERO_BALL].dat, x, y-16, itofix(p->angle));
		else
			rotate_sprite_v_flip(bmp, p->actor->data[HERO_BALL].dat, x, y-16, itofix(p->angle + 128));
	}
	else if (p->actor->status != AC_EAT) {
		if (p->actor->status == AC_FULL) head = p->actor->data[HERO_FULL].dat;
		else if (p->actor->status == AC_SPIT) head = p->actor->data[HERO_SPIT].dat;
		else head = p->actor->data[HERO_NORM].dat;

		if (p->jumping)
			body = p->actor->data[HERO_JUMP].dat;
		else
			body = p->actor->data[p->actor->frames[p->actor->frame]].dat;

		if (p->actor->direction) {
			draw_sprite_h_flip(bmp, body, x, y - 16);
			draw_sprite_h_flip(bmp, head, x, y - 16);
		}
		else {
			draw_sprite(bmp, body, x, y - 16);
			draw_sprite(bmp, head, x, y - 16);
		}
	}
	else {
		if (!p->jumping) {
			if (p->actor->direction) {
				draw_sprite_h_flip(bmp, p->actor->data[HERO_EAT].dat, x, y - 16);
			}
			else {
				draw_sprite(bmp, p->actor->data[HERO_EAT].dat, x - 16, y - 16);
			}
		}
		else {
			head = p->actor->data[HERO_SPIT].dat;
			body = p->actor->data[HERO_JUMP].dat;
			if (p->actor->direction) {
				draw_sprite_h_flip(bmp, body, x, y - 16);
				draw_sprite_h_flip(bmp, head, x, y - 16);
			}
			else {
				draw_sprite(bmp, body, x, y - 16);
				draw_sprite(bmp, head, x, y - 16);
			}
		}
	}
}

// set player as dead
void kill_player(Tplayer *p) {
	p->lives --;
	p->ammo = 0;
	p->wounded = 0;
	p->health = 0;
	p->actor->status = AC_DEAD;
	p->actor->dy = -15;
}

// decreases player health etc
void wound_player(Tplayer *p) {
	if (p->wounded) return;
	p->health --;
	p->wounded = 100;
	if (p->health <= 0)	kill_player(p);
}


