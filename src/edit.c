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
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "map.h"
#include "edit.h"
#include "timer.h"
#include "../data/data.h"

// current tile
int edit_tile = 0;
// path to current map
char edit_path_and_file[1024];
// datafile to use
DATAFILE *data;
// current edit mode
int edit_mode;

// set new edit mode
void set_edit_mode(int mode) {
	edit_mode = mode;
}

// set the path for the current map
void set_edit_path_and_file(char *str) {
	strcpy(edit_path_and_file, str);
	log2file("  edit path set to: <%s>", edit_path_and_file);
}

// returns the path for the current map
char *get_edit_path_and_file() {
	return edit_path_and_file;
}

// draws the edit screens, panels, whatever
void draw_edit_mode(BITMAP *bmp, Tmap *map, int mx, int my) {
	int x, y;

	if (edit_mode == EDIT_MODE_DRAW) { 	// draw map and HUD
		int tx = (mx + map->offset_x) >> 4;
		int ty = my >> 4;

		// blit mouse tile
		if (tx < map->width && ty < map->height) {
			if (edit_tile) draw_sprite(bmp, data[TILE000 + edit_tile - 1].dat, (tx << 4) - map->offset_x, ty << 4);
			rect(bmp, (tx << 4) - map->offset_x - 1, (ty << 4) - 1, (tx << 4)+16 - map->offset_x, (ty << 4)+16, gui_bg_color);
		}
	
		// show stuff
		textprintf(bmp, data[THE_FONT].dat, 1, 1, 0, "TILE: %d,%d", tx, ty);
		textprintf(bmp, data[THE_FONT].dat, 1, 11, 0, "SIZE: %d,%d", map->width, map->height);

		// show start pos
		x = (ABS(map->start_x) << 4) - map->offset_x;
		y = (map->start_y << 4) - map->offset_y;
		rect(bmp, x+5, y+5, x+10, y+10, gui_bg_color);
		line(bmp, x+8, y+8, x+8+SGN(map->start_x)*6, y+8, gui_bg_color);
	
		// draw status bar
		rectfill(bmp, 0, 110, 159, 119, 1);
		textprintf(bmp, data[THE_FONT].dat, 1, 111, 4, "EDITING: %s", get_filename(edit_path_and_file));
	}
	else if (edit_mode == EDIT_MODE_SELECT) {	// draw tile palette
		// calculate offset depending on mouse pointer
		int ox = (32.0 * mx) / 160.0;
		int oy = ( 8.0 * my) / 120.0;
		int tx = (mx + ox) / 16;
		int ty = (my + oy) / 16;

		// draw 
		clear_to_color(bmp, gui_bg_color);
		for(y = 0; y < 8; y ++) {
			for(x = 0; x < 12; x ++) {
				draw_sprite(bmp, data[TILE000 + x + y * 12].dat, x * 16 - ox, y * 16 - oy);
			}
		}
		
		// draw selector
		if (tx >= 0 && tx < 12 && ty >= 0 && ty < 8)
			rect(bmp, tx * 16 - 1 - ox, ty * 16 - 1 - oy, tx * 16 + 16 - ox, ty * 16 + 16 - oy, (logic_count & 4 ? gui_bg_color : gui_fg_color));
	}
	else if (edit_mode == EDIT_MODE_STATS) {	// draw map properties
		int ty = 16;
		clear_to_color(bmp, 3);
		textprintf(bmp, data[THE_FONT].dat, 1, 1, 1, "%s (props)", get_filename(edit_path_and_file));	
		line(bmp, 0, 10, 159, 10, 1);
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "Win by:");
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "  1) reach exit (%s)", (map->win_conditions & MAP_WIN_EXIT ? "X" : " "));
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "  2) kill boss  (%s)", (map->win_conditions & MAP_WIN_KILL_GUARDIAN ? "X" : " "));
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "  3) kill all   (%s)", (map->win_conditions & MAP_WIN_KILL_ALL ? "X" : " "));
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "4) Boss level:  (%s)", (map->boss_level ? "X" : " "));
		textprintf(bmp, data[THE_FONT].dat, 1, ty+=10, 1, "5) Name: %s", map->name);

		textprintf(bmp, data[THE_FONT].dat, 1, 110, 1, "F1: back to editor");
	}

	if (edit_mode != EDIT_MODE_STATS) {
		// draw mouse
		line(bmp, mx-4, my,   mx+4, my, gui_fg_color);
		line(bmp, mx,   my-4, mx,   my+4, gui_fg_color);
	}
}



