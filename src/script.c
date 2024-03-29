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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "port.h"
#include "script.h"
#include "timer.h"
#include "token.h"
#include "main.h"
#include "misc.h"
#include "sound.h"

#include "data.h"

// silly value 
#define		NO_CHANGE	-3249587

// array holding the sounds ids
int active_sounds[MAX_SCRIPT_SOUNDS];
// internal buffers
BITMAP *buffer;
BITMAP *swap_buffer;
// bool if script is still running or not
int script_done = 0;
// any objects
Tscript_object *objects = NULL;

// shows a speak bulb 
void draw_speak_bulb(BITMAP *bmp, int src_x, int src_y, int up, int left, int lines, char **rows, int arrow) {
	int i;
	int max_w = 0;
	int x1, y1, x2, y2;		// bulb coords
	int xa, ya;				// bulb_arrow position

	// get max text length
	for(i = 0; i < lines; i ++) {
		int len = text_length(rows[i]);
		if (max_w < len) max_w = len;
	}

	// calc bulb position
	x1 = MAX(src_x - max_w/2 - 8, 4);
	x2 = x1 + max_w + 8;
	if (x2 > 156) {
		x2 = 156;
		x1 = x2 - max_w - 16;
	}
	y2 = src_y - 10;
	y1 = src_y - 10 - lines * 9 - 8;
	xa = src_x - 4;
	ya = (up ? y1 - 10: src_y - 10);

	// draw the bulb
	rectfill(bmp, x1, y1 + 4, x2, y2 - 4, 4);
	rectfill(bmp, x1 + 4, y1, x2 - 4, y2, 4);
	line(bmp, x1, y1 + 4, x1, y2 - 4, 1);
	line(bmp, x1 + 4, y1, x2 - 4, y1, 1);
	line(bmp, x2, y1 + 4, x2, y2 - 4, 1);
	line(bmp, x1 + 4, y2, x2 - 4, y2, 1);
	line(bmp, x2+1, y1 + 4, x2+1, y2 - 4, 1);
	line(bmp, x1 + 4, y2+1, x2 - 4, y2+1, 1);
	draw_sprite(bmp, bitmaps[I_BULB_TL], x1, y1);
	draw_sprite(bmp, bitmaps[I_BULB_BL], x1, y2 - 5);
	draw_sprite(bmp, bitmaps[I_BULB_TR], x2 - 5, y1);
	draw_sprite(bmp, bitmaps[I_BULB_BR], x2 - 5, y2 - 5);
	if (arrow)
		draw_sprite(bmp, bitmaps[(left ? (up ? I_BULBA_TL : I_BULBA_BL) : (up ? I_BULBA_TR : I_BULBA_BR))], xa, ya);

	// draw text
	for(i = 0; i < lines; i ++) {
		textout_ex(bmp, rows[i], x1 + 4, y1 + 5 + i * 9, 1, -1);
	}
}

// waits for x millis or ESC
void esc_rest(int millis) {
	int clicks = millis / 20;
	int count = 0;
	while(count < clicks && !key[KEY_ESC]) {
		cycle_count = 0;
		update_platform_controls();
		count ++;
		while(!cycle_count)
			rest(20);
	}
	if (key[KEY_ESC]) script_done = -1;
}

// object handling

Tscript_object *get_object(char *name) {
	Tscript_object *o = objects;

	while(o != NULL) {
		if (!stricmp(name, o->name)) return o;
		o = (Tscript_object *)o->next;
	}
	return o;
}

Tscript_object *get_prev_object(char *name) {
	Tscript_object *o = objects;
	Tscript_object *prev = NULL;

	while(o != NULL) {
		if (!stricmp(name, o->name)) return prev;
		prev = o;
		o = (Tscript_object *)o->next;
	}

	return NULL;
}

