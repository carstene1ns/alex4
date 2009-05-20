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
#include <aldumb.h>
#include <string.h>
#include <ctype.h>

#include "timer.h"
#include "map.h"
#include "control.h"
#include "actor.h"
#include "player.h"
#include "particle.h"
#include "bullet.h"
#include "scroller.h"
#include "options.h"
#include "hisc.h"
#include "script.h"
#include "main.h"
#include "edit.h"
#include "shooter.h"

#include "../data/data.h"


// some game status defines
#define GS_OK				1
#define GS_GAMEOVER			2
#define GS_QUIT_GAME		3
#define GS_QUIT_MENU		4
#define GS_PLAY				5
#define GS_LEVEL_DONE		6
#define GS_GAME_COMPLETE	7
#define GS_GAME_DIED		8
#define GS_SCORES			9
#define GS_EDIT			   10

int game_status = 0;


// globalez
DATAFILE *data = NULL;
DATAFILE *maps = NULL;
DATAFILE *sfx_data = NULL;
BITMAP *swap_screen;
PALETTE org_pal;
Tscroller hscroll;
Thisc *hisc_table;
Thisc *hisc_table_space;
char working_directory[1024];

// the map
Tmap *map = NULL;

// controls
Tcontrol ctrl;

// actors
Tactor actor[MAX_ACTORS];

// edit
int editing = 0;

// music and stuff
static AL_DUH_PLAYER *dp = NULL;
static DUH_SIGRENDERER *sr = NULL;
static DUH *duh = NULL;

// mod start patterns
#define MOD_INTRO_SONG		 0x00
#define MOD_MENU_SONG		 0x08
#define MOD_BOSS_SONG		 0x0c
#define MOD_OUTRO_SONG		 0x11
#define MOD_PLAYER_DIES		 0x1a
#define MOD_LEVEL_DONE		 0x1b
#define MOD_GAME_OVER		 0x1c
#define MOD_LEVEL_SONG		 0x1d


// sound fx
#define MAX_SOUNDS		32
SAMPLE *sfx[MAX_SOUNDS] = { NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL,
							NULL, NULL, NULL, NULL };




// various
char scroller_message[] = 
	"Free Lunch Design      presents      Alex the Allegator 4      "
	"Guide Alex to the exit of each level      Jump or shoot enemies picking up stars and cherries on the way      "
	"Use arrows to move Alex, ALT to jump and Left CTRL to shoot, or use a gamepad or joystick      "
	"Code and GFX by Johan Peitz      Music and SFX by Anders Svensson       ";
char init_string[] = "[YhJPJKUSY`0-'\"7 ";

char *level_files[256];
int num_levels;
int got_sound = 0;
Toptions options;
int menu_choice = 1;
int playing_original_game = 1;
int init_ok = 0;



// // // // // // // // // // // // // // // // // // // // // 

// returns pointer to the game over bitmap
BITMAP *get_gameover_sign() {
	return data[GAME_OVER].dat;
}

// returns pointer to the lets go bitmap
BITMAP *get_letsgo_sign() {
	return data[LETSGO].dat;
}

// returns pointer to the space highscore table
Thisc *get_space_hisc() {
	return hisc_table_space;
}

// returns the init_string
char *get_init_string() {
	return init_string;
}


// loggs the text to the text file
void log2file(char *format, ...) {
	va_list ptr; /* get an arg pointer */
 	FILE *fp;
	
	fp = fopen("log.txt", "at");
	if (fp) {
		/* initialize ptr to point to the first argument after the format string */
		va_start(ptr, format);
 
		/* Write to logfile. */
		vfprintf(fp, format, ptr); // Write passed text.
		fprintf(fp, "\n"); // New line..
 
		va_end(ptr);
 
		fclose(fp);
	}

}


// saves a screenshot
void take_screenshot(BITMAP *bmp) { 
	static int number = 0;
	PALETTE p;
	BITMAP *b;
	char buf[256];
	int ok = 0;

	// check if the file name allready exists
	do {
		sprintf(buf, "a4_%03d.pcx", number ++);
		if (!exists(buf)) ok = 1;
		if (number > 999) return;
	} while(!ok);

	get_palette(p);

	b = create_sub_bitmap(bmp, 0, 0, bmp->w, bmp->h);
	save_bitmap(buf, b, p);
	destroy_bitmap(b);

}


// garbles a string using a key
void garble_string(char *str, int key) {
	int i;
	int len_i = strlen(str);
	for(i = 0; i < len_i; i ++) {
		str[i] ^= (key+i);
	}
}

// sets the current map
void set_map(Tmap *m) {
	map = m;
}

// plays a sample using default settings
void play_sound(SAMPLE *s) {
	if (got_sound && s != NULL) play_sample(s, get_config_int("sound", "sample_volume", 100), 128, 1000, 0); 
}

// plays a sample using default settings
// (from an index)
void play_sound_id(int id) {
	if (got_sound) play_sound(sfx[id]);
}

// plays a sample using user settings
void play_sound_ex(SAMPLE *s, int vol, int freq, int loop) {
	if (got_sound && s != NULL) {
		int v = (get_config_int("sound", "sample_volume", 100) * (float)(vol))/100.0;
		play_sample(s, v, 128, freq, loop); 
	}
}

// plays a sample using user settings
// (from an index)
void play_sound_id_ex(int id, int vol, int freq, int loop) {
	play_sound_ex(sfx[id], vol, freq, loop);
}

// stops a sample (providing an id)
void stop_sound_id(int id) {
	stop_sample(sfx[id]);
}

// adjusts a sample (from an index) according to player position
void adjust_sound_id_ex(int id, int x) {
	if (got_sound) {
		int vol = MAX(100 - ABS(player.actor->x - x) / 2, 0);
		int v = get_config_int("sound", "sample_volume", 100) * (float)(vol)/100.0;
		int pan = MID(0, 128 + x - player.actor->x, 255);
		if (sfx[id] != NULL) {
			adjust_sample(sfx[id], v, pan, 1000, 1);
		}
	}
}


// shows a little message
void msg_box(char *str) {
	if (got_sound) al_pause_duh(dp);
	alert("Alex 4: Message", NULL, str, "OK", NULL, 0, 0);
	if (got_sound) al_resume_duh(dp);
}

// polls the music
void poll_music() {
	if (got_sound && dp != NULL) al_poll_duh(dp);
}


// waits for user to strike a key, or x seconds
void wait_key(int seconds) {
	int t = 0;
	int kp = 0;

	clear_keybuf();
	cycle_count = 0;
	while(!kp && t < seconds * 50) {
		cycle_count = 0;
		poll_control(&ctrl);
		t ++;
		if (keypressed()) kp = 1;
		if (is_fire(&ctrl) || is_jump(&ctrl)) kp = 1;
		if (got_sound && dp != NULL) al_poll_duh(dp);
		while(!cycle_count);
	}
}


// returns the player actor
Tactor *get_alex() {
	return &actor[0];
}

// stops any mod playing
static void stop_music(void) {
	al_stop_duh(dp);
	dp = NULL;
}


// starts the mod at position x
static void start_music(int startorder) {
	const int n_channels = 2; /* stereo */

	stop_music();

	{
		sr = dumb_it_start_at_order(duh, n_channels, startorder);
		dp = al_duh_encapsulate_sigrenderer(sr,
			((float)(get_config_int("sound", "music_volume", 255)) / 255.0),
			get_config_int("dumb", "buffer_size", 4096),
			get_config_int("dumb", "sound_freq", 44100));
		if (!dp) duh_end_sigrenderer(sr); // howto.txt doesn't mention that this is necessary! dumb.txt does ...
	}
}


// delay routine used by the fades
void fade_rest(int msec, AL_DUH_PLAYER *duh_player) {
	int i = 0;

	while(i < msec / 20) {
		cycle_count = 0;
		if (got_sound && duh_player != NULL) al_poll_duh(duh_player);
		i ++;
		while(!cycle_count)	yield_timeslice();
	}
}


// fades in from white to 4 color palette
void fade_in_pal(int delay) {
	set_color(1, &org_pal[3]);	
	fade_rest(delay, dp);

	set_color(1, &org_pal[2]);	
	set_color(2, &org_pal[3]);	
	fade_rest(delay, dp);

	set_color(1, &org_pal[1]);	
	set_color(2, &org_pal[2]);	
	set_color(3, &org_pal[3]);	
	fade_rest(delay, dp);
}


// fades 4 color palette to white
void fade_out_pal(int delay) {
	set_color(1, &org_pal[2]);	
	set_color(2, &org_pal[3]);	
	set_color(3, &org_pal[4]);	
	fade_rest(delay, dp);
	set_color(1, &org_pal[3]);	
	set_color(2, &org_pal[4]);	
	fade_rest(delay, dp);
	set_color(1, &org_pal[4]);	
	fade_rest(delay, dp);
}



// fade in from black to 4 colors pal
void fade_in_pal_black(int delay, AL_DUH_PLAYER *duh_player) {
	set_color(4, &org_pal[2]);	
	fade_rest(delay, duh_player);

	set_color(3, &org_pal[2]);	
	set_color(4, &org_pal[3]);	
	fade_rest(delay, duh_player);

	set_color(2, &org_pal[2]);	
	set_color(3, &org_pal[3]);	
	set_color(4, &org_pal[4]);	
	fade_rest(delay, duh_player);
}


// fades 4 color palette to black
void fade_out_pal_black(int delay, AL_DUH_PLAYER *duh_player) {
	set_color(2, &org_pal[1]);	
	set_color(3, &org_pal[2]);	
	set_color(4, &org_pal[3]);	
	fade_rest(delay, duh_player);
	set_color(3, &org_pal[1]);	
	set_color(4, &org_pal[2]);	
	fade_rest(delay, duh_player);
	set_color(4, &org_pal[1]);	
	fade_rest(delay, duh_player);
}



// ets all color to white
void wipe_pal() {
	set_color(1, &org_pal[4]);	
	set_color(2, &org_pal[4]);	
	set_color(3, &org_pal[4]);	
	set_color(4, &org_pal[4]);	
}

// removes trailing white space from a null terminated string
void clear_trailing_whitespace(char *data) {
	unsigned int i;

	for(i = 0; i < strlen(data); i++) {
		if (data[i] == ' ' || data[i] == '\t' || data[i] == '\n' || data[i] == '\r')
			data[i] = 0;
	}
}


/// load the level filenames
void load_level_files(const char *filename) {
	FILE *fp;
	char buf[1024];
	int mode = 0;
	char *ret;
	char path[1024];
	
	// get path to maps
	replace_filename(path, filename, "", 1024);

	// reset counters
	num_levels = 0;

	// open level file
	fp = fopen(filename, "rt");
	if (!fp) {
		allegro_message("%s not found", filename);
		log2file("  *** %s not found", filename);
		return;
	}

	// start parsing
    ret = fgets(buf, 1024, fp);  // read a line
	while(ret != NULL && mode == 0) {
		// read lines until #start# token is found
		if (!strncmp("#start#", buf, 7)) { 
			mode = 1;
		}
	    ret = fgets(buf, 1024, fp);  // read a line
	}
	if (!mode) {
		mode = 2;
		allegro_message("#start# not found");
		log2file("  *** #start# not found");
	}
		
	// read level lines until #end#
	while(ret != NULL && mode == 1) {
		// read levels until #end# token is found
		if (!strncmp("#end#", buf, 5)) {
			mode = 2;
		}
		else {
			char full_path_to_map[1024];

			clear_trailing_whitespace(buf);
			sprintf(full_path_to_map, "%s%s", path, buf);
			level_files[num_levels] = strdup(full_path_to_map);

			// test if map exists
			if (exists(level_files[num_levels])) {
				num_levels ++;
				log2file("  <%s> found", buf);
			}
			else {
				allegro_message("ALEX4:\n<%s> not found - skipping", level_files[num_levels]);
				log2file("  *** <%s> not found - skipping", level_files[num_levels]);
			}
		}

		ret = fgets(buf, 1024, fp);  // read a line
	}

	// close file
	fclose(fp);

	return;
}