// updates the edit mode
void update_edit_mode(Tmap *map, BITMAP *bmp, int mx, int my, int mb) {
	
	if (edit_mode == EDIT_MODE_DRAW) {
		int tx = (mx + map->offset_x) >> 4;
		int ty = my >> 4;
		int	pos = tx + ty * map->width;

		if (mb == 1) {
			// put tile
			if (set_tile(map, tx, ty, edit_tile)) {
				if (edit_tile > 0 && edit_tile <= 48) map->dat[pos].mask = 1;
				else map->dat[pos].mask = 0;
			}
		}
		if (mb == 2) {
			// get tile
			edit_tile = get_tile(map, tx, ty);
		}


		// Handle key presses
		//
		// note the ugly "if (ty < 7) ...)" that I had to
		// add on all map changing commands because I had bad
		// design and didn't notice until afterwards that
		// it was possible to click outside the map :(
		if (keypressed()) {
			int kb = readkey() & 0xff;
			char buf[1024];
			int ret;
			Tmap *tmp_map;
			
			switch(kb) {
			// next tile
			case 'x': 
				edit_tile ++;
				if (edit_tile > 96) edit_tile = 0;
				break;
			// previous tile
			case 'z':
				edit_tile --;
				if (edit_tile < 0) edit_tile = 96;
				break;
			// save map
			case 'S':
				// get filename from user
				strcpy(buf, edit_path_and_file);
				ret = file_select_ex("select map to save", buf, "MAP", sizeof(buf), 400, 300);
				if (ret) {
					int ok_to_save = 1;
					// check if file already exists and prompt user
					// for confirmation about overwrite
					if (exists(buf)) {
						char tmp_buf[256];
						sprintf(tmp_buf, "The file '%s' already", get_filename(buf));
						ok_to_save = alert("Alex 4: OVERWRITE?", tmp_buf, "exists. Overwrite?", "YES", "NO", KEY_SPACE, KEY_ESC);
						ok_to_save = (ok_to_save == 2 ? 0 : 1);
					}

					// allright, let's save
					if (ok_to_save) {
						if (save_map(map, buf)) {
							msg_box("map saved");
							log2file("map saved: %s", buf);
							strcpy(edit_path_and_file, buf);
						}
						else {
							msg_box("save failed");
						}
					}
					else {
						msg_box("Map not saved.");
					}
				}
				break;
			// load map
			case 'L':
				// get filename from user
				strcpy(buf, edit_path_and_file);
				ret = file_select_ex("select map to load", buf, "MAP", sizeof(buf), 400, 300);
				if (ret) {
					tmp_map = load_map(buf);
					if (tmp_map != NULL) {
						int tox;
						// destroy old map and switch in the new one
						destroy_map(map);
						map = tmp_map;
						msg_box("map loaded");
						log2file("map loaded: %s", buf);
						// set the new map as current
						set_map(map);
						// setup initial map variables
						map->data = data;
						tox = MAX(0, (ABS(map->start_x) << 4) - 80);
						tox = MIN((map->width << 4) - 160, tox);
						map->offset_x = tox;
						strcpy(edit_path_and_file, buf);
					}
					else {
						msg_box("load failed");
					}
				}
				break;
			// position items on map
			case '1':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_1UP ? 0 : MAP_1UP);
				break;
			case '2':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_STAR ? 0 : MAP_STAR);
				break;
			case '3':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_CHERRY ? 0 : MAP_CHERRY);
				break;
			case '4':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_EGG ? 0 : MAP_EGG);
				break;
			case '5':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_HEART ? 0 : MAP_HEART);
				break;
			case '6':
				if (ty < 7) map->dat[pos].item = (map->dat[pos].item == MAP_LOLA ? 0 : MAP_LOLA);
				break;
			// position tile types on map
			case 'q':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_DEAD ? 0 : MAP_DEAD);
				break;
			case 'w':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_EXIT ? 0 : MAP_EXIT);
				break;
			case 'e':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_SPIN ? 0 : MAP_SPIN);
				break;
			case 'r':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_BRK ? 0 : MAP_BRK);
				break;
			case 't':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_WATER ? 0 : MAP_WATER);
				break;
			case 'y':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_SURFACE ? 0 : MAP_SURFACE);
				break;
			// position enemies on map
			case 'Q':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY1 ? 0 : MAP_ENEMY1);
				break;
			case 'W':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY2 ? 0 : MAP_ENEMY2);
				break;
			case 'E':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY3 ? 0 : MAP_ENEMY3);
				break;
			case 'R':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY4 ? 0 : MAP_ENEMY4);
				break;
			case 'T':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY5 ? 0 : MAP_ENEMY5);
				break;
			case 'Y':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_ENEMY6 ? 0 : MAP_ENEMY6);
				break;
			case 'U':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_GUARD1 ? 0 : MAP_GUARD1);
				break;
			case 'I':
				if (ty < 7) map->dat[pos].type = (map->dat[pos].type == MAP_GUARD2 ? 0 : MAP_GUARD2);
				break;
			// set alex start position
			case 'a':
				if (ty < 7) {
					map->start_x = tx;
					map->start_y = ty;
				}
				break;
			case 'A':
				if (ty < 7) {
					map->start_x = -tx;
					map->start_y = ty;
				}
				break;
			// show tile palette
			case 'p':
				set_edit_mode(EDIT_MODE_SELECT);				
				break;
			default:
				break;
			}
		}	
		
		// move map
		if (key[KEY_LEFT])  map->offset_x = MAX(0, map->offset_x - 4);
		if (key[KEY_RIGHT]) map->offset_x = MIN((map->width << 4) - 160, map->offset_x + 4);
		
		// change map size
		if (key[KEY_INSERT] && key[KEY_RIGHT]) change_map_size(map, 1, 0, SM_RIGHT);
		if (key[KEY_DEL] && key[KEY_RIGHT] && map->width > 10) change_map_size(map, -1,  0, SM_RIGHT);
		// when pressing left, alex' start pos must be adjusted too
		if (key[KEY_INSERT] && key[KEY_LEFT])  {
			change_map_size(map, 1, 0, SM_LEFT);
			if (map->start_x >= 0) map->start_x ++;
			else map->start_x --;
		}
		if (key[KEY_DEL] && key[KEY_LEFT] && map->width > 10) {
			change_map_size(map, -1,  0, SM_LEFT); 
			if (map->start_x >= 0) map->start_x --;
			else map->start_x ++;
		}
		
		// enter stat mode
		if (key[KEY_F1]) {
			edit_mode = EDIT_MODE_STATS;
			clear_keybuf();
			while(key[KEY_F1]);
		}

	}
	else if (edit_mode == EDIT_MODE_SELECT) {
		int ox = (32.0 * mx) / 160.0;
		int oy = ( 8.0 * my) / 120.0;
		int tx = (mx + ox) / 16;
		int ty = (my + oy) / 16;
		
		// get new tile from palette
		if (mb) {
			edit_tile = tx + ty * 12 + 1;
			set_edit_mode(EDIT_MODE_DRAW);
			while(mouse_b);
			clear_keybuf();
		}
	}
	else if (edit_mode == EDIT_MODE_STATS) {
		if (keypressed()) {
			int kb = readkey() & 0xff;

			if (kb == '1') map->win_conditions ^= MAP_WIN_EXIT;
			if (kb == '2') map->win_conditions ^= MAP_WIN_KILL_GUARDIAN;
			if (kb == '3') map->win_conditions ^= MAP_WIN_KILL_ALL;

			if (kb == '4') map->boss_level ^= 1;
			if (kb == '5') get_string(bmp, map->name, 32, data[THE_FONT].dat, 8, 90, gui_fg_color, NULL);
		}

		if (key[KEY_F1]) {
			edit_mode = EDIT_MODE_DRAW;
			while(key[KEY_F1]);
		}
	}
}


