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

#include "player.h"
#include "timer.h"
#include "data.h"

// the player
Tplayer player;

// draws the player depending on his state
void draw_player(BITMAP *bmp, Tplayer *p, int x, int y) {
	BITMAP *head, *body;

	// if flashing, just bail out
	if (p->wounded && (game_count % 2)) return;

	if (p->actor->status == AC_DEAD) {	
		head = bitmaps[I_HERO_NORM];
		body = bitmaps[I_HERO_JUMP];
		draw_sprite_v_flip(bmp, body, x, y - 32);
		draw_sprite_v_flip(bmp, head, x, y - 16);
	}
	else if (p->actor->status == AC_BALL) {
		if (!p->actor->direction) 
			rotate_sprite(bmp, bitmaps[I_HERO_BALL], x, y-16, itofix(p->angle));
		else
			rotate_sprite_v_flip(bmp, bitmaps[I_HERO_BALL], x, y-16, itofix(p->angle + 128));
	}
	else if (p->actor->status != AC_EAT) {
		if (p->actor->status == AC_FULL) head = bitmaps[I_HERO_FULL];
		else if (p->actor->status == AC_SPIT) head = bitmaps[I_HERO_SPIT];
		else head = bitmaps[I_HERO_NORM];

		if (p->jumping)
			body = bitmaps[I_HERO_JUMP];
		else
			body = bitmaps[p->actor->frames[p->actor->frame]];

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
				draw_sprite_h_flip(bmp, bitmaps[I_HERO_EAT], x, y - 16);
			}
			else {
				draw_sprite(bmp, bitmaps[I_HERO_EAT], x - 16, y - 16);
			}
		}
		else {
			head = bitmaps[I_HERO_SPIT];
			body = bitmaps[I_HERO_JUMP];
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