int set_object(char *name, int x, int y, int dir) {
	Tscript_object *o;
	int new_object = 0;

	// search for object
	o = get_object(name);

	// create new if not found
	if (o == NULL) {
		o = malloc(sizeof(Tscript_object));
		if (o == NULL) return 0;
		new_object = 1;
		o->x = 0;
		o->y = 0;
		o->line = 0;
		o->frames = 0;
	}

	// set parameters
	o->x = (x == NO_CHANGE ? o->x : x);
	o->y = (y == NO_CHANGE ? o->y : y);
	o->dir = (dir == NO_CHANGE ? o->dir : dir);;
	o->vx = o->vy = 0;
	strcpy(o->name, name);

	// set bitmap
	if (!stricmp(name, "helicopter")) {
		o->bmp[0] = bitmaps[I_CHOPPER1];
		o->bmp[1] = bitmaps[I_CHOPPER2];
		o->bmp[2] = bitmaps[I_CHOPPER3];
		o->bmp[3] = bitmaps[I_CHOPPER4];
		o->frames = 4;
	}
	else if (!stricmp(name, "cage1")) o->bmp[0] = bitmaps[I_CAGE_SML1];
	else if (!stricmp(name, "cage2")) o->bmp[0] = bitmaps[I_CAGE_BIG];
	else if (!stricmp(name, "cage3")) o->bmp[0] = bitmaps[I_CAGE_SML2];
	else if (!stricmp(name, "chain")) o->bmp[0] = bitmaps[I_CHAIN];
	else if (!stricmp(name, "lola"))  o->bmp[0] = bitmaps[I_LOLA];
	else if (!stricmp(name, "alex"))  o->bmp[0] = bitmaps[I_ALEX];
	else if (!stricmp(name, "ufo"))   o->bmp[0] = bitmaps[I_UFO_BIG];
	else if (!stricmp(name, "ufo0"))  o->bmp[0] = bitmaps[I_UFO0];
	else if (!stricmp(name, "ufo1"))  o->bmp[0] = bitmaps[I_UFO1];
	else if (!stricmp(name, "beam"))  o->bmp[0] = bitmaps[I_UFOBEAM];
	else if (!stricmp(name, "ship0"))  o->bmp[0] = bitmaps[I_SHIP0];
	else if (!stricmp(name, "ship1"))  o->bmp[0] = bitmaps[I_SHIP1];
	else if (!stricmp(name, "ball"))  o->bmp[0] = bitmaps[I_HERO_BALL];
	else if (!strncmp(name, "drumcan", 7))  o->bmp[0] = bitmaps[I_DRUMCAN];
	else o->bmp[0] = NULL;

	// rearrange pointers
	if (new_object) {
		o->next = (struct Tscript_object *)objects;
		objects = o;
	}

	return 1;
}

int set_line(char *name, Tscript_object *src, int sx, int sy, Tscript_object *dst, int dx, int dy) {
	Tscript_object *o;
	int new_object = 0;

	// search for object
	o = get_object(name);

	// create new if not found
	if (o == NULL) {
		o = malloc(sizeof(Tscript_object));
		if (o == NULL) return 0;
		new_object = 1;
		o->line = 1;
	}

	// set parameters
	o->x = sx;
	o->y = sy;
	o->vx = dx;
	o->vy = dy;
	strcpy(o->name, name);

	// set bitmap
	o->bmp[0] = o->bmp[1] = o->bmp[2] = o->bmp[3] = NULL;

	// set line end point refs
	o->line_from = (struct Tscript_object *)src;
	o->line_to = (struct Tscript_object *)dst;

	// rearrange pointers
	if (new_object) {
		o->next = (struct Tscript_object *)objects;
		objects = o;
	}

	return 1;
}

int move_object(char *name, int vx, int vy) {
	Tscript_object *o;

	// search for object
	o = get_object(name);

	// exit if not found
	if (o == NULL) return 0;

	// set parameters
	o->vx = vx;
	o->vy = vy;

	return 1;
}

