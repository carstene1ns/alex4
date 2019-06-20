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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(__FreeBSD__)
#include <machine/endian.h>
#else
#include <endian.h>
#endif

#include "allegro.h"
#include "map.h"
#include "timer.h"
#include "../data/data.h"

// creates one splendid map 
Tmap *create_map(int w, int h) {
	Tmap *m;
	
	m = malloc(sizeof(Tmap));
	if (m == NULL) return NULL;

	strcpy(m->name, "noname");

	m->map_data_needs_to_be_destroyed = 1;
	m->boss_level = 0;
	m->num_enemies = 0;
	m->start_x = 2;
	m->start_y = 4;
	m->win_conditions = MAP_WIN_EXIT;
	m->win_conditions_fullfilled = 0;

	m->offset_x = 0;
	m->offset_y = 0;
	m->width = w;
	m->height = h;
	m->dat = malloc(w * h * sizeof(Tmappos));
	
	if (m->dat == NULL) { 
		free(m);
		return NULL; 
	}
	
	// clear map
	memset(m->dat, 0, w * h * sizeof(Tmappos));

	return m;
}

static void mem_to_int(int *dest, unsigned char *mem)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	memcpy(dest, mem, 4);
#else
	*dest = mem[0] | (((int)mem[1]) << 8) | (((int)mem[2]) << 16) |
		(((int)mem[3]) << 24);
#endif
}

static int fread_int(int *dest, FILE *fp)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return (fread(dest, 4, 1, fp));
#else
	unsigned char buf[4];
	if (fread(buf, 1, 4, fp) < 4)
		return 0;
	mem_to_int(dest, buf);
	return 1;
#endif
}

static int fwrite_int(const int *src, FILE *fp)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return (fwrite(src, 4, 1, fp));
#else
	unsigned char buf[4];
	buf[0] = *src;
	buf[1] = *src >> 8;
	buf[2] = *src >> 16;
	buf[3] = *src >> 24;
	return (fwrite(buf, 1, 4, fp) == 4? 1: 0);
#endif
}

// loads one splendind map from disk
Tmap *load_map(const char *fname) {
	Tmap *m;
	FILE *fp;
	char header[6];
	
	// open file
	fp = fopen(fname, "rb");
	if (fp == NULL) {
		return NULL;
	}

	// does the header match?
	if (fread(header, 6, 1, fp) != 1) {
		fclose(fp);
		return NULL;
	}
	if (header[0] != 'A' && header[1] != 'X' && header[2] != '4' && header[3] != 'M' && header[4] != 'A' && header[5] != 'P') {
		fclose(fp);
		return NULL;
	}

	// get memory
	m = malloc(sizeof(Tmap));
	if (m == NULL) {
		fclose(fp);
		return NULL;
	}
	
	// read datastruct
	// a mapfile contain a raw dump of the Tmap struct made on an i386
	// the code below reads these struct dumps in an arch neutral manner
	// Note this dumps contains pointers, these are not used because these
	// ofcourse point to some no longer valid address.
	if (fread(m, 64, 1, fp) +           // first 64 bytes data
		fread_int(&(m->width), fp) +
		fread_int(&(m->height), fp) +
		fread(header, 4, 1, fp) +       // skip the first pointer
		fread_int(&(m->offset_x), fp) +
		fread_int(&(m->offset_y), fp) +
		fread(header, 4, 1, fp) +       // skip the second pointer
		fread_int(&(m->start_x), fp) +
		fread_int(&(m->start_y), fp) != 9) {
		fclose(fp);
		free(m);
		return NULL;
	}

	// read map data
	m->dat = malloc(m->width * m->height * sizeof(Tmappos));
	if (m->dat == NULL) {
		fclose(fp);
		free(m);
		return NULL;
	}

	if (fread(m->dat, sizeof(Tmappos), m->width * m->height, fp) != (size_t)m->width * m->height) {
		fclose(fp);
		free(m->dat);
		free(m);
		return NULL;
	}

	// close file
	fclose(fp);

	m->map_data_needs_to_be_destroyed = 1;

	return m;
}