// blits anything to screen
void blit_to_screen(BITMAP *bmp) {
	acquire_screen();
	if (options.use_vsync) vsync();
	stretch_blit(bmp, screen, 0, 0, bmp->w, bmp->h, 0, 0, SCREEN_W, SCREEN_H);
	release_screen();
}


// draws the status bar
void draw_status_bar(BITMAP *bmp, int y) {
	int i;

	rectfill(bmp, 0, y, 159, y+9, 1);
	draw_sprite_h_flip(bmp, data[HERO_NORM].dat, 0, y+1); 
	textprintf(bmp, data[THE_FONT].dat, 9, y+1, 4, " :%d", player.lives);

	for(i = 0; i < player.health; i ++)
		draw_sprite(bmp, data[HEART2].dat, 40 + 10 * i, y-3);

	draw_sprite(bmp, data[EGG].dat, 80, y-5); 
	textprintf(bmp, data[THE_FONT].dat, 85, y+1, 4, " :%d", player.ammo);

	textprintf_right(bmp, data[THE_FONT].dat, 158, y+1, 4, "%d", player.score);
}


// draws a frame of the action
void draw_frame(BITMAP *bmp, int _status_bar) {
	int x0, x1, x2;
	int ox = map->offset_x;
	int i;

	// calc bg pos
	x0 = (-ox % 1280) >> 3;
	x1 = (-ox % 640) >> 2;
	x2 = (-ox % 320) >> 1;

	// draw backgrounds
	blit(data[BG0].dat, bmp, 0, 0, x0, 0, 160, 120);
	blit(data[BG0].dat, bmp, 0, 0, x0 + 160, 0, 160, 120);
	draw_sprite(bmp, data[BG1].dat, x1, 120 - ((BITMAP *)data[BG1].dat)->h);
	draw_sprite(bmp, data[BG1].dat, x1 + 160, 120 - ((BITMAP *)data[BG1].dat)->h);
	draw_sprite(bmp, data[BG2].dat, x2, 120 - ((BITMAP *)data[BG2].dat)->h);
	draw_sprite(bmp, data[BG2].dat, x2 + 160, 120 - ((BITMAP *)data[BG2].dat)->h);

	// draw frame
	draw_map(bmp, map, 0, 0, 160, 120, editing);

	if (!editing) {
		// draw actors
		for(i = 1; i < MAX_ACTORS; i ++)
			if (actor[i].active)
				draw_actor(bmp, &actor[i], actor[i].x - ox, actor[i].y);
			
		// draw particles
		for(i = 0; i < MAX_PARTICLES; i ++) 
			if (particle[i].life)
				draw_particle(bmp, &particle[i], ox, 0);
	}

	// draw player
	draw_player(bmp, &player, player.actor->x - ox, player.actor->y);
	
	// draw bullets
	for(i = 0; i < MAX_BULLETS; i ++) 
		if (bullet[i].exist)
			draw_bullet(bmp, &bullet[i], ox, 0);

	// draw statusbar
	if (!editing) {
		if (_status_bar)	draw_status_bar(bmp, 110);
		else rectfill(bmp, 0, 110, 159, 119, 1);

	}
	else { 		/////////////// EDIT stats
		int mx = mouse_x / (SCREEN_W / 160);
		int my = mouse_y / (SCREEN_H / 120);
		draw_edit_mode(bmp, map, mx, my);
	}
}


// loads a sample from disk
SAMPLE *load_path_sample(char *fname) {
	char buf[1024];
	SAMPLE *s;
	sprintf(buf, "%s/%s", get_config_string("sound", "sfx_path", "sfx"), fname);
	s = load_sample(buf);
	if (s == NULL) {
		sprintf(buf, "not found: %s/%s", get_config_string("sound", "sfx_path", "sfx"), fname);
		log2file(" *** %s", buf);
		msg_box(buf);
	}
	return s;
}


// counts number of maps
// invoked when loading the map datafile
void count_maps_callback(DATAFILE *d) {
	num_levels ++;
}


// invoked when game looses focus
void display_switch_out(void) {
	if (got_sound) {
		al_pause_duh(dp);
		rest(100);
	}
}

// invoked when game regains focus
void display_switch_in(void) {
	if (got_sound) al_resume_duh(dp);
}

// sets up the gui colors
void fix_gui_colors() {
	((RGB *)data[0].dat)[255].r = 0;
	((RGB *)data[0].dat)[255].g = 0;
	((RGB *)data[0].dat)[255].b = 0;
	((RGB *)data[0].dat)[254].r = 63;
	((RGB *)data[0].dat)[254].g = 63;
	((RGB *)data[0].dat)[254].b = 63;
	gui_fg_color = 255;
	gui_bg_color = 254;
}

