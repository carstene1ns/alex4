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
 
 
 

#include "allegro.h"
#include "actor.h"
#include "map.h"
#include "timer.h"
#include "player.h"
#include "bullet.h"
#include "particle.h"
#include "main.h"


#include "../data/data.h"

// pointer to datafile declared in main.c
extern DATAFILE* data;

// draw an actor at specified position
void draw_actor(BITMAP *bmp, Tactor *a, int x, int y) {
	// check if the actor has any frames and quit otherwise
	if (!a->num_frames) return;

	if (a->status == AC_NORM) {
		// normal drawing, just select direction
		if (a->direction) 
			draw_sprite_h_flip(bmp, a->data[a->frames[a->frame] + (a->is_hit ? a->hit_offset : 0)].dat, x, y - a->h - a->oy);
		else
			draw_sprite(bmp, a->data[a->frames[a->frame] + (a->is_hit ? a->hit_offset : 0)].dat, x, y - a->h - a->oy);
	}
	else if (a->status == AC_DEAD) {
		// draw dead frame, check for direction
		if (a->direction) 
			draw_sprite_vh_flip(bmp, a->data[a->frames[a->frame]].dat, x, y - a->h - a->oy);
		else
			draw_sprite_v_flip(bmp, a->data[a->frames[a->frame]].dat, x, y - a->h - a->oy);
	}
	else if (a->status == AC_FLAT) {
		// draw flat frame, check for direction
		if (a->direction) 
			draw_sprite_h_flip(bmp, a->data[a->flat_frame].dat, x, y - a->h - a->oy);
		else
			draw_sprite(bmp, a->data[a->flat_frame].dat, x, y - a->h - a->oy);
	}
}


// animates the actor one tick
void animate_actor(Tactor *a) {
	a->anim_counter ++;
	if (a->anim_counter > a->anim_max) {
		a->anim_counter = 0;
		a->frame ++;
		if (a->frame >= a->num_frames)
			a->frame = 0;
	}
}

// gets an unused actor from the array a
// a = array to look in
Tactor *make_actor(Tactor *a, int x, int y, DATAFILE *data) {
	int i = 0;
	// find free slot
	while(a[i].active && i < MAX_ACTORS) i ++;

	// set values
	if (i < MAX_ACTORS) {
		a[i].active = 1;
		a[i].status = AC_NORM;
		a[i].x = x;
		a[i].y = y;
		a[i].direction = 0;
		a[i].data = data;

		a[i].frame = 0;
		a[i].anim_counter = 0;
		a[i].anim_max = 4;

		a[i].dy = a[i].dx = a[i].tx = 0;
		a[i].mode = a[i].toggle = 0;
		a[i].counter = 0;

		a[i].energy = 1;
		a[i].is_hit = 0;
		a[i].sound = -1;

		return &a[i];
	}

	// failed
	return NULL;
}