void delete_object(char *name) {
	Tscript_object *actual;
	Tscript_object *prev;

	actual = get_object(name);
	prev = get_prev_object(name);
	if (actual == NULL) return;

	if (prev == NULL) 
		objects = (Tscript_object *)actual->next;
	else 
		prev->next = actual->next;
	
	free(actual);
}

void delete_all_objects() {
	Tscript_object *o = objects;

	while(o != NULL) {
		o = (Tscript_object *)objects->next;
		free(objects);
		objects = o;
	}

	objects = NULL;
}

// end object handling

// script commands
// used by the script interpretor to launch various events

void cmd_showbmp(char *name) {
	if (!stricmp(name, "buffer")) {
		blit(buffer, swap_buffer, 0, 0, 0, 0, 160, 120);
	}
	else if (!stricmp(name, "treetops")) {
		blit(bitmaps[I_INTRO_BG], swap_buffer, 0, 0, 0, 0, 160, 120);
	}
	else if (!stricmp(name, "darkness")) {
		clear_to_color(swap_buffer, 2);
	}
}

void cmd_savebmp() {
	blit(swap_buffer, buffer, 0, 0, 0, 0, 160, 120);
}

void cmd_loadmap(char *fname) {
	new_game(0);
	new_level(fname, -2, 0);
}

void cmd_drawmap() {
	draw_frame(swap_buffer, 0);
}

void cmd_fadein() {
	fade_in_pal(swap_buffer, 100);
}

void cmd_fadeout() {
	fade_out_pal(swap_buffer, 100);
}

void cmd_speak(Ttoken *t, int arrow) {
	int sx, sy;
	int up = 0, left = 1;
	int lines;
	char *rows[8];
	int i;

	sx = atoi(t->word); t = (Ttoken *)t->next;
	sy = atoi(t->word); t = (Ttoken *)t->next;
	if (!stricmp(t->word, "right")) left = 0;
	else if (!stricmp(t->word, "up")) up = 1;
	else if (!stricmp(t->word, "upright")) { up = 0; left = 0; }
	else if (!stricmp(t->word, "rightup")) { up = 0; left = 0; }
	t = (Ttoken *)t->next;
	lines = MIN(atoi(t->word), 8);
	t = (Ttoken *)t->next;

	for(i = 0; i < lines; i ++) {
		rows[i] = t->word;
		t = (Ttoken *)t->next;
	}

	draw_speak_bulb(swap_buffer, sx, sy, up, left, lines, rows, arrow);
}

int remember_sound(int id) {
	int i = 0;
	while(active_sounds[i] != -1 && i < MAX_SCRIPT_SOUNDS) i ++;

	if (i == MAX_SCRIPT_SOUNDS) return 0;
	active_sounds[i] = id;
	return 1;
}

void forget_sound(int id) {
	int i;
	for(i = 0; i < MAX_SCRIPT_SOUNDS; i ++) {
		if (active_sounds[i] == id) {
			active_sounds[i] = -1;
			return;
		}
	}
}

int sample_to_id(char *sample) {
	int id;

	if (!stricmp(sample, "chopper")) id = S_CHOPPER;
	else if (!stricmp(sample, "talk")) id = S_TALK;
	else if (!stricmp(sample, "impact")) id = S_IMPACT;
	else if (!stricmp(sample, "crush")) id = S_CRUSH;
	else if (!stricmp(sample, "engine")) id = S_ENGINE;
	else if (!stricmp(sample, "energy")) id = S_ENERGY;
	else if (!stricmp(sample, "ship")) id = S_SHIP;
	else {
		printf("unknown sample \"%s\"", sample);
		return -1;
	}

	return id;
}

void cmd_play_sample(Ttoken *t) {
	int vol = 100;
	int loop = 0;
	int freq = 1000;
	int id;

	if (t != NULL) {
		Ttoken *tmp = (Ttoken *)t->next;
		if (tmp != NULL) {
			vol = atoi(tmp->word);
			tmp = (Ttoken *)tmp->next;
			if (tmp != NULL) {
				freq = atoi(tmp->word);
				tmp = (Ttoken *)tmp->next;
				if (tmp != NULL) {
					loop = atoi(tmp->word);
					tmp = (Ttoken *)tmp->next;
				}
			}
		}

		id = sample_to_id(t->word);
		if (id > 0) {
			if (remember_sound(id)) play_sound_id_ex(id, vol, freq, loop);
			else log2file("*** cannot play sample %d, buffer full", id);
		}
	}
}