// init the game
int init_game(const char *map_file) {
	PACKFILE *pf;
	BITMAP *bmp;
	int i;
	int w, h;

	log2file("\nInit routines:");

	// various allegro things
	log2file(" initializing allegro");
	text_mode(-1);
	garble_string(init_string, 53);
	set_config_file("alex4.ini");
	set_window_close_button(FALSE);
	
	// install timers
	log2file(" installing timers");
	install_timers();

	// set color depth to 8bpp
	log2file(" setting color depth (8)");
	set_color_depth(8);
	
	// allocating memory 
	log2file(" allocating memory for off screen buffers");
	swap_screen = create_bitmap(160, 120);
	if (swap_screen == NULL) {
		log2file("  *** failed");
		allegro_message("ALEX4:\nFailed to allocate memory for swap screen.");
		return FALSE;
	}

	log2file(" allocating memory for high score table 1");
	hisc_table = make_hisc_table();
	if (hisc_table == NULL) {
		log2file("  *** failed");
		allegro_message("ALEX4:\nFailed to allocate memory for high score list.");
		return FALSE;
	}

	log2file(" allocating memory for high score table 2");
	hisc_table_space = make_hisc_table();
	if (hisc_table_space == NULL) {
		log2file("  *** failed");
		allegro_message("ALEX4:\nFailed to allocate memory for high score list.");
		return FALSE;
	}
	

	// init gfx
	if (get_config_int("graphics", "fullscreen", 0)) {
		w = get_config_int("graphics", "f_width", 640);
		h = get_config_int("graphics", "f_height", 480);
	}
	else {
		w = get_config_int("graphics", "w_width", 640);
		h = get_config_int("graphics", "w_height", 480);
	}

	log2file(" entering gfx mode set in alex4.ini (%dx%d %s)", w, h, (get_config_int("graphics", "fullscreen", 0) ? "full" : "win"));

    if (set_gfx_mode(
		(get_config_int("graphics", "fullscreen", 0) ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED),
		w, h, 0, 0)) {
		log2file("  *** failed");
		log2file(" entering gfx mode (640x480 windowed)");
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
			log2file("  *** failed");
			log2file(" entering gfx mode (640x480 fullscreen)");
			if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0)) {
				log2file("  *** failed");
				allegro_message("ALEX4:\nFailed to enter gfx mode.\nTry setting a custom resolution in alex4.ini.");
				return FALSE;
			}
		}
	}

	// show initial loading screen
	clear(swap_screen);
	textout_centre(swap_screen, font, "loading...", 320, 200, 1);
	blit_to_screen(swap_screen);

	// set switch modes and callbacks
	if (set_display_switch_mode(SWITCH_PAUSE) < 0)
		log2file("  * display switch mode failed");
	if (set_display_switch_callback(SWITCH_IN, display_switch_in) < 0)
		log2file("  * display switch in failed");
	if (set_display_switch_callback(SWITCH_OUT, display_switch_out) < 0)
		log2file("  * display switch out failed");


	// set win title (no! really???)
	log2file(" setting window title");
	set_window_title("Alex 4");

	// register dumb
	log2file(" registering dumb");
	dumb_register_packfiles();
    dumb_register_dat_mod(DUMB_DAT_MOD);
	dumb_resampling_quality = get_config_int("dumb", "dumb_resampling_quality", 4);
	dumb_it_max_to_mix = get_config_int("dumb", "dumb_it_max_to_mix", 128);

	// load data
	log2file(" loading data");
	packfile_password(init_string);
	data = load_datafile("data/data.dat");
	packfile_password(NULL);
	if (data == NULL) {
    	log2file("  *** failed");
		allegro_message("ALEX4:\nFailed to load data.");
		return FALSE;
	}

	// load options
	log2file(" loading options");
	pf = pack_fopen("alex4.sav", "rp");
	if (pf) {
		load_options(&options, pf);
		pack_fclose(pf);
	}
	else { 
	    log2file("  *** failed, resetting to defaults");
		reset_options(&options);
	}

	// loading highscores
	log2file(" loading hiscores");
	pf = pack_fopen("alex4.hi", "rp");
	if (pf) {
		load_hisc_table(hisc_table, pf);
		load_hisc_table(hisc_table_space, pf);
		pack_fclose(pf);
	}
	else {
	    log2file("  *** failed, resetting");
		reset_hisc_table(hisc_table, "alex", 25000, 5000);
		sort_hisc_table(hisc_table);
		reset_hisc_table(hisc_table_space, "Lola", 3000000, 600000);
		sort_hisc_table(hisc_table_space);
	}

	// fix some palette entries
	((RGB *)data[0].dat)[0].r = 0;
	((RGB *)data[0].dat)[0].g = 0;
	((RGB *)data[0].dat)[0].b = 0;
	fix_gui_colors();
	set_palette(data[0].dat);

	// show splash screen
	clear_to_color(swap_screen, 3);

	bmp = data[FLD_LOGO].dat;
	draw_character(swap_screen, bmp, 80 - bmp->w / 2 + 0, 50 + 1, 1);
	draw_character(swap_screen, bmp, 80 - bmp->w / 2, 50, 4);
	
	blit_to_screen(swap_screen);

	
	// load maps
	if (playing_original_game) {
		log2file(" loading original maps");
		packfile_password(init_string);
		num_levels = -1;  // skip end object when counting
		maps = load_datafile_callback("data/maps.dat", count_maps_callback);
		packfile_password(NULL);
		if (maps == NULL) {
	    	log2file("  *** failed");
			allegro_message("ALEX4:\nFailed to load original maps.");
			return FALSE;
		}
		log2file(" loaded %d maps", num_levels);
	}
	else {
		log2file(" loading custom maps");
		log2file("  reading map file: %s", map_file);
		if (exists(map_file)) {
			// load level file names
			load_level_files(map_file);
			log2file("  %d maps loaded", num_levels);
			if (num_levels == 0) {
				log2file("  *** no maps were loaded");
				allegro_message("ALEX4:\nCustom map file must\nhold at least one\nlegal map file.");
				return FALSE;
			}
		}
		else {
			log2file("  *** file not found: %s", map_file);
			allegro_message("ALEX4:\nCustom map file not found:\n%s", map_file);
			return FALSE;
		}		
	}


	// install some parts of allegro
	log2file(" installing keyboard");
	install_keyboard();
	log2file(" installing mouse");
	install_mouse();

	// fix palette
	for(i = 0; i < 256; i ++) {
		org_pal[i].r = ((RGB *)data[0].dat)[i].r;
		org_pal[i].g = ((RGB *)data[0].dat)[i].g;
		org_pal[i].b = ((RGB *)data[0].dat)[i].b;
	}

	// init control
	log2file(" initializing controls");
	init_control(&ctrl);

	// initializing joystick
	log2file(" initializing joystick/gamepad");
	if (install_joystick(JOY_TYPE_AUTODETECT)) {
    	log2file("  *** failed");
	}
	else {
		ctrl.use_joy = 1;
	}

	// install sound
	log2file(" installing sound");
   	set_volume_per_voice(0);
	switch(get_config_int("sound", "sound_device", 0)) {
		case 1:
			i = DIGI_AUTODETECT;
			log2file("  DIGI_AUTODETECT selected (%d)", i);
			break;
		case 2:
			i = DIGI_DIRECTX(0);
			log2file("  DIGI_DIRECTX(0) selected (%d)", i);
			break;
		case 3:
			i = DIGI_DIRECTAMX(0);
			log2file("  DIGI_DIRECTAMX(0) selected (%d)", i);
			break;
		default:
			i = -770405;	// dummy number
			got_sound = 0;
			log2file("  NO_SOUND selected");
			break;
	}
	if (i != -770405) {
		if (install_sound(i, MIDI_NONE, NULL)) {
			log2file("  *** failed");
			got_sound = 0;
		}
		else got_sound = 1;
	}

	if (got_sound) {
		int s = 0;
		// load music
        duh = (DUH *)data[MSC_GAME].dat;
		
		if (get_config_int("sound", "use_sound_datafile", 1)) {
			log2file(" loading sound datafile");
			packfile_password(init_string);
			sfx_data = load_datafile("data/sfx_44.dat");
			if (sfx_data == NULL) {
				sfx_data = load_datafile("data/sfx_22.dat");
				log2file("  sfx_44.dat not found");
				s = 0;
			}
			else {
				s = 1;
				log2file("  sfx_44.dat will be used");
			}
			packfile_password(NULL);
			if (sfx_data == NULL) {
				log2file("  sfx_22.dat not found");
				log2file("  *** failed - no sfx will be available");
			}
			else {
				if (!s) log2file("  sfx_22.dat will be used");

				// assign samples
				sfx[SMPL_CHERRY]	 = sfx_data[0].dat;
				sfx[SMPL_CHOPPER]	 = sfx_data[1].dat;
				sfx[SMPL_CRUSH]		 = sfx_data[2].dat;
				sfx[SMPL_A_DIE]		 = sfx_data[3].dat;
				sfx[SMPL_EAT]		 = sfx_data[4].dat;
				sfx[SMPL_BEAM]		 = sfx_data[5].dat;
				sfx[SMPL_ENGINE]	 = sfx_data[6].dat;
				sfx[SMPL_HEART]		 = sfx_data[7].dat;
				sfx[SMPL_HIT]		 = sfx_data[8].dat;
				sfx[SMPL_HURT]		 = sfx_data[9].dat;
				sfx[SMPL_CRUSH_LAND] = sfx_data[10].dat;
				sfx[SMPL_JUMP]		 = sfx_data[11].dat;
				sfx[SMPL_E_DIE]		 = sfx_data[12].dat;
				sfx[SMPL_MENU]		 = sfx_data[13].dat;
				sfx[SMPL_PAUSE]		 = sfx_data[14].dat;
				sfx[SMPL_POINT]		 = sfx_data[15].dat;
				sfx[SMPL_SHIP]		 = sfx_data[16].dat;
				sfx[SMPL_SHOOT]		 = sfx_data[17].dat;
				sfx[SMPL_SPIT]		 = sfx_data[18].dat;
				sfx[SMPL_STAR]		 = sfx_data[19].dat;
				sfx[SMPL_STARTUP]	 = sfx_data[20].dat;
				sfx[SMPL_MASH]		 = sfx_data[21].dat;
				sfx[SMPL_TALK]		 = sfx_data[22].dat;
				sfx[SMPL_SPIN]		 = sfx_data[23].dat;
				sfx[SMPL_XTRALIFE]	 = sfx_data[24].dat;
			}
		}	
		else {
			// load sounds from disk
			log2file(" loading sounds from %s", get_config_string("sound", "sfx_path", ""));
			sfx[SMPL_STARTUP]	 = load_path_sample("startup.wav");
			sfx[SMPL_POINT]		 = load_path_sample("point.wav");
			sfx[SMPL_JUMP]		 = load_path_sample("jump.wav");
			sfx[SMPL_MASH]		 = load_path_sample("stomp.wav");
			sfx[SMPL_EAT]		 = load_path_sample("eat.wav");
			sfx[SMPL_SPIT]		 = load_path_sample("spit.wav");
			sfx[SMPL_A_DIE]		 = load_path_sample("die.wav");
			sfx[SMPL_HIT]		 = load_path_sample("hit.wav");
			sfx[SMPL_CRUSH]		 = load_path_sample("crush.wav");
			sfx[SMPL_E_DIE]		 = load_path_sample("kill.wav");
			sfx[SMPL_HEART]		 = load_path_sample("heart.wav");
			sfx[SMPL_HURT]		 = load_path_sample("hurt.wav");
			sfx[SMPL_XTRALIFE]	 = load_path_sample("xtralife.wav");
			sfx[SMPL_CHERRY]	 = load_path_sample("cherry.wav");
			sfx[SMPL_MENU]		 = load_path_sample("menu.wav");
			sfx[SMPL_SHOOT]		 = load_path_sample("shoot.wav");
			sfx[SMPL_SPIN]		 = load_path_sample("turn.wav");
			sfx[SMPL_STAR]		 = load_path_sample("star.wav");
			sfx[SMPL_CRUSH_LAND] = load_path_sample("impact.wav");
			sfx[SMPL_PAUSE]		 = load_path_sample("pause.wav");
			sfx[SMPL_ENGINE]	 = load_path_sample("engine.wav");
			sfx[SMPL_CHOPPER]	 = load_path_sample("chopper.wav");
			sfx[SMPL_SHIP]		 = load_path_sample("ship.wav");
			sfx[SMPL_BEAM]		 = load_path_sample("energy.wav");
			sfx[SMPL_TALK]		 = load_path_sample("alex.wav");
		}
	}


	// misc
	log2file(" initializing scroller");
	init_scroller(&hscroll, data[THE_FONT].dat, scroller_message, 160, 10, TRUE);

	options.use_vsync = get_config_int("graphics", "vsync", 0);
	log2file(" vsync set to %s", (options.use_vsync ? "ON" : "OFF"));

	// done!
	play_sound(sfx[SMPL_STARTUP]); 
	wait_key(2);
	fade_out_pal(100);


	init_ok = 1;
	log2file(" init OK!");
	return TRUE;
}


// uninits the game
void uninit_game() {
	int i;
	PACKFILE *pf;

	log2file("\nExit routines:");

	log2file(" unloading datafile");
	if (data != NULL) unload_datafile(data);
	
	log2file(" unloading original maps");
	if (maps != NULL) unload_datafile(maps);

	log2file(" destroying temporary map");
	if (map != NULL) destroy_map(map);

	log2file(" freeing level names");
	for(i = 0; i < num_levels; i ++) free(level_files[i]);

	// only save if everything was inited ok!
	if (init_ok) {
		log2file(" saving options");
		pf = pack_fopen("alex4.sav", "wp");
		if (pf) {
			save_options(&options, pf);
			pack_fclose(pf);
		}
		
		log2file(" saving highscores");
		pf = pack_fopen("alex4.hi", "wp");
		if (pf) {
			save_hisc_table(hisc_table, pf);
			save_hisc_table(hisc_table_space, pf);
			pack_fclose(pf);
		}
	}

	if (get_config_int("sound", "use_sound_datafile", 1)) {
		log2file(" unloading sound data");
		if (sfx_data != NULL) unload_datafile(sfx_data);
	}
	else {
		log2file(" freeing sounds");
		for(i = 0; i < MAX_SOUNDS; i ++) {
			if (sfx[i] != NULL)	destroy_sample(sfx[i]);
		}
	}

	log2file(" exiting dumb");
	dumb_exit();

	log2file(" exiting allegro");
	allegro_exit();   
}


// inits the player on a map
void init_player(Tplayer *p, Tmap *m) {
	actor[0].direction = 1;
	actor[0].x = m->start_x << 4;
	if (actor[0].x < 0) {
		actor[0].direction = 0;
		actor[0].x = -actor[0].x;
	}
	actor[0].dy = 0;
	actor[0].y = (m->start_y << 4) + 16;

	p->jumping = 0;
	p->wounded = 0;
	p->dy = 0;
	p->ammo = 0;
	p->eat_counter = 0;
	p->actor->status = (p->ammo ? AC_FULL : AC_NORM);
	p->health = MAX(p->health, 1);
}


// draws text with an outline
void textout_outline(BITMAP *bmp, const char *txt, int x, int y) {
	textout(bmp, data[THE_FONT].dat, txt, x+1, y, 1);
	textout(bmp, data[THE_FONT].dat, txt, x-1, y, 1);
	textout(bmp, data[THE_FONT].dat, txt, x, y+1, 1);
	textout(bmp, data[THE_FONT].dat, txt, x, y-1, 1);
	textout(bmp, data[THE_FONT].dat, txt, x, y, 4);
}


// draws centered text with an outline
void textout_outline_center(BITMAP *bmp, const char *txt, int cx, int y) {
	int x = cx - text_length(data[THE_FONT].dat, txt) / 2;
	textout_outline(bmp, txt, x, y);
}



// plays the let's go sequence
void show_lets_go() {
	BITMAP *go = data[LETSGO].dat;
	int x = -go->w;
	int mode = 0;
	int wait = 0;
	int y = 120, ty = 60;
	int dy = 0;

	cycle_count = 0;
	while(mode != 3) {
		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (got_sound) al_poll_duh(dp);

			// move text
			if (mode == 0 || mode == 2) x += 4;
			if (x >= 80 - go->w/2) mode = 1;
			if (x > 160) mode = 3;
			if (mode == 1) wait ++;
			if (wait > 50 && mode == 1) {
				mode = 2;
				ty = 130;
			}

			// move level name
			if (y > ty) y -= (y-ty)/8;
			if (y < ty) y += dy++;

			// move on
			cycle_count --;
		}

		// let other processes play
		yield_timeslice();

		// draw stuff
		draw_frame(swap_screen, 1);
		draw_sprite(swap_screen, go, x, 35);
		textout_outline_center(swap_screen, map->name, 80, y);
		blit_to_screen(swap_screen);
	
	}
}