// loads one splendind map from memory
Tmap *load_map_from_memory(void *mem) {
	Tmap *m;
	unsigned char header[6];
	unsigned char *c = (unsigned char *)mem;
	
	// does the header match?
	//fread(header, 6, 1, fp);
	memcpy(header, c, 6);
	if (header[0] != 'A' && header[1] != 'X' && header[2] != '4' && header[3] != 'M' && header[4] != 'A' && header[5] != 'P') {
		return NULL;
		//fclose(fp);
	}
	c += 6;

	// get memory
	m = malloc(sizeof(Tmap));
	if (m == NULL) {
		//fclose(fp);
		return NULL;
	}
	
	// read datastruct
	// a mapfile contain a raw dump of the Tmap struct made on an i386
	// the code below reads these struct dumps in an arch neutral manner
	// Note this dumps contains pointers, these are not used because these
	// ofcourse point to some no longer valid address.
	memcpy(m, c, 64); c += 64;             // first 64 bytes data
	mem_to_int(&(m->width), c);  c += 4;
	mem_to_int(&(m->height), c); c += 4;
	c += 4;	                               // skip the first pointer
	mem_to_int(&(m->offset_x), c); c += 4;
	mem_to_int(&(m->offset_y), c); c += 4;
	c += 4;                                // skip the second pointer
	mem_to_int(&(m->start_x), c); c+= 4;
	mem_to_int(&(m->start_y), c); c+= 4;

	// read map data
	m->dat = malloc(m->width * m->height * sizeof(Tmappos));
	if (m->dat == NULL) {
		free(m);
		return NULL;
	}

	// fread(m->dat, sizeof(Tmappos), m->width * m->height, fp);
	memcpy(m->dat, c, sizeof(Tmappos) * m->width * m->height);
	c += sizeof(Tmappos) * m->width * m->height;

	// close file
	//fclose(fp);

	m->map_data_needs_to_be_destroyed = 0;

	return m;
}

// saves a map to file
int save_map(Tmap *m, char *fname) {
	FILE *fp;
	char header[6] = "AX4MAP";
	
	// open file
	fp = fopen(fname, "wb");
	if (fp == NULL) return FALSE;

	// write header
	if (fwrite(header, 6, 1, fp) != 1) {
		fclose(fp);
		return FALSE;
	}

	// write datastruct
	// a mapfile should contain a raw dump of the Tmap struct as made on an
	// i386 the code below writes a struct dump as an i386 in an arch
	// neutral manner
	if (fwrite(m, 64, 1, fp) +           // first 64 bytes data
		fwrite_int(&(m->width), fp) +
		fwrite_int(&(m->height), fp) +
		fwrite(header, 4, 1, fp) +       // skip the first pointer
		fwrite_int(&(m->offset_x), fp) +
		fwrite_int(&(m->offset_y), fp) +
		fwrite(header, 4, 1, fp) +       // skip the second pointer
		fwrite_int(&(m->start_x), fp) +
		fwrite_int(&(m->start_y), fp) != 9) {
		fclose(fp);
		return FALSE;
	}

	// write map data
	if (fwrite(m->dat, sizeof(Tmappos), m->width * m->height, fp) != (size_t)m->width * m->height) {
		fclose(fp);
		return FALSE;
	}

	// close file
	fclose(fp);
	
	return TRUE;
}

// frees the memory of a map
void destroy_map(Tmap *m) {
	if (m == NULL) return;
	if (!m->map_data_needs_to_be_destroyed) return;

	// free data
	if (m->dat != NULL) free(m->dat);

	// free the map itself
	free(m);
}