void cmd_stop_sample(Ttoken *t) {
	if (t != NULL) {
		int id = sample_to_id(t->word);
		if(id > 0) {
			stop_sound_id(id);
			forget_sound(id);
		}
	}
}

void cmd_blit() {
	blit_to_screen(swap_buffer);
}

void cmd_wait(int msec) {
	esc_rest(msec);
}

void cmd_end() {
	script_done = 1;
}

void cmd_set(Ttoken *t) {
	char *name;
	int x;
	int y;
	int dir = NO_CHANGE;

	name = t->word;    
	t = (Ttoken *)t->next;

	if (!strncmp(name, "line", 4)) {
		// special handling of lines
		Tscript_object *s, *d;
		int sx, sy, dx, dy;

		if (t == NULL) return;
		s = get_object(t->word);
		if (s == NULL) return;

		t = (Ttoken *)t->next; if (t == NULL) return;
		sx = atoi(t->word);
		t = (Ttoken *)t->next; if (t == NULL) return;
		sy = atoi(t->word);

		t = (Ttoken *)t->next; if (t == NULL) return;
		d = get_object(t->word);
		if (d == NULL) return;

		t = (Ttoken *)t->next; if (t == NULL) return;
		dx = atoi(t->word);
		t = (Ttoken *)t->next; if (t == NULL) return;
		dy = atoi(t->word);

		set_line(name, s, sx, sy, d, dx, dy);
	}
	else {
		if (t->word[0] == '*') x = NO_CHANGE;
		else x = atoi(t->word); 
		t = (Ttoken *)t->next;
		
		if (t->word[0] == '*') y = NO_CHANGE;
		else y = atoi(t->word); 
		t = (Ttoken *)t->next;
		
		if (t != NULL) {
			if (!stricmp(t->word, "right")) dir = 1;
			else if (!stricmp(t->word, "rotate")) dir = 2;
			else dir = 0;
		}
		
		set_object(name, x, y, dir);
	}
}

void cmd_move(Ttoken *t) {
	char *name;
	int vx;
	int vy;

	name = t->word;    
	t = (Ttoken *)t->next;

	vx = atoi(t->word); 
	t = (Ttoken *)t->next;

	vy = atoi(t->word); 

	move_object(name, vx, vy);
}

void cmd_del(Ttoken *t) {
	delete_object(t->word);
}

void cmd_run(Ttoken *t) {
	Tscript_object *o;
	int loops;
	static int i = 0;

	loops = atoi(t->word);

	cycle_count = 0;
	while(loops && !script_done) {

		// let other processes play
		while(cycle_count == 0)
			rest(1);

		while(cycle_count > 0 && loops && !script_done) {
			logic_count ++;

			update_platform_controls();

			// update objects
			o = objects;
			while(o != NULL) {
				if (!o->line) {
					o->x += o->vx;
					o->y += o->vy;
				}
				o = (Tscript_object *) o->next;
			}

			if (key[KEY_ESC]) script_done = -1;

			i ++;
			loops --;
			cycle_count --;
		}

		// blit buffer to swap buffer
		blit(buffer, swap_buffer, 0, 0, 0, 0, 160, 120);
		
		// draw objects
		o = objects;
		while(o != NULL) {
			if (o->bmp[0] != NULL) {
				int frame = (o->frames ? logic_count % o->frames : 0);
				if (o->dir == 1)
					draw_sprite_h_flip(swap_buffer, o->bmp[frame], o->x, o->y);
				else if (o->dir == 0)
					draw_sprite(swap_buffer, o->bmp[frame], o->x, o->y);
				else // rotate
					rotate_sprite(swap_buffer, o->bmp[frame], o->x, o->y, itofix(-8*i));
			}
			else if (o->line) {
				line(swap_buffer, ((Tscript_object *)o->line_from)->x + o->x,
					((Tscript_object *)o->line_from)->y + o->y,
					((Tscript_object *)o->line_to)->x + o->vx,
					((Tscript_object *)o->line_to)->y + o->vy,
					1);
			}
			o = (Tscript_object *) o->next;
		}
		
		// blit to screen
		blit_to_screen(swap_buffer);
	}
	
}