// shows the game over sign sequence
void show_game_over() {
	BITMAP *go = data[GAME_OVER].dat;
	
	int x = -go->w;
	int mode = 0;
	int wait = 0;

	cycle_count = 0;
	while(mode != 3) {
		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (got_sound) al_poll_duh(dp);

			// move text
			if (mode == 0 || mode == 2) x += 4;
			if (x >= 80 - go->w/2) mode = 1;
			if (x > 160) mode = 3;
			if (mode == 1) wait ++;
			if (wait > 120 && mode == 1) mode = 2;

			if (key[KEY_ESC]) mode = 3;

			// move on
			cycle_count --;
		}

		// let other processes play
		yield_timeslice();

		// draw stuff
		draw_frame(swap_screen, 1);
		draw_sprite(swap_screen, go, x, 35);
		blit_to_screen(swap_screen);
	}
}

// drawing routine used by
// show_custom_ending()
void draw_custom_ending(BITMAP *bmp) {
	int i, r;
	BITMAP *head = data[FLD_HEAD].dat;

	blit(data[INTRO_BG].dat, bmp, 0, 0, 0, 0, 160, 120);

	r = 70 + fixtoi(20 * fcos(itofix(game_count >> 1)) + 20 * fsin(itofix((int)(game_count / 2.7))) );
	for(i = 0; i < 256; i += 32) 
		draw_sprite(bmp, head, 80 - head->w/2 + fixtoi(r * fcos(itofix(game_count + i))), 60 - head->h/2 + fixtoi(r * fsin(itofix(game_count + i))));

	draw_sprite_h_flip(bmp, data[ALEX].dat, 60, 40);
	draw_sprite(bmp, data[LOLA].dat, 84, 40);

	textout_outline_center(bmp, "Congratulations!", 80, 60);
	textout_outline_center(bmp, "A winner is you!", 80, 80);
}

// show the ending of a custom game
void show_custom_ending() {
	int done = 0;
	int wait = 1000; // ten seconds
	int tmp;

	tmp = game_count;
	draw_custom_ending(swap_screen);
	blit_to_screen(swap_screen);
	fade_in_pal(100);
	game_count = tmp;

	cycle_count = 0;
	while(!done) {
		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (got_sound) al_poll_duh(dp);

			// poll user
			poll_control(&ctrl);
			if (is_fire(&ctrl) || is_jump(&ctrl) || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_ESC]) 
				done = 1;

			// decrease time
			if (--wait < 0) done = 1;

			// move on
			cycle_count --;
		}

		// let other processes play
		yield_timeslice();

		// draw stuff
		draw_custom_ending(swap_screen);
		blit_to_screen(swap_screen);
	}

}

// lighten or darken a 4 color bitmap
void transform_bitmap(BITMAP *bmp, int steps) {
	int x, y;
	int c;

	for(x = 0; x < bmp->w; x ++) {
		for(y = 0; y < bmp->h; y ++) {
			c = _getpixel(bmp, x, y);
			_putpixel(bmp, x, y, MIN(MAX(c + steps, 1), 4));
		}
		if (got_sound) al_poll_duh(dp);
	}
}

// draws the scoring sequence at end of level
// used by show_cutscene(..)
void draw_cutscene(BITMAP *bmp, int org_level, int _level, int _lives, int _stars, int _cherries) {
	BITMAP *go = data[LEVELCOMPLETE].dat;
	char buf[80];

	clear_bitmap(bmp);

	draw_sprite(bmp, go, 80 - go->w/2, 10);

	sprintf(buf, "Level    %2d*100 = %4d", org_level, _level);
	textout_outline(bmp, buf, 3, 60);
	sprintf(buf, "Lives    %2d*100 = %4d", player.lives, _lives);
	textout_outline(bmp, buf, 3, 72);
	sprintf(buf, "Stars    %2d*100 = %4d", player.stars_taken, _stars);
	textout_outline(bmp, buf, 3, 84);
	sprintf(buf, "Cherries %2d* 10 = %4d", player.cherries_taken, _cherries);
	textout_outline(bmp, buf, 3, 96);
}

// shows the scoring sequence at end of level
void show_cutscene(int level) {
	int x = -160;
	BITMAP *bmp = create_bitmap(160, 120);
	BITMAP *swap2 = create_bitmap(160, 120);
	int mode = 0;
	int my_counter = 0;

	int _lives = player.lives * 100;
	int _level = level * 100;
	int _cherries = player.cherries_taken * 10;
	int _stars = player.stars_taken * 100;

	if (swap2 == NULL || bmp == NULL) {
		if (swap2 != NULL) destroy_bitmap(swap2);
		if (bmp != NULL) destroy_bitmap(bmp);
		return;
	}

	// music!
	if (got_sound) start_music(MOD_LEVEL_DONE);


	// create cutscene screene
	blit(swap_screen, swap2, 0, 0, 0, 0, 160, 120);

	transform_bitmap(swap2, -1);

	draw_cutscene(bmp, level, _level, _lives, _stars, _cherries);

	// scroll bmp onto swap_screen
	clear_keybuf();
	cycle_count = 0;
	while(mode != 3) {
		// do logic
		while(cycle_count > 0) {
			logic_count ++;
			my_counter ++;
			poll_control(&ctrl);

			// poll music machine
			if (got_sound) al_poll_duh(dp);

			if (mode == 1 && (keypressed() || is_fire(&ctrl) || is_jump(&ctrl) ) || my_counter > 200) {
				mode = 2;
			}
			
			// move text
			if (mode == 0) x += 8;
			if (x == 0 && mode == 0) {
				mode = 1;
				clear_keybuf();
			}

			// count stats
			if (mode == 2 && !(game_count % 4)) {
				int a = 0;
				if (_level)    { player.score +=  100; _level    -= 100; a++; }
				if (_lives)    { player.score +=  100; _lives    -= 100; a++; }
				if (_stars)    { player.score +=  100; _stars    -= 100; a++; }
				if (_cherries) { player.score +=   10; _cherries -=  10; a++; }
				if (!a) mode = 3;

				play_sound(sfx[SMPL_POINT]);

				draw_cutscene(bmp, level, _level, _lives, _stars, _cherries);
			}

			// move on
			cycle_count --;
		}

		// let other processes play
		yield_timeslice();

		// draw stuff
		blit(swap2, swap_screen, 0, 0, 0, 0, 160, 120);
		draw_status_bar(swap_screen, 110);
		draw_sprite(swap_screen, bmp, x, 0);
		blit_to_screen(swap_screen);
	}

	wait_key(5);

	if (got_sound) stop_music();

	destroy_bitmap(bmp);
}

// shows a highscore table
void show_scores(int space, Thisc *table) {
	DATAFILE *df = NULL;
	BITMAP *bg = NULL;

	if (space) {
		// get space bg
		packfile_password(init_string);
		df = load_datafile_object("data/a45.dat", "BG1");
		packfile_password(NULL);
		if (df != NULL)	{
			bg = df->dat;
		}
		else
			msg_box("ooga");
	}

	if (bg == NULL || !space)
		blit(data[INTRO_BG].dat, swap_screen, 0, 0, 0, 0, 160, 120);
	else {
		clear_to_color(swap_screen, 1);
		blit(bg, swap_screen, 0, 0, 0, 0, 160, 120);
	}


	textout_outline_center(swap_screen, "High scores", 80, 8);
	textout_outline_center(swap_screen, "Press any key", 80, 100);
	draw_hisc_table(table, swap_screen, data[THE_FONT].dat, 10, 30, (space ? 4 : 1), !space);

	blit_to_screen(swap_screen);
	fade_in_pal(100);

	clear_keybuf();
	poll_control(&ctrl);
	while(!is_jump(&ctrl) && !keypressed()) {
		poll_control(&ctrl);
		if (got_sound) al_poll_duh(dp);
	}
	play_sound(sfx[SMPL_MENU]);

	fade_out_pal(100);

	// clean up
	if (df != NULL) unload_datafile_object(df);

}

// draws the level selector
void draw_select_starting_level(BITMAP *bmp, int level, int min, int max) {
	BITMAP *stuff = create_bitmap(40, 10);
	char buf[80];
	int xpos = 2;

	blit(data[ALEX_BG].dat, bmp, 0, 0, 0, 0, 160, 112);
	rectfill(bmp, 0, 112, 160, 120, 2);

	sprintf(buf, "%s %d %s", (level > min ? "<" : " "), level, (level < max ? ">" : " "));
	clear_bitmap(stuff);
	textout_centre(stuff, data[THE_FONT].dat, buf, stuff->w/2 + 1, 1, 2);
	textout_centre(stuff, data[THE_FONT].dat, buf, stuff->w/2, 0, 1);
	stretch_sprite(bmp, stuff, 80 - 4*stuff->w/2, 30, 4*stuff->w, 4*stuff->h);

	textout_centre(bmp, data[THE_FONT].dat, "SELECT START LEVEL", 80, 90, 1);
	textout_centre(bmp, data[THE_FONT].dat, "SELECT START LEVEL", 79, 89, 4);

	if (options.one_hundred) {
		if (game_count & 32 || game_count & 16) draw_sprite(bmp, data[SHIP100].dat, xpos, 2);
	}
	else {
		if (options.stars[level - 1]) {
			draw_sprite(bmp, data[STAR].dat, xpos, 2);
			draw_sprite(bmp, data[ALL100].dat, xpos + 4, 14);
			xpos += 20;
		}
		if (options.cherries[level - 1]) {
			draw_sprite(bmp, data[CHERRY].dat, xpos, 2);
			draw_sprite(bmp, data[ALL100].dat, xpos + 4, 14);
		}
	}


	destroy_bitmap(stuff);
}


// lets the player select starting level
int select_starting_level() { 
	int start_level = 1;
	int done = 0;
	int counter = 0;

	draw_select_starting_level(swap_screen, start_level, 1, options.max_levels + 1);
	blit_to_screen(swap_screen);
	fade_in_pal(100);

	clear_keybuf();

	cycle_count = 0;
	while(!done) {
		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (got_sound) al_poll_duh(dp);

			// check controls
			poll_control(&ctrl);
			if (is_right(&ctrl) && start_level < options.max_levels + 1 && !counter) {
				start_level ++;
				play_sound(sfx[SMPL_MENU]);
			}

			if (is_left(&ctrl) && start_level > 1 && !counter) {
				start_level --;
				play_sound(sfx[SMPL_MENU]);
			}

			if (is_jump(&ctrl) || is_fire(&ctrl)) {
				done = 1;
				play_sound(sfx[SMPL_MENU]);
			}
			if (keypressed()) {
				int scancode = readkey() >> 8;
				if (scancode == KEY_SPACE || scancode == KEY_ENTER) {
					done = 1;
					play_sound(sfx[SMPL_MENU]);
				}
				if (scancode == KEY_F1 && options.one_hundred) {
					done = 1;
					start_level = -100;
					play_sound(sfx[SMPL_MENU]);
				}
				if (scancode == KEY_ESC) {
					done = 1;
					start_level = -1;
					play_sound(sfx[SMPL_MENU]);
				}
			}
			if (!is_left(&ctrl) || !is_right(&ctrl)) counter = 0;
			if (is_left(&ctrl) || is_right(&ctrl)) counter = 10;

			// move on
			cycle_count --;
		}

		// draw stuff
		if (start_level >= 0) {
			draw_select_starting_level(swap_screen, start_level, 1, options.max_levels + 1);
			blit_to_screen(swap_screen);
		}
	}

	return start_level;
}