// special handler for jumping the spike crusher guardian
void _crush_jump(Tactor *a, Tmap *m) {
	a->y += a->dy >> 2;
	a->dy ++;
	if (a->x < a->tx) a->x ++;
	if (a->x > a->tx) a->x --;

	// check left and right
	if (is_ground(m, a->x, a->y-1)) a->x ++;
	if (is_ground(m, a->x+31, a->y-1)) a->x --;

	// check ground
	if (is_ground(m, a->x+1, a->y)) {
		a->y += adjust_ypos(m, a->x+1, a->y - 1, 1, -1);
		create_burst(particle, a->x +  8, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 16, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 24, a->y, 8, 10, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->mode ++;
	}
	else if (is_ground(m, a->x+15, a->y)) {
		a->y += adjust_ypos(m, a->x+15, a->y - 1, 1, -1);
		create_burst(particle, a->x +  8, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 16, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 24, a->y, 8, 10, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->mode ++;
	}
	else if (is_ground(m, a->x+30, a->y)) {
		a->y += adjust_ypos(m, a->x+30, a->y - 1, 1, -1);
		create_burst(particle, a->x +  8, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 16, a->y, 8, 10, 25, PARTICLE_DUST);
		create_burst(particle, a->x + 24, a->y, 8, 10, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->mode ++;
	}
}

// special handler for jumping the spike crusher guardian
void _spike_jump(Tactor *a, Tmap *m) {
	a->y += a->dy >> 2;
	a->dy ++;
	if (a->x < a->tx) a->x ++;
	if (a->x > a->tx) a->x --;
	
	// check left and right
	if (is_ground(m, a->x, a->y-1)) a->x ++;
	if (is_ground(m, a->x+31, a->y-1)) a->x --;

	// check ground
	if (is_ground(m, a->x+1, a->y)) {
		int tx, ty;
		Tmappos *mp;

		tx = (a->x+1)>>4;
		ty = (a->y)>>4;
		mp = get_mappos(m, tx, ty);
		if (mp->type == MAP_BRK) {
			mp->tile = mp->type = mp->mask = 0;
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			play_sound_id(SMPL_CRUSH);
		}

		create_burst(particle, a->x + 16, a->y, 8, 6, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->toggle = 1;  // don't crush any more , fall and land
	}

	// check ground
	if (is_ground(m, a->x+15, a->y)) {
		int tx, ty;
		Tmappos *mp;

		tx = (a->x+15)>>4;
		ty = (a->y)>>4;
		mp = get_mappos(m, tx, ty);
		if (mp->type == MAP_BRK) {
			mp->tile = mp->type = mp->mask = 0;
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			play_sound_id(SMPL_CRUSH);
		}

		create_burst(particle, a->x + 16, a->y, 8, 6, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->toggle = 1;  // don't crush any more , fall and land
	}

	// check ground
	if (is_ground(m, a->x+30, a->y)) {
		int tx, ty;
		Tmappos *mp;

		tx = (a->x+30)>>4;
		ty = (a->y)>>4;
		mp = get_mappos(m, tx, ty);
		if (mp->type == MAP_BRK) {
			mp->tile = mp->type = mp->mask = 0;
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 32, 64, 0, -1);
			play_sound_id(SMPL_CRUSH);
		}

		create_burst(particle, a->x + 16, a->y, 8, 6, 25, PARTICLE_DUST);
		play_sound_id(SMPL_CRUSH_LAND);
		a->toggle = 1;  // don't crush any more , fall and land
	}

}


// checks the actor with the tile map
// this is an ugly function with an if clause
// for each enemy type
void update_actor_with_map(Tactor *a, Tmap *m) {
	if (a->energy <= 0 && a->status == AC_NORM) {
		a->status = AC_DEAD;	
		play_sound_id(SMPL_E_DIE);
		a->dy = -8;
		if (a->type == MAP_GUARD1) a->dy =- 16;
		
	}

	if (a->status == AC_DEAD) {
		if (a->direction) a->x -= 1;
		else a->x += 1;
		a->dy ++;
		a->y += a->dy >> 2;

		// add smoke to dying actor
		if (a->type == MAP_GUARD1) {
			Tparticle *p;
			p = get_free_particle(particle, MAX_PARTICLES);
			if (p != NULL) {
				set_particle(p, a->x + 8 + rand()%16, a->y - 8 - rand()%16, 0, 0, -1, 16, SMOKE1);
			}
		}
	} 
	else if (a->status == AC_FLAT) {
		if (a->counter > 200) {
			a->x += (a->counter % 3) - 1;
		}
		if (++ a->counter > 250) a->status = AC_NORM;

		if (!is_ground(m, (int)a->x + 2, (int)a->y + 0)) {
			if (!is_ground(m, (int)a->x + 13, (int)a->y + 0)) {
				a->dy ++;
				a->y += a->dy >> 2;
				while(is_ground(m, (int)a->x + 13, (int)a->y - 2)) { a->y --; a->dy = 0; }
				while(is_ground(m, (int)a->x + 2, (int)a->y - 2)) { a->y --; a->dy = 0; }
			}
		}

	}	
	else if (a->type == MAP_ENEMY6) {		// cannon
		a->mode ++;
		if (a->mode >= 125) {
			Tbullet *b = get_free_bullet(bullet, MAX_BULLETS);
			if (b != NULL) {
				int diff = (player.actor->x + 8) - (a->x + 8);
				set_bullet(b, a->x + (diff > 0 ? 8 : -12), a->y - 16, (diff > 0 ? 3 : -3), 0, data[ENEMY6].dat, 1);
				create_burst(particle, a->x + (diff > 0 ? 18 : -2), a->y - 8, 8, 8, 25, PARTICLE_DUST);
				a->mode = 0;
				play_sound_id(SMPL_SHOOT);
			}
		}
	}
	else if (a->type == MAP_ENEMY4 || a->type == MAP_ENEMY5) {	// fish
		if (a->mode == 0) {
			int b[] = {8, 14, 18, 21, 23, 26, 28};
			a->ymax = a->y;
			a->y = 120;
			a->mode ++;
			a->dy = -b[MIN(MAX(ABS(a->ymax - a->y) / 16, 0), 7)];
			a->ymax = a->dy;
		}
		else if (a->mode == 1) {
			a->dy ++;
			a->y += a->dy >> 2;
			if (a->y >= 120) {
				a->y = 130;
				a->mode = 2;
			}

			if (a->type == MAP_ENEMY5) {
				a->frames[0] = (a->dy < 0 ? ENEMY5_02 : ENEMY5_01);
			}
		}
		else if (a->mode >= 2) {
			a->mode ++;
			if (a->mode > 40) {
				a->y = 120;
				a->dy = a->ymax;
				a->mode = 1;
			}
		}
	}
	else if (a->type == MAP_ENEMY3) {		// stomper/crusher
		if (a->mode == 0) {
			a->ymax = a->y;
			a->mode ++;
			a->dy = 0;
		}
		else if (a->mode == 1) {
			int diff = ABS((player.actor->x + 8) - (a->x + 16));
			if (diff < 32) {
				a->mode = 2;
				a->dy = 0;
				a->dy = -4;
			}
		}
		else if (a->mode == 2) {
			int hit = 0;
			
			a->dy ++;
			a->y += a->dy >> 2;
			
			while(is_ground(m, (int)a->x + 8, (int)a->y - 1)) { a->y --; hit = 1; }
			while(is_ground(m, (int)a->x + 24, (int)a->y - 1)) { a->y --; hit = 1; }

			if (hit) {
				a->dy = -1;
				a->mode = 3;
				play_sound_id(SMPL_CRUSH_LAND);
				create_burst(particle, a->x + 16, a->y, 8, 6, 25, PARTICLE_DUST);
			}
		}
		else if (a->mode >= 3) {
			a->mode ++;
			if (a->mode > 25) {
				if (game_count & 1) a->y += a->dy;
				if (a->y <=  a->ymax) a->mode = 1;
			}
		}
	}
	else if (a->type == MAP_GUARD2) {		// ground pounder
		/* MODES:
			0 long wait
			    if jumped on, mode = 4
			1 get alex pos
			2 jump
			3 mode = 0
			4 spikes on, wait
			5 get alex pos
			6 jump
			7 small wait 
			8 spikes off
			9 small wait 
		   10 fire
		   11 small wait 
		   12 mode = 0
		*/

		Tactor *alex = get_alex();
		switch(a->mode) {
			case 0:		// wait
				a->frames[0] = GUARD2_1a;
				a->frames[1] = GUARD2_1b;
				a->counter ++; 
				if (a->counter > 60) {
					a->mode ++;
					a->counter = 0;
				}
				break;
			case 1:		// aim
				a->tx = alex->x - 8;
				a->dy = - MIN(MAX(ABS(a->x - a->tx) / 2, 10), 30);
				a->mode ++;
				break;
			case 2:		// jump
				a->frames[0] = a->frames[1] = GUARD2_2;
				_crush_jump(a, m);
				break;
			case 3:		// land, goto 0
				a->mode = 0;
				break;
			case 4:		// turn on spikes, wait
				a->frames[0] = a->frames[1] = GUARD2_3;
				a->counter ++; 
				if (a->counter > 50) {
					a->mode ++;
					a->counter = 0;
				}
				break;
			case 5:		// aim
				a->tx = alex->x - 8;
				a->dy = - MAX(ABS(a->x - a->tx) / 2, 10);
				a->mode ++;	
				a->toggle = 0;  // crush
				break;
			case 6:		// jump 
				a->frames[0] = a->frames[1] = GUARD2_4;
				if (a->toggle) _crush_jump(a, m);
				else _spike_jump(a, m);
				break;
			case 7:		// land, short wait
				a->frames[0] = a->frames[1] = GUARD2_5;
				a->counter ++; 
				if (a->counter > 50) {
					a->mode ++;
					a->counter = 0;
				}
				break;
			case 8: {		// fire
				Tbullet *b;
				int i;
				
				play_sound_id(SMPL_SHOOT);
				for(i = 0; i < 3; i ++) {
					b = get_free_bullet(bullet, MAX_BULLETS);
					if (b != NULL) {
						set_bullet(b, a->x + 16, a->y - 32, rand()%5 - 2, -(rand()%5 + 2), data[BULLET_1].dat, 1);
						b->animate = b->gravity = 1;
						b->bmp2 = data[BULLET_2].dat;
					}
				}
				a->mode ++;
				break;
			}
			case 9:		// wait
				a->counter ++; 
				if (a->counter > 10) {
					a->mode ++;
					a->counter = 0;
				}
				break;
			case 10:		// spikes off
				a->frames[0] = a->frames[1] = GUARD2_6;
				a->mode ++;
				break;
			case 11:		// wait
				a->counter ++; 
				if (a->counter > 10) {
					a->mode ++;
					a->counter = 0;
				}
				break;
			case 12:		// return
				a->frames[0] = GUARD2_1a;
				a->frames[1] = GUARD2_1b;
				a->mode = 0;
				break;
		}
		animate_actor(a);
	}
	else if (a->status == AC_NORM) {  // other ppl
		if (game_count & 1) {
			if (a->direction) a->x += 1;
			else a->x -= 1;
		}
		animate_actor(a);

		if (a->type == MAP_ENEMY2 || a->type == MAP_GUARD1) {
			if (!is_ground(m, (int)a->x + (a->direction ? a->w - 1 : 0), (int)a->y + 2)) 
				a->direction = (a->direction ? 0 : 1);
		}
		else {
			if (!is_ground(m, (int)a->x + 2, (int)a->y + 0)) {
				if (!is_ground(m, (int)a->x + 13, (int)a->y + 0)) {
					a->dy ++;
					a->y += a->dy >> 2;
					while(is_ground(m, (int)a->x + 13, (int)a->y - 1)) { a->y --; a->dy = 0; }
					while(is_ground(m, (int)a->x + 2, (int)a->y - 1)) { a->y --; a->dy = 0; }
				}
			}
		}

		if (is_ground(m, (int)a->x + (a->direction ? a->w : -1), (int)a->y - 2)) a->direction = (a->direction ? 0 : 1);

	}

	// play/adjust sound
	if (a->sound != -1) {
		adjust_sound_id_ex(a->sound, a->x);
	}
	
	

	if (a->y > 160 + (a->type == MAP_GUARD2 || a->type == MAP_GUARD1 ? 600 : 0)) {
		// change win conditions if needed
		if (a->type == MAP_GUARD2 || a->type == MAP_GUARD1)  {
			m->win_conditions_fullfilled |= m->win_conditions & MAP_WIN_KILL_GUARDIAN;
		}

		a->active = 0;
		
		// stop sample if available
		if (a->sound != -1) stop_sound_id(a->sound);
		
		// if not already doen, decrease number of enemies left
		if (a->status != AC_DEAD) {
			m->num_enemies --;
		}
	}
}


// puts the actor into dead-mode
void kill_actor(Tactor *a) {
	a->status = AC_DEAD;
	a->dy = -10;
}