// end commands

// helper to get a line from a string
char *get_line(char *dst, char *src) {
	int go = 1;
	while(go) {
		if (*src == '\n' || *src == '\r' || *src == '\0' )
			go = 0;

		*dst = *src;
		dst ++; src ++;
	}
	*dst = '\0';

	return src;
}

// runs a script
// returns -1 is esc was pressed, 0 o/w
int run_script(int id) {
	char buf[512];
	Ttoken *token;
	int i;

	char *script = (char*)scripts[id].dat;

	// init sound memory
	for(i = 0; i < MAX_SCRIPT_SOUNDS; i ++) active_sounds[i] = -1;

	// create gfx buffers
	swap_buffer = create_bitmap(160, 120);
	buffer = create_bitmap(160, 120);

	script_done = FALSE;
	
	while(!script_done) {
	  
		// get commands from script string
		script = get_line(buf, script);
		
		if (buf[0] != '#' && buf[0] != '\n' && buf[0] != '\r' && buf[0] != '-') {
			token = tokenize(buf);	
			if (token != NULL) {
				if      (!stricmp(token->word, "load_map"))		cmd_loadmap(get_next_word(token));
				else if (!stricmp(token->word, "draw_map"))		cmd_drawmap();
				else if (!stricmp(token->word, "set"))			cmd_set((Ttoken *)token->next);
				else if (!stricmp(token->word, "move"))			cmd_move((Ttoken *)token->next);
				else if (!stricmp(token->word, "delete"))		cmd_del((Ttoken *)token->next);
				else if (!stricmp(token->word, "run"))			cmd_run((Ttoken *)token->next);
				else if (!stricmp(token->word, "speak"))		cmd_speak((Ttoken *)token->next, 1);
				else if (!stricmp(token->word, "text"))			cmd_speak((Ttoken *)token->next, 0);
				else if (!stricmp(token->word, "save_buffer"))	cmd_savebmp();
				else if (!stricmp(token->word, "show_bmp"))		cmd_showbmp(get_next_word(token));
				else if (!stricmp(token->word, "blit"))			cmd_blit();
				else if (!stricmp(token->word, "fade_in"))		cmd_fadein();
				else if (!stricmp(token->word, "fade_out"))		cmd_fadeout();
				else if (!stricmp(token->word, "wait"))			cmd_wait(atoi(get_next_word(token)));
				else if (!stricmp(token->word, "play_sample"))  cmd_play_sample((Ttoken *)token->next);
				else if (!stricmp(token->word, "stop_sample"))  cmd_stop_sample((Ttoken *)token->next);
				else if (!stricmp(token->word, "end_script"))	cmd_end();
				else {
					char msg[256];
					sprintf(msg, "unknown: %s", token->word);
					msg_box(msg);
				}
				
				flush_tokens(token);
			}
		}
	}

	if (script_done != -1)
		fade_out_pal(swap_buffer, 100);

	// destroy buffers
	delete_all_objects();
	destroy_bitmap(buffer);
	destroy_bitmap(swap_buffer);

	// stop old sounds
	for(i = 0; i < MAX_SCRIPT_SOUNDS; i ++) {
		if (active_sounds[i] != -1) {
			stop_sound_id(active_sounds[i]);
			forget_sound(active_sounds[i]);
		}
	}

	return (script_done == -1 ? -1 : 0);
}