// starts a new game
void new_game(int reset_player_data) {
	// init player
	if (reset_player_data) {
		player.ammo = 0;
		player.lives = 2;
		player.score = 0;
		player.health = 1;
	}
	player.actor = &actor[0];
	player.eat_counter = 0;
	actor[0].active = 1;
	actor[0].data = data;
	actor[0].frames[0] = HERO000;
	actor[0].frames[1] = HERO001;
	actor[0].frames[2] = HERO002;
	actor[0].frames[3] = HERO003;
	actor[0].frames[4] = HERO_NORM;
	actor[0].num_frames = 4;
	actor[0].frame = 0;
	actor[0].anim_counter = 0;
	actor[0].anim_max = 4;
}


// tidies up after a map has been used
void deinit_map(Tmap *m) {
	int i;

	// stop any playing sounds
	for(i = 1; i < MAX_ACTORS; i ++) {
		if (actor[i].active && actor[i].sound != -1) {
			stop_sound_id(actor[i].sound);
		}
	}
}




// sets up actors and stuff
void init_map(Tmap *m) {
	int x, y, i;

	m->win_conditions_fullfilled = 0;
	m->num_enemies = 0;

	for(i = 1; i < MAX_ACTORS; i ++) {
		actor[i].active = 0;
	}

	// check entire map for enemies and initialize
	// them as they are found
	for(x = 0; x < m->width; x ++) {
		for(y = 0; y < m->height; y ++) {
			if (m->dat[x + y * m->width].type == MAP_ENEMY1) {	// small human
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY1;
					a->num_frames = 4;					
					a->frames[0] = ENEMY1_01;
					a->frames[1] = ENEMY1_02;
					a->frames[2] = ENEMY1_03;
					a->frames[3] = ENEMY1_04;
					a->flat_frame = ENEMY1_05;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags = ACF_JUMPABLE | ACF_FLATABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY2) { // big human
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY2;
					a->num_frames = 4;					
					a->frames[0] = ENEMY2_01;
					a->frames[1] = ENEMY2_02;
					a->frames[2] = ENEMY2_03;
					a->frames[3] = ENEMY2_04;
					a->flat_frame = ENEMY2_05;
					a->w = 12; a->h = 19;
					a->ox = 2; a->oy = 5;
					a->flags = ACF_JUMPABLE | ACF_FLATABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY3) { // crusher
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY3;
					a->num_frames = 1;					
					a->frames[0] = ENEMY3;
					a->w = 30; a->h = 16;
					a->ox = 1; a->oy = 112-16;
					a->flags = ACF_HURTS;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY4) { // spike fish
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY4;
					a->num_frames = 1;					
					a->frames[0] = ENEMY4;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags =  ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY5) { // yelly fish
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY5;
					a->num_frames = 1;					
					a->frames[0] = ENEMY5_01;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags = ACF_JUMPABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY6) { // cannon
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_ENEMY6;
					a->num_frames = 0;					
					a->flags = 0;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_GUARD1) { // spike-car
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_GUARD1;
					a->energy = 100;
					a->num_frames = 4;					
					a->hit_offset = 4;					
					a->frames[0] = GUARD1_1;
					a->frames[1] = GUARD1_2;
					a->frames[2] = GUARD1_3;
					a->frames[3] = GUARD1_4;
					a->w = 32; a->h = 32;
					a->ox = 0; a->oy = 0;
					a->flags = ACF_HURTS | ACF_ROLLABLE_BACK;
					m->num_enemies ++;
					a->sound = SMPL_ENGINE;
					play_sound_id_ex(a->sound, 100, 1000, 1);
				}
			}
			if (m->dat[x + y * m->width].type == MAP_GUARD2) { // spike-jumper-crusher
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16, data);
				if (a != NULL) {
					a->type = MAP_GUARD2;
					a->energy = 150;
					a->num_frames = 2;					
					a->frames[0] = GUARD2_1a;
					a->frames[1] = GUARD2_1b;
					a->w = 32; a->h = 32;
					a->ox = 0; a->oy = 16;
					a->flags = ACF_HURTS | ACF_JUMPABLE | ACF_BOUNCEABLE;
					m->num_enemies ++;
				}
			}
		}
	}
}


// starts a new level
// level_id < 0 -> load fname
// uses datafile map o/w
void new_level(char *fname, int level_id, int draw) {
	int tox;
	int i;
	int x, y;

	if (map != NULL) destroy_map(map);
	map = NULL;

	if (level_id < 0) {
		if (level_id == -1) {
			// get map from file
			log2file(" loading map <%s>", fname);
			map = load_map(fname);
			if (!map) {
				msg_box("failed to load map!!!");
    			log2file(" *** failed!");
				return;
			}
		}
		else {
			// get map from data file
			log2file(" getting map %d from datafile", atoi(fname));
			map = load_map_from_memory((void *)data[SCRIPTMAP0 + atoi(fname)].dat);
		}
	}
	else {
		// get map from MAP data file
		log2file(" getting map %d from map-datafile", level_id);
		map = load_map_from_memory((void *)maps[level_id].dat);
	}

	

	map->data = data;

	init_player(&player, map);

	if (player.actor->direction) tox = MAX(0, MIN(player.actor->x - 60, map->width * 16 - 160));
	else tox = MAX(0, MIN(player.actor->x - 85, map->width * 16 - 160));
	map->offset_x = tox;

	init_map(map);

	reset_particles(particle, MAX_PARTICLES);
	reset_bullets(bullet, MAX_BULLETS);

	for(i = 1; i < MAX_ACTORS; i++)
		if (actor[i].active) 
			update_actor_with_map(&actor[i], map);

	// calc pickup frequency
	player.cherries = player.cherries_taken = 0;
	player.stars = player.stars_taken = 0;
	for(x = 0; x < map->width; x ++) {
		for(y = 0; y < map->height; y ++) {
			if (map->dat[x + y * map->width].item == MAP_STAR) player.stars ++;
			if (map->dat[x + y * map->width].item == MAP_CHERRY) player.cherries ++;
		}
	}

	if (draw) {
		draw_frame(swap_screen, 1);
		blit_to_screen(swap_screen);
		fade_in_pal(100);
	}
}




// updates player movement
void update_player() {
	int old_x = player.actor->x;
	int x1, x2;
	Tmappos *mp;
	int tx, ty;

	if (player.actor->direction) {
		x1 = 2;
		x2 = 10;
	}
	else {
		x1 = 13;
		x2 = 6;
	}
	
	poll_control(&ctrl);

	if (player.wounded) player.wounded --;
	
	// most stuff can only be done when the player is alive
	if (player.actor->status != AC_DEAD) {

		// handle eating animation
		if (player.actor->status == AC_EAT) {
			player.eat_counter ++;
			if (player.eat_counter == 8) {
				player.actor->status = AC_FULL;	
				player.eat_counter = 0;
			}
		}
	
		// handle firing
		if (is_fire(&ctrl) && player.actor->status == AC_FULL && !player.eat_pressed) {
			player.actor->status = AC_SPIT;
			player.eat_pressed = 1;
			if (player.ammo) {
				Tbullet *b = get_free_bullet(bullet, MAX_BULLETS);
				if (b != NULL) {
					set_bullet(b, (int)player.actor->x + 7, (int)player.actor->y - 14, (player.actor->direction ? 4 : -4), 0, data[EGG2].dat, 0);
					player.ammo --;
					play_sound(sfx[SMPL_SPIT]);
				}
			}


		}
		
		// return to normal
		if (!is_fire(&ctrl)) {
			player.eat_pressed = 0;
			if (player.eat_counter == 0 && player.actor->status != AC_BALL) {
				if (player.ammo) player.actor->status = AC_FULL;
				else player.actor->status = AC_NORM;
			}
		}
	
	
		// horizontal movement
		if (is_left(&ctrl) && (player.actor->status != AC_BALL)) {
			player.actor->x -= 1;
			if (player.actor->direction) player.actor->x -= 3;
			player.actor->direction = 0;
			animate_actor(&actor[0]);
		}
		else if (is_right(&ctrl) && (player.actor->status != AC_BALL)) {
			player.actor->x += 1;
			if (!player.actor->direction) player.actor->x += 3;
			player.actor->direction = 1;
			animate_actor(&actor[0]);
		}

		// rolling?
		if (player.actor->status == AC_BALL) {
			if (player.actor->direction) {
				player.angle += 8;
				player.actor->x += 2;
			}
			else {
				player.angle -= 8;
				player.actor->x -= 2;
			}
		}
	
		// horizontal tile collision
		if (is_ground(map, player.actor->x + x1, player.actor->y - 1))  { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound(sfx[SMPL_HIT]); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x2, player.actor->y - 1))  { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound(sfx[SMPL_HIT]); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x1, player.actor->y - 15)) { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound(sfx[SMPL_HIT]); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x2, player.actor->y - 15)) { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound(sfx[SMPL_HIT]); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
	
		
		// vertical movement
		if (!is_ground(map, player.actor->x + x1, player.actor->y)) {
			if (!is_ground(map, player.actor->x + x2, player.actor->y)) {
				player.actor->dy ++;
				player.jumping = 1;
			}
		}
		else {
			player.jumping = 0;
			player.actor->dy = 0;
		}
	
		// jumping?
		if (is_jump(&ctrl) && !player.jumping && !player.jump_pressed) {
			player.actor->dy = -16;
			player.jumping = 1;
			player.jump_pressed = 1;
			play_sound(sfx[SMPL_JUMP]);
		}
		if (!is_jump(&ctrl)) {
			if (player.actor->dy < 0) player.actor->dy >>= 1;
			player.jump_pressed = 0;
		}
	
		// apply vertical force
		player.actor->y += player.actor->dy >> 2;

		// check floor
		while (is_ground(map, player.actor->x + x1, player.actor->y - 1)) { player.actor->y --; player.jumping = 0; }
		while (is_ground(map, player.actor->x + x2, player.actor->y - 1)) { player.actor->y --; player.jumping = 0; }
		
		// check roof
		while (is_ground(map, player.actor->x + x1, player.actor->y - 15)) { player.actor->y ++; player.actor->dy = 0; }
		while (is_ground(map, player.actor->x + x2, player.actor->y - 15)) { player.actor->y ++; player.actor->dy = 0; }
		
		// check for map types
		////// PICKUPS / EATS
		tx = (player.actor->x + (player.actor->direction ? 10 : 6)) >> 4;
		ty = (player.actor->y - 10) >> 4;
		mp = get_mappos(map, tx, ty);
		
		if (mp != NULL) {
			if (mp->item == MAP_EGG) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.ammo ++;
				play_sound(sfx[SMPL_EAT]);
			}
			if (mp->item == MAP_HEART) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				if (player.health >= 2) {
					player.score += 100;
				}
				else { 
					player.health ++;
				}
				play_sound(sfx[SMPL_HEART]);
			}
			if (mp->item == MAP_STAR) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.score += 100;
				player.stars_taken ++;
				play_sound(sfx[SMPL_STAR]);

				if (player.stars == player.stars_taken) {
					Tparticle *p;
					p = get_free_particle(particle, MAX_PARTICLES);
					if (p != NULL) {
						set_particle(p, (tx << 4) + 4, ty << 4, 0, -0.5, 0, 50, ALL100);
					}
				}
			}
			if (mp->item == MAP_CHERRY) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.score += 10;
				player.cherries_taken ++;
				play_sound(sfx[SMPL_CHERRY]);
 
				if (player.cherries == player.cherries_taken) {
					Tparticle *p;
					p = get_free_particle(particle, MAX_PARTICLES);
					if (p != NULL) {
						set_particle(p, (tx << 4) + 8, ty << 4, 0, -0.5, 0, 50, ALL100);
					}
				}
			}
			else if (mp->item == MAP_1UP) {
				create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 4, 32, 0, -1);
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.lives ++;
				play_sound(sfx[SMPL_XTRALIFE]);
			}
		}

		
		////// MIDDLE STUFF (door, spikes)
		tx = (player.actor->x + 7) >> 4;
		ty = (player.actor->y - 7) >> 4;
		mp = get_mappos(map, tx, ty);

		// turn off exit flag
		map->win_conditions_fullfilled &= ~MAP_WIN_EXIT;
		// check tile for death or door
		if (mp != NULL) {
			if (mp->type == MAP_DEAD) {
				kill_player(&player);
				play_sound(sfx[SMPL_A_DIE]);
			}
			if (mp->type == MAP_EXIT) {
				map->win_conditions_fullfilled |=  map->win_conditions & MAP_WIN_EXIT;
			}
		}
		
	
		////// BREAKABLES
		tx = (player.actor->x + (player.actor->direction ? 15 : 0)) >> 4;
		ty = (player.actor->y - 7) >> 4;
		mp = get_mappos(map, tx, ty);
		
		if (mp != NULL) {
			if (mp->type == MAP_BRK && player.actor->status == AC_BALL) {
				mp->tile = mp->type = mp->mask = 0;
				create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 16, 64, 0, -1);
				create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 16, 64, 0, -1);
				play_sound(sfx[SMPL_CRUSH]);
			}
		}
			

		////// FLOORS
		tx = (player.actor->x + 7) >> 4;
		ty = (player.actor->y + 4) >> 4;
		mp = get_mappos(map, tx, ty);
		
		if (mp != NULL) {
			if (mp->type == MAP_SPIN && player.actor->status != AC_BALL) {
				Tmappos *m = get_mappos(map, tx + (player.actor->direction ? 1 : -1), ty - 1);
				if (m != NULL) {
					if (!m->mask) {
						player.actor->status = AC_BALL;
						play_sound(sfx[SMPL_SPIN]);
					}
				}
			}
		}
		
	}
	else {	// player is dead
		player.actor->y += player.actor->dy >> 2;
		player.actor->dy ++;
		player.actor->x += (player.actor->direction ? 1 : -1); 
	}
}