// draws a submap to a bitmap
void draw_map(BITMAP *bmp, Tmap *sm, int dx, int dy, int w, int h, int edit) {
	int x, y;
	int tox = sm->offset_x >> 4;		// Tile Offset X
	int toy = sm->offset_y >> 4;		// Tile Offset Y
	int tx, ty;
	int pos;
	int ax = (tox << 4) - sm->offset_x; // pixel adjustments in x
	int ay = (toy << 4) - sm->offset_y; // pixel adjustments in y
	
	if (sm == NULL) return;
	
	set_clip_rect(bmp, dx, dy, dx+w-1, dy+h-1);
	
	for(y=0;y<7;y++) {
		for(x=0;x<11;x++) {
			tx = x + tox;
			ty = y + toy;
			if (tx < sm->width && ty < sm->height && tx >= 0 && ty >= 0) {
				pos = tx + ty * sm->width;

				if (sm->dat[pos].tile) {
					draw_sprite(bmp, sm->data[sm->dat[pos].tile + TILE000 - 1].dat, dx + x*16 + ax, dy + y*16 + ay);
				}


				if (sm->dat[pos].item) {
					if (sm->dat[pos].item == MAP_EGG) {
						draw_sprite(bmp, sm->data[EGG].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					else if (sm->dat[pos].item == MAP_1UP) {
						draw_sprite(bmp, sm->data[ONEUP].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					else if (sm->dat[pos].item == MAP_STAR) {
						draw_sprite(bmp, sm->data[STAR].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					else if (sm->dat[pos].item == MAP_HEART) {
						draw_sprite(bmp, sm->data[HEART].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					else if (sm->dat[pos].item == MAP_CHERRY) {
						draw_sprite(bmp, sm->data[CHERRY].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					else if (sm->dat[pos].item == MAP_LOLA) {
						draw_sprite(bmp, sm->data[LOLA].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
				}
				

				if (sm->dat[pos].type) {
					if (sm->dat[pos].type == MAP_SPIN) {
						draw_sprite(bmp, sm->data[SPIN1 + ((ABS(game_count) >> 2) % 4)].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					if (sm->dat[pos].type == MAP_WATER) {
						blit(sm->data[WATER].dat, bmp, ((ABS(game_count) >> 2) % 4) << 4, 0, dx + x*16 + ax, dy + y*16 + ay, 16, 16);
					}
					if (sm->dat[pos].type == MAP_SURFACE) {
						masked_blit(sm->data[WATER_SURFACE].dat, bmp, ((ABS(game_count) >> 2) % 8) << 4, 0, dx + x*16 + ax, dy + y*16 + ay, 16, 16);
					}
				}

				if (edit) {
					if (sm->dat[pos].type == MAP_DEAD) {
						textout_ex(bmp, font, "D", dx + x*16 + ax, dy + y*16 + ay, 0, -1);
					}
					if (sm->dat[pos].type == MAP_EXIT) {
						textout_ex(bmp, font, "XT", dx + x*16 + ax, dy + y*16 + ay, 255, -1);
					}
					if (sm->dat[pos].type == MAP_BRK) {
						textout_ex(bmp, font, "GL", dx + x*16 + ax, dy + y*16 + ay, 255, -1);
					}
					if (sm->dat[pos].type == MAP_ENEMY1) {
						draw_sprite(bmp, sm->data[ENEMY1_01 + ((ABS(game_count) >> 3) % 4)].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					if (sm->dat[pos].type == MAP_ENEMY2) {
						draw_sprite(bmp, sm->data[ENEMY2_01 + ((ABS(game_count) >> 3) % 4)].dat, dx + x*16 + ax, dy + y*16 + ay - 8);
					}
					if (sm->dat[pos].type == MAP_ENEMY3) {
						draw_sprite(bmp, sm->data[ENEMY3].dat, dx + x*16 + ax, dy + y*16 + ay - 96);
					}
					if (sm->dat[pos].type == MAP_ENEMY4) {
						draw_sprite(bmp, sm->data[ENEMY4].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					if (sm->dat[pos].type == MAP_ENEMY5) {
						draw_sprite(bmp, sm->data[ENEMY5_01].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					if (sm->dat[pos].type == MAP_ENEMY6) {
						draw_sprite(bmp, sm->data[ENEMY6].dat, dx + x*16 + ax, dy + y*16 + ay);
					}
					if (sm->dat[pos].type == MAP_GUARD1) {
						draw_sprite(bmp, sm->data[GUARD1_1].dat, dx + x*16 + ax, dy + y*16 + ay - 16);
					}
					if (sm->dat[pos].type == MAP_GUARD2) {
						draw_sprite(bmp, sm->data[GUARD2_1a].dat, dx + x*16 + ax, dy + y*16 + ay - 32);
					}
				}
			}
		}
	}

	set_clip_rect(bmp, 0, 0, SCREEN_W - 1, SCREEN_H - 1);
	
}

// gets a pointer to the mappos at tx, ty
Tmappos *get_mappos(Tmap *m, int tx, int ty) {
	if (m == NULL) return NULL;
	if (tx < 0 || tx >= m->width) return NULL;
	if (ty < 0 || ty >= m->height) return NULL;

	return &m->dat[tx + ty * m->width];
}

// copies a part of a map to another part
void copy_map(Tmap *src, Tmap *dst, int sx, int sy, int dx, int dy, int w, int h) {
	int x, y;
	for(x=0;x<w;x++)
		for(y=0;y<h;y++)
			dst->dat[dx + x + (dy + y) * dst->width] = src->dat[sx + x + (sy + y) * src->width];
}

// changes the size of the sub map according to dw and dh
// dir_flags hold on which sides to shrink and/or expand
void change_map_size(Tmap *sm, int dw, int dh, int dir_flags) {
	Tmap tsm;
	int i;

	if (sm == NULL) return;
	if (!dir_flags) return;

	if (dh) {
		// save old map and create new
		tsm.dat = sm->dat;
		tsm.width = sm->width;
		tsm.height = sm->height;
		sm->height += dh;
		sm->dat = malloc(sm->width * sm->height * sizeof(Tmappos));
		for(i=0;i<sm->width*sm->height;i++) sm->dat[i].tile = sm->dat[i].mask = sm->dat[i].type = sm->dat[i].item = 0;
	
		// copy relevant parts
		if (dh > 0) {
			if      (dir_flags & SM_UP  ) copy_map(&tsm, sm, 0,  0,  0, dh, tsm.width, tsm.height);
			else if (dir_flags & SM_DOWN) copy_map(&tsm, sm, 0,  0,  0,  0, tsm.width, tsm.height);
		}
		else if (dh < 0) {
			if      (dir_flags & SM_UP  ) copy_map(&tsm, sm,  0,-dh, 0, 0, tsm.width, tsm.height + dh);
			else if (dir_flags & SM_DOWN) copy_map(&tsm, sm,  0,  0, 0, 0, tsm.width, tsm.height + dh);
		}

		free(tsm.dat);
	}

	if (dw) {
		// save old map and create new
		tsm.dat = sm->dat;
		tsm.width = sm->width;
		tsm.height = sm->height;
		sm->width += dw;
		sm->dat = malloc(sm->width * sm->height * sizeof(Tmappos));
		for(i=0;i<sm->width*sm->height;i++) sm->dat[i].tile = sm->dat[i].mask = sm->dat[i].type = sm->dat[i].item = 0;
	
		// copy relevant parts
		if (dw > 0) {
			if      (dir_flags & SM_LEFT ) copy_map(&tsm, sm, 0,  0, dw, 0, tsm.width, tsm.height);
			else if (dir_flags & SM_RIGHT) copy_map(&tsm, sm, 0,  0,  0, 0, tsm.width, tsm.height);
		}
		else if (dw < 0) {
			if      (dir_flags & SM_LEFT ) copy_map(&tsm, sm,-dw, 0, 0, 0, tsm.width + dw, tsm.height);
			else if (dir_flags & SM_RIGHT) copy_map(&tsm, sm,  0, 0, 0, 0, tsm.width + dw, tsm.height);
		}

		free(tsm.dat);
	}

}

// returns the mask id if there is ground at a,y
// can handel masks [0, 8]
int is_ground(Tmap *sm , int x, int y) {
	int tile_x = x >> 4;
	int tile_y = y >> 4;
	int ox, oy;
	int pos, mask;

	// return FALSE if outside map
	if (tile_x < 0 || tile_x >= sm->width) return 1;
	if (tile_y < 0 || tile_y >= sm->height) return 0;
	
	// cals position and mask
	pos = tile_x + tile_y * sm->width;
	mask = sm->dat[pos].mask;

	// the simple ones
	if (mask < 2) return mask;	// 0 = no mask, 1 = full mask

	// calculate offset from top left corner of tile
	ox = x - (tile_x << 5);
	oy = y - (tile_y << 5);

	// the semi-simple ones
	if (mask == 2) {				// half full block
		if (oy < 16) return 0;
		else return mask;
	}

	if (mask == 5 && oy > 31 - ox) return mask; // 45 degree slope /     (med)
	if (mask == 6 && oy > ox) return mask;		// 45 degree slope \ (med)

	// the not so simple ones
	if (mask == 3 && oy > 31 - ox / 2) return mask;	// 22 degree slope / (low)
	if (mask == 4 && oy > 15 - ox / 2) return mask;	// 22 degree slope / (hi)
	if (mask == 7 && oy > ox / 2)      return mask;	// 22 degree slope \ (hi)
	if (mask == 8 && oy > 16 + ox / 2) return mask;	// 22 degree slope \ (low)

	// return FALSE for all other cases
	return 0;
}

// returns delta to new y coord
int adjust_ypos (Tmap *sm, int x, int y, int ground, int dy) {
	int oy = y;
	if (ground)	while (is_ground(sm, x, y))  y += dy;
	else		while (!is_ground(sm, x, y)) y += dy;
	return y - oy;
}

// returns delta to new x coord
int adjust_xpos (Tmap *sm, int x, int y, int ground, int dx) {
	int ox = x;
	if (ground)	while (is_ground(sm, x, y))  x += dx;
	else		while (!is_ground(sm, x, y)) x += dx;
	return x - ox;
}

// sets the tile id
// returns 1 if all went well
int set_tile(Tmap *m, int tx, int ty, int tile) {
	if (tx < 0 || ty < 0) return 0;
	if (tx >= m->width || ty >= m->height) return 0;
	m->dat[tx + ty * m->width].tile = tile;
	return 1;
}

// returns the tile id
int get_tile(Tmap *m, int tx, int ty) {
	if (tx < 0 || ty < 0) return 0;
	if (tx >= m->width || ty >= m->height) return 0;
	return m->dat[tx + ty * m->width].tile;
}