// checks and acts upone collisions between
// bullets and enemies
void check_bullets_with_enemies() {
	int b, e;
	
	for(b = 0; b < MAX_BULLETS; b ++) {
		if (bullet[b].exist && bullet[b].bad == 0) {
			for(e = 1; e < MAX_ACTORS; e ++) {
				if (actor[e].active && actor[e].status == AC_NORM && (actor[e].flags & ACF_SHOOTABLE)) {
					if (check_bb_collision(bullet[b].x, bullet[b].y, 7, 6, actor[e].x, actor[e].y-actor[e].h, 16, actor[e].h)) {
						kill_actor(&actor[e]);
						map->num_enemies --;
						actor[e].direction = (bullet[b].dx > 0 ? 0 : 1);
						bullet[b].exist = 0;
						create_burst(particle, (int)bullet[b].x+4, (int)bullet[b].y+3, 4, 8, 0, -1);
						play_sound(sfx[SMPL_E_DIE]);
					}
				}
			}
		}
	}
}


// checks and acts upon collisions between
// bullets and alex
void check_bullets_with_alex() {
	int b;
	
	for(b = 0; b < MAX_BULLETS; b ++) {
		if (bullet[b].exist && bullet[b].bad) {
			if (actor[0].status != AC_DEAD) {
				if (check_bb_collision(bullet[b].x+1, bullet[b].y+1, bullet[b].bmp->w-1, bullet[b].bmp->h-1, actor[0].x+4, actor[0].y-16, 8, 16)) {
					if (!player.wounded) {
						wound_player(&player);
						if (player.actor->status == AC_DEAD) play_sound(sfx[SMPL_A_DIE]);
						else play_sound(sfx[SMPL_HURT]);
					}
					//actor[0].direction = (bullet[b].dx > 0 ? 1 : 0);
					bullet[b].exist = 0;
					create_burst(particle, (int)bullet[b].x+4, (int)bullet[b].y+3, 4, 16, 0, -1);
				}
			}
		}
	}
}



// checks and acts upon collisions between
// enemies and alex
void check_alex_with_enemies() {
	int e;

	if (actor[0].status != AC_DEAD) {
		for(e = 1; e < MAX_ACTORS; e ++) {
			actor[e].is_hit = 0;
			if (actor[e].active && actor[0].status != AC_DEAD && actor[e].flags & ACF_HURTS) {
				// first check if alex jumped on the enemy
				if (actor[0].dy >= 0 && actor[e].status == AC_NORM && player.jumping && actor[0].status != AC_BALL && actor[e].flags & ACF_JUMPABLE) {
					if (check_bb_collision(actor[0].x+2, actor[0].y+12-16, 12, 8, actor[e].x, actor[e].y-actor[e].h, actor[e].w, 4)) {
						Tparticle *p;
						p = get_free_particle(particle, MAX_PARTICLES);
						if (p != NULL) {
							set_particle(p, actor[e].x + (actor[e].w >> 1), actor[0].y, 0, 0, 0, 5, PARTICLE_BOPP);
						}
						
						if (actor[e].flags & ACF_FLATABLE) {
							// flatten
							actor[e].status = AC_FLAT;
							actor[e].counter = 0;
						}
						else if (!(actor[e].flags & ACF_BOUNCEABLE)) {
							// kill
							kill_actor(&actor[e]);
							map->num_enemies --;
							actor[e].direction = (actor[0].direction ? 0 : 1);
						}
						
						if (actor[e].flags & ACF_BOUNCEABLE) {
							// bounce
							player.actor->y = actor[e].y - actor[e].h - 6;
							player.dy = -15;	// jump!
							player.jumping = TRUE;
							play_sound(sfx[SMPL_JUMP]);
							if (actor[e].mode == 0) {
								actor[e].mode = 4;
								actor[e].counter = 0;
							}
						}
						else {
							player.dy = -5;
							//play_sound((actor[e].type == MAP_ENEMY5 ? sfx[SMPL_E_DIE] : sfx[SMPL_MASH]));
							play_sound(sfx[SMPL_MASH]);
							if (is_jump(&ctrl) ) {
								player.dy = -20;	// jump!
								player.jumping = TRUE;
								play_sound(sfx[SMPL_JUMP]);
							}
						}
					}
				}
				else {
					// now check if alex dies or knocks them over
					if (actor[e].status == AC_NORM || (actor[e].status == AC_FLAT && actor[0].status == AC_BALL)) {
						if (check_bb_collision(actor[0].x + (actor[0].direction ? 0 : 8), actor[0].y-16, 8, 16, actor[e].x + actor[e].ox, actor[e].y - actor[e].h, actor[e].w, actor[e].h)) {
							int wounded = 0;
							if (actor[0].status == AC_BALL) {
								if (actor[e].flags & ACF_ROLLABLE) { 
									kill_actor(&actor[e]);
									map->num_enemies --;
									actor[e].direction = (actor[0].direction ? 0 : 1);
									play_sound(sfx[SMPL_E_DIE]);	
								}
								else if (actor[e].flags & ACF_ROLLABLE_BACK) {
									if (actor[e].direction == actor[0].direction) {
										actor[e].energy --;
										actor[e].is_hit = 1;
										play_sound(sfx[SMPL_HIT]);	
									}
									else wounded = 1;
								}
								else wounded = 1;
							}
							else wounded = 1;

							if (!player.wounded && wounded) {
								clear(swap_screen);
								wound_player(&player);
								if (player.actor->status == AC_DEAD) {
									actor[0].direction = actor[e].direction;
									play_sound(sfx[SMPL_A_DIE]);
								}
								else {
									play_sound(sfx[SMPL_HURT]);
								}
							}
						}
					}
					// maybe he can eat them?
					else if (actor[e].status == AC_FLAT && actor[0].status != AC_BALL) {
						if (check_bb_collision(actor[0].x + (actor[0].direction ? 16 : -8), actor[0].y, 8, 16, actor[e].x + 2, actor[e].y + 8, 12, 8)) {
							actor[e].active = 0;
							map->num_enemies --;
							actor[0].status = AC_EAT;
							player.score += 50;
							player.eat_counter = 0;
							play_sound(sfx[SMPL_EAT]);
						}
					}
				}
			}
		}
	}
}


// calculates camera pos for map m considering player p
void calculate_camera_pos(Tplayer *p, Tmap *m) {
	static camera_type = 1;

	if (p->actor->status == AC_BALL) {
		camera_type = 2;
	}
	else {
		camera_type = 1;
	}


	if (camera_type == 0) { // centered camera, doesn't happen a lot
		static int target_x;
		target_x = MAX(0, MIN(p->actor->x + 8 - 80, m->width * 16 - 160));
		if (m->offset_x < target_x) m->offset_x ++;
		if (m->offset_x > target_x) m->offset_x --;
	}
	else if (camera_type == 1) { // walk camera
		int x = p->actor->x - m->offset_x;
		int pan = 1;
		int tox, d_tox, step;
		static int side = 1;
		static int d_step = 0;
		static int count = 0;
		
		if (side) {
			if (x < 60 && x > 40 ) pan = 0;
		}
		else {
			if (x < 105 && x > 85 ) pan = 0;
		}
		
		if (pan) {
			side = p->actor->direction;
			count = 0;
		}
		else {
			d_step = 0;
			if (++ count > 50) pan = 1; // pan anyway
		}
		
		
		
		if (pan) {
			if (p->actor->direction) tox = MAX(0, MIN(p->actor->x - 60, m->width * 16 - 160));
			else tox = MAX(0, MIN(p->actor->x - 85, m->width * 16 - 160));
			
			d_tox = tox - m->offset_x;
			if (ABS(d_tox) > 1) step = 2;
			else if (ABS(d_tox) > 0) step = 1;
			else step = 0;
			if (p->actor->status != AC_BALL) step = MIN(step, 3);
			step *= SGN(d_tox);
			
			if (d_step > step) d_step --;
			if (d_step < step) d_step ++;
			
			m->offset_x = MAX(0, MIN(m->offset_x + d_step, m->width * 16 - 160));
		}
	}
	else if (camera_type == 2) { // roll camera
		int x = p->actor->x - m->offset_x;
		int pan = 1;
		int tox, d_tox, step;
		static int side = 1;
		static int d_step = 0;
		static int count = 0;
		
		if (side) {
			if (x < 34 && x > 8 ) pan = 0;
		}
		else {
			if (x < 136 && x > 111 ) pan = 0;
		}
		
		if (pan) {
			side = p->actor->direction;
			count = 0;
		}
		else {
			d_step = 0;
			if (++ count > 50) pan = 1; // pan anyway
		}
		
		
		
		if (pan) {
			if (p->actor->direction) tox = MAX(0, MIN(p->actor->x - 34, m->width * 16 - 160));
			else tox = MAX(0, MIN(p->actor->x - 112, m->width * 16 - 160));
			
			d_tox = tox - m->offset_x;
			if (ABS(d_tox) > 70) step = 8;
			else if (ABS(d_tox) > 60) step = 7;
			else if (ABS(d_tox) > 50) step = 6;
			else if (ABS(d_tox) > 40) step = 5;
			else if (ABS(d_tox) > 30) step = 4;
			else if (ABS(d_tox) > 20) step = 3;
			else if (ABS(d_tox) > 1) step = 2;
			else if (ABS(d_tox) > 0) step = 1;
			else step = 0;
			if (p->actor->status != AC_BALL) step = MIN(step, 3);
			step *= SGN(d_tox);
			
			if (d_step > step) d_step --;
			if (d_step < step) d_step ++;
			
			m->offset_x = MAX(0, MIN(m->offset_x + d_step, m->width * 16 - 160));
		}
	}
}


// shows the pause menu
int do_pause_menu(BITMAP *bg) {
	int done = 0;

	// pause sound
	if (got_sound && !editing) al_pause_duh(dp);
	play_sound_id(SMPL_PAUSE);

	// darken screen
	transform_bitmap(bg, -1);
	blit_to_screen(bg);
	
	// show text
	textout_outline_center(bg, (editing ? "<<< EDITOR PAUSED >>>" : "<<< GAME PAUSED >>>"), 80, 40);
	textout_outline_center(bg, "esc to quit", 80, 60);
	textout_outline_center(bg, "any key to resume", 80, 70);
	blit_to_screen(bg);

	// wait to release esc
	while(key[KEY_ESC]);
	
	// wait for user input
	clear_keybuf();
	while(!done) {
		if (got_sound) al_poll_duh(dp);
		poll_control(&ctrl);
		if (is_fire(&ctrl) || is_jump(&ctrl)) done = 1;
		if (keypressed()) done = 1;
		if (key[KEY_ESC]) done = -1;
		yield_timeslice();
	}

	if (done == -1) {
		game_status = GS_QUIT_GAME;
		play_sound_id(SMPL_MENU);
	}
	else {
		play_sound_id(SMPL_PAUSE);
	}

	if (got_sound && !editing) al_resume_duh(dp);

	return done;
}


// play the game!
int play(int level) {
	int i;
	int playing_go_music = 0;


	game_status = GS_OK;
	cycle_count = 0;
	while(game_status == GS_OK) {

		//  do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll music machine
			if (got_sound) {
				al_poll_duh(dp);
			}

			// check if user wants to enter edit mode
			if (!playing_original_game && !editing) {
				if (key[KEY_F1]) {		// toggle edit on/off
					editing = 1;
					if (got_sound) stop_music();
					set_edit_mode(EDIT_MODE_DRAW);
					log2file("Entering EDIT MODE");
				}
			}
			
			// check player moves
			if (!editing) {
				update_player();
	
				// dead?
				if (player.actor->status == AC_DEAD && !playing_go_music) {
					if (got_sound) start_music((player.lives ? MOD_PLAYER_DIES : MOD_GAME_OVER));
					playing_go_music = 1;
				}
				
				if (player.actor->y > 160) {
					if (player.actor->status != AC_DEAD) {
						kill_player(&player);
						player.actor->y = 150;
						player.actor->dy = -20;
						play_sound(sfx[SMPL_A_DIE]);
					}
					else 
						game_status = GS_GAME_DIED;
					
				}

				// camera
				calculate_camera_pos(&player, map);

				// actors
				for(i = 1; i < MAX_ACTORS; i ++) {
					if (actor[i].active) {
						// always update guard actors
						if (actor[i].type == MAP_GUARD1 || actor[i].type == MAP_GUARD2) {
							update_actor_with_map(&actor[i], map);
						}
						// update other actors only if on screen
						else if (actor[i].x > map->offset_x - 32 && actor[i].x < map->offset_x + 160 + 32)
							update_actor_with_map(&actor[i], map);
					}
				}
				if (player.dy) {
					player.actor->dy = player.dy;
					player.dy = 0;
				}

				// particles
				for(i = 0; i < MAX_PARTICLES; i ++) {
					if (particle[i].life) {
						//update_particle(&particle[i]);
						update_particle_with_map(&particle[i], map);
					}
				}
				
				// bullets
				for(i = 0; i < MAX_BULLETS; i ++) {
					if (bullet[i].exist) {
						update_bullet_with_map(&bullet[i], map);
					}
				}
				
				// collision checking
				// bullets vs enemies!
				check_bullets_with_enemies();
				// alex vs enemies!
				check_alex_with_enemies();
				// alex vs enemy bullets!
				check_bullets_with_alex();
				

				// update and check win conditions
				if (map->num_enemies == 0) map->win_conditions_fullfilled |= map->win_conditions & MAP_WIN_KILL_ALL;
				if (map->win_conditions_fullfilled == map->win_conditions) 
					game_status = GS_LEVEL_DONE;


			}
			else { 			// edit stuff
				int mx = mouse_x / (SCREEN_W / 160);
				int my = mouse_y / (SCREEN_H / 120);

				player.actor->x = map->start_x * 16;
				player.actor->y = map->start_y * 16 + 16;
				if (player.actor->x < 0) {
					player.actor->x = -map->start_x * 16;
					player.actor->direction = 0;
				}
				else {
					player.actor->direction = 1;
				}
				update_edit_mode(map, swap_screen, mx, my, mouse_b);
			}
			
			// COMMON STUFF
			if (key[KEY_ESC]) {
				do_pause_menu(swap_screen);
				cycle_count = 0;
			}
			if (key[KEY_F12]) {
				take_screenshot(swap_screen);
				while(key[KEY_F12])	if (got_sound) al_poll_duh(dp);
				cycle_count = 0;
			}
			
			cycle_count --;
		}
		
		// let other processes play
		yield_timeslice();

		// draw 
		frame_count ++;
		draw_frame(swap_screen, 1);
		blit_to_screen(swap_screen);
	}

	return game_status;
}


// draws the title
void draw_title(BITMAP *bmp, int tick) {
	int w, h;
	char start_string[128] = "START GAME";
	int y, x, step = 12;

	if (!playing_original_game) strcpy(start_string, "CUSTOM GAME");

	blit(data[ALEX_BG].dat, bmp, 0, 0, 0, 0, 160, 112);
	rectfill(bmp, 0, 112, 160, 120, 2);

	w = ((BITMAP *)data[ALEX_LOGO].dat)->w;
	h = ((BITMAP *)data[ALEX_LOGO].dat)->h;
	draw_sprite(bmp, data[ALEX_LOGO].dat, 80 - w/2, 30 - h/2);

	draw_scroller(&hscroll, bmp, 0, 110);

	y = 60;
	x = 50;
	textout(bmp, data[THE_FONT].dat, start_string, x+1, y+1, 1);
	textout(bmp, data[THE_FONT].dat, start_string, x, y, 4);

	y += step;
	textout(bmp, data[THE_FONT].dat, "HIGH SCORES", x+1, y+1, 1);
	textout(bmp, data[THE_FONT].dat, "HIGH SCORES", x, y, 4);

	y += step;
	textout(bmp, data[THE_FONT].dat, "EDITOR", x+1, y+1, 1);
	textout(bmp, data[THE_FONT].dat, "EDITOR", x, y, 4);

	y += step;
	textout(bmp, data[THE_FONT].dat, "QUIT", x+1, y+1, 1);
	textout(bmp, data[THE_FONT].dat, "QUIT", x, y, 4);

	draw_sprite(bmp, data[POINTER].dat, x - 25 + fixtoi(3 * fcos(itofix(tick << 2))), 44 + menu_choice * step);
}


// switches gfx mode
void switch_gfx_mode(int mode, int w, int h) {
	log2file(" switching to %d x %d (%s)", w, h, (mode == GFX_AUTODETECT_WINDOWED ? "w" : "f"));
	if (set_gfx_mode(mode, w, h, 0, 0)) {
		log2file("  *** failed");
		log2file("  trying default mode (640x480 win)");
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
			log2file("  *** failed");
			log2file("  trying default mode (640x480 full)");
			if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
				log2file("  *** failed");
				log2file("  exiting program...");
				uninit_game();
				exit(0);
				log2file("\nDone...\n");
			}
		}
	}

	set_palette(data[0].dat);
}




// gets a string from the user
// used for getting highscore names
int get_string(BITMAP *bmp, char *string, int max_size, FONT *f, int pos_x, int pos_y, int colour, Tcontrol *pad) {
    int i = 0, c;
    BITMAP *block = create_bitmap(text_length(f, "w")*max_size + 2, text_height(f) + 2);
	char letters[] = "_abcdefghijklmnopqrstuvwxyz {}";
	int current_letter = 0;
	int max_letter = strlen(letters) - 1;
	int print_delay = 0;
    
    if (block == NULL)
        return 1;

	blit(bmp, block, pos_x - 1, pos_y - 1, 0, 0, block->w, block->h);

    clear_keybuf();
    while(1) {
		cycle_count = 0;
        string[i] = letters[current_letter];
		string[i + 1] = '\0';
        blit(block, bmp, 0, 0, pos_x - 1, pos_y - 1, block->w, block->h);
        textout(bmp, f, string, pos_x, pos_y, colour);
		blit_to_screen(bmp);

		if (pad != NULL) {
			poll_control(pad);
			if (print_delay > 0) print_delay --;
			if (!is_any(pad)) print_delay = 0;
		}
		// get input
		if (keypressed()) {
			c = readkey();
			switch((c >> 8)) {
	            case KEY_BACKSPACE :
					i--;
					i = (i < 0)?0 :i;
					break;
	
				case KEY_ENTER :
					string[i] = '\0';
					destroy_bitmap(block);
					return 0;
					break;
	
				default :
	                if (i < max_size - 2 && isprint(c & 0xff)) {
						string[i] = c & 0xff;
						i++;
						current_letter = 0;
					}
					break;
			}
		}
		else if (pad != NULL && !print_delay) {
			// check pad
			if (is_up(pad) || is_right(pad)) {
				current_letter ++;
				if (current_letter > max_letter) current_letter = 0;
				print_delay = 8;
			}
			if (is_down(pad) || is_left(pad)) {
				current_letter --;
				if (current_letter < 0) current_letter = max_letter;
				print_delay = 8;
			}

			if (is_jump(pad) || is_fire(pad)) {
				print_delay = 50;
				switch(letters[current_letter]) {
					case '{':
						i--;
						i = (i < 0)?0 :i;
						break;
					case '}':
						string[i] = '\0';
						destroy_bitmap(block);
						return 0;
						break;

					default :
			            if (i < max_size - 2) {
							string[i] = letters[current_letter];
							i++;
						}
						break;
				}
			}

		}

		while(!cycle_count);

    }
}

	
// lets the player enter a name for highscore use (or what ever)
void get_player_name(char *name) {
	blit(data[INTRO_BG].dat, swap_screen, 0, 0, 0, 0, 160, 120);
	transform_bitmap(swap_screen, -1);
	textout_outline_center(swap_screen, "Congratulations,", 80, 8);
	textout_outline_center(swap_screen, "You've got", 80, 19);
	textout_outline_center(swap_screen, "a high score!", 80, 30);
	textout_outline_center(swap_screen, "Enter your name:", 80, 55);
	blit_to_screen(swap_screen);
	fade_in_pal(100);
	get_string(swap_screen, name, 10, data[THE_FONT].dat, 50, 80, 4, &ctrl);
}


// title and menu
int do_main_menu() {
	int status = GS_OK;
	int count = 0;
	int tick = 0;

	log2file("\nRunning main menu:");

	draw_title(swap_screen, tick);
	blit_to_screen(swap_screen);
	fade_in_pal(100);

	clear_keybuf();
	cycle_count = 0;
	while(status == GS_OK) {

		//  do logic
		while(cycle_count > 0) {
			logic_count ++;
			tick ++;

			// poll music
			if (got_sound) al_poll_duh(dp);

			scroll_scroller(&hscroll, -1);
			if (!scroller_is_visible(&hscroll)) restart_scroller(&hscroll);

			poll_control(&ctrl);
			if (count) count --;

			// is it ok to check for actions?
			if (!count) {
				// if the user has pressed an action button,
				// handle the different alternatives
				if (key[KEY_SPACE] || key[KEY_ENTER] || is_fire(&ctrl) || is_jump(&ctrl)) {
					if (menu_choice == 1) {
						log2file(" play selected");
						status = GS_PLAY;
						play_sound(sfx[SMPL_MENU]);
					}
					if (menu_choice == 2) {
						log2file(" scores selected");
						status = GS_SCORES;
						play_sound(sfx[SMPL_MENU]);
					}
					if (menu_choice == 3) {
						log2file(" edit selected");
						status = GS_EDIT;
						play_sound(sfx[SMPL_MENU]);
					}
					if (menu_choice == 4) {
						log2file(" quit selected");
						status = GS_QUIT_MENU;
						play_sound(sfx[SMPL_MENU]);
					}
					count = 10;
				}

				// esc is always a shortcut to quit
				if (key[KEY_ESC]) {
					if (menu_choice == 4) {
						log2file(" quit selected");
						status = GS_QUIT_MENU;
					}
					else menu_choice = 4;
					play_sound(sfx[SMPL_MENU]);
					count = 10;
				}

				// movements in the menu
				if (key[KEY_UP] || is_up(&ctrl)) { 
					menu_choice --; 
					if (menu_choice < 1) menu_choice = 4;
					play_sound(sfx[SMPL_MENU]); 
					count = 10; 
				}
				if (key[KEY_DOWN] || is_down(&ctrl)) {
					menu_choice ++; 
					if (menu_choice > 4) menu_choice = 1;
					play_sound(sfx[SMPL_MENU]); 
					count = 10; 
				}
			}

			// releasing all these keys, and you cen press them again
			if (!is_any(&ctrl) && !key[KEY_UP] && !key[KEY_ESC] && !key[KEY_DOWN] && !key[KEY_SPACE] && !key[KEY_ENTER]) count = 0;

			// shortcuts to gfx modes
			if (key[KEY_1]) { while(key[KEY_1]); switch_gfx_mode(GFX_AUTODETECT_WINDOWED, 160, 120); }
			if (key[KEY_2]) { while(key[KEY_2]); switch_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240); }
			if (key[KEY_3]) { while(key[KEY_3]); switch_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480); }
			if (key[KEY_4]) { while(key[KEY_4]); switch_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480); }

			cycle_count --;
		}

		// let other processes play
		yield_timeslice();

		// draw 
		frame_count ++;
		draw_title(swap_screen, tick);
		blit_to_screen(swap_screen);
	}

	// user selected EDIT
	if (status == GS_EDIT) {
		stop_music();
		fix_gui_colors();
		log2file("\nEntering editor:");
		fade_out_pal(100);

		// create an empty map for the user
		log2file(" creating empty map");
		if (map != NULL) destroy_map(map);
		map = create_map(50, 7);
		
		// if all went well, start editing
		if (map != NULL) {
			map->data = data;

			new_game(1);
			init_player(&player, map);
			init_map(map);

			reset_particles(particle, MAX_PARTICLES);
			reset_bullets(bullet, MAX_BULLETS);
			
			editing = TRUE;
			set_edit_mode(EDIT_MODE_DRAW);
			set_edit_path_and_file("new.map");

			draw_frame(swap_screen, 1);
			blit_to_screen(swap_screen);
			fade_in_pal(100);
			status = play(-1);
			deinit_map(map);
		}
		else {
			log2file(" *** failed");
		}
		if (got_sound) start_music(MOD_MENU_SONG);
	}
	else if (status == GS_PLAY) {  //// user selected PLAY
		int level = 0;

		log2file("\nStarting new game:");
		fade_out_pal(100);

		game_status = GS_OK;
		editing = FALSE;
		new_game(1);

		// select starting level
		if (options.max_levels && playing_original_game) {
			level = select_starting_level() - 1;
			fade_out_pal(100);

			if (level == -2) {  // esc
				log2file(" cancelled");
				game_status = status = GS_QUIT_GAME;
			}

			if (level == -101) {  // shooter
				log2file(" shooter selected");
				stop_music();
				start_shooter(&ctrl, got_sound);
				game_status = status = GS_QUIT_GAME;
				start_music(MOD_MENU_SONG);
			}

		}  

		// start playing
		while(game_status == GS_OK || game_status == GS_LEVEL_DONE) {
			// select level 
			if (playing_original_game) {
				log2file(" starting level <%d>", level);
				new_level("datafile", level, 1);
			}
			else {
				log2file(" starting level <%s>", level_files[level]);
				set_edit_path_and_file(level_files[level]);
				new_level(level_files[level], -1, 1);
			}

			if (got_sound) {
				if (map->boss_level) start_music(MOD_BOSS_SONG);
				else start_music(MOD_LEVEL_SONG);
			}

			// actual game starts here
			show_lets_go();
			status = play(level);
			// done playing level

			deinit_map(map);

			// act on different outcomes
			if (status == GS_GAME_DIED) {
				log2file(" player died");
				if (player.lives == 0) {
					game_status = GS_GAMEOVER;
				}
				else {
					game_status = GS_OK;
					fade_out_pal(200);
				}
			}
			else if (status == GS_QUIT_GAME) {
				log2file(" player quit");
				if (got_sound) stop_music();
				fade_out_pal(100);
				if (got_sound) start_music(MOD_MENU_SONG);
			}
			else {
				PACKFILE *pf;
				log2file(" level complete");
				if (got_sound) stop_music();
				if (level < MAX_LEVELS && playing_original_game) {
					int i;
					int ace = 1;
					if (player.cherries == player.cherries_taken) {
						options.cherries[level] = 100;
						log2file("   all cherries taken");
					}
					if (player.stars == player.stars_taken) {
						options.stars[level] = 100;
						log2file("   all stars taken");
					}

					// check if all levels are aced
					for(i = 0; i < num_levels; i ++) {
						if (options.cherries[i] != 100) ace = 0;
						if (options.stars[i] != 100) ace = 0;
					}
					if (ace) options.one_hundred = 1;
				}
				level ++;
				show_cutscene(level);
				if (level == num_levels) {
					log2file(" game completed");
					game_status = GS_GAME_COMPLETE;
				}
				if (level > options.max_levels && game_status != GS_GAME_COMPLETE) {
					options.max_levels = level;
				}
				fade_out_pal(100);

				// save options
				log2file(" saving options");
				pf = pack_fopen("alex4.sav", "wp");
				if (pf) {
					save_options(&options, pf);
					pack_fclose(pf);
				}
			}
		}

		// are we done or dead?
		if (game_status == GS_GAMEOVER || game_status == GS_GAME_COMPLETE) {
			Thisc post;

			if (game_status == GS_GAMEOVER) {
				log2file(" game over");
				show_game_over();
			}
			else { // game complete
				if (playing_original_game) {
					start_music(MOD_OUTRO_SONG);
					run_script((char *)data[SCR_OUTRO].dat, data);
					stop_music();
				}
				else {
					show_custom_ending();
				}
				clear_to_color(swap_screen, 4);
				blit_to_screen(swap_screen);
			}

			stop_music();

			// build post
			post.level = level + 1;
			post.score = player.score;			

			// check if player got a high score
			if (qualify_hisc_table(hisc_table, post)) {
				fade_out_pal(100);

				log2file(" player qualified for highscore (%d, %d)", post.level, post.score);

				// get player name
				get_player_name(post.name);
				log2file(" score logged as <%s>", post.name);

				enter_hisc_table(hisc_table, post);
				sort_hisc_table(hisc_table);
			}

			if (got_sound) start_music(MOD_MENU_SONG);
			status = GS_SCORES;									
		}
		
	}

	// show highscores? 
	// can be entered from both menu and end of game
	if (status == GS_SCORES) {
		fade_out_pal(100);
		log2file(" showing scores");
		show_scores(0, hisc_table);
		if (options.one_hundred) {
			show_scores(1, hisc_table_space);
		}
	}
	
	// user quit
	if (status == GS_QUIT_MENU) {
		stop_music();
		fade_out_pal(100);
		// show bye bye screen
		blit(data[INTRO_BG].dat, swap_screen, 0, 0, 0, 0, 160, 120);
		transform_bitmap(swap_screen, -1);
		textout_outline_center(swap_screen, "Thanks for playing!", 80, 20);
		textout_outline_center(swap_screen, "Design, Code, GFX:", 80, 48);
		textout_outline_center(swap_screen, "Johan Peitz", 80, 60);
		textout_outline_center(swap_screen, "MUSIC, SFX:", 80, 78);
		textout_outline_center(swap_screen, "Anders Svensson", 80, 90);
		blit_to_screen(swap_screen);
		fade_in_pal(100);
		cycle_count = 0;
		while(!key[KEY_ESC] && cycle_count < 200);
		fade_out_pal(100);
		clear(screen);
	}

	return status;
}




// main
int main(int argc, char **argv) {   
	FILE *fp;
	int i;
	char full_path[1024];

	// init allegro
	allegro_init();

	// get working directory
	get_executable_name(full_path, 1024);
	replace_filename(working_directory, full_path, "", 1024);
	chdir(working_directory);


	// start logfile
	fp = fopen("log.txt", "wt");
	if (fp) {
		fprintf(fp, "Alex 4 (%s) - log file\n-------------------\n", GAME_VERSION_STR);
		fclose(fp);
	}

	// log program arguments
	log2file("Game started with the following commands:");
	for(i = 0; i < argc; i ++) {
		log2file("   %s", argv[i]);
	}
	log2file("Working directory is:\n   %s", working_directory);

	// test wether to play real game
	// or custom levels
	if (argc == 1) playing_original_game = TRUE;
	else playing_original_game = FALSE;

	// init game
	if (init_game((playing_original_game ? "maps from datafile please" : argv[1]))) {
		if (playing_original_game) {
			start_music(MOD_INTRO_SONG);
			if (run_script((char *)data[SCR_INTRO].dat, data) < 0) {
				fade_out_pal(100);
			}
			stop_music();
		}
		if (got_sound) start_music(MOD_MENU_SONG);
		while(do_main_menu() != GS_QUIT_MENU);
	}
	else {
		log2file("*** init failed!");
		allegro_message("ALEX4:\nFailed to start game.");
	}

	// tidy up
	uninit_game();
	allegro_exit();
	log2file("\nDone...\n");

	return 0;
} END_OF_MAIN(); 






