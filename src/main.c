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
 
#define _XOPEN_SOURCE 700 // for strdup
 
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include "alex4.h"
#include "sdl_port.h"
#include "hisc.h"
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
#include "misc.h"
#include "sound.h"
#ifdef ENABLE_EDITOR
#include "edit.h"
#endif
#ifdef ENABLE_SHOOTER
#include "shooter.h"
#endif
#include "unix.h"

#include "data.h"

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
BITMAP *swap_screen;
Tscroller hscroll;
Thisc *hisc_table;
#ifdef ENABLE_SHOOTER
Thisc *hisc_table_space;
#endif

// the map
Tmap *map = NULL;

// controls
Tcontrol ctrl;

// actors
Tactor actor[MAX_ACTORS];

// edit
int editing = 0;

// various
char scroller_message[] = 
	"Free Lunch Design      presents      Alex the Allegator 4      "
	"Guide Alex to the exit of each level      Jump or shoot enemies picking up stars and cherries on the way      "
	"Use arrows to move Alex, ALT to jump and Left CTRL to shoot, or use a gamepad or joystick      "
	"Code and GFX by Johan Peitz      Music and SFX by Anders Svensson      SDL2 port by carstene1ns       ";

char *level_files[256];
int num_levels;
int got_sound = 0;
Toptions options;
int menu_choice = 1;
int playing_original_game = 1;
int init_ok = 0;

FILE* log_fp = NULL;

// // // // // // // // // // // // // // // // // // // // // 

// returns pointer to the game over bitmap
BITMAP *get_gameover_sign() {
	return bitmaps[I_GAME_OVER];
}

// returns pointer to the lets go bitmap
BITMAP *get_letsgo_sign() {
	return bitmaps[I_LETSGO];
}

#ifdef ENABLE_SHOOTER
// returns pointer to the space highscore table
Thisc *get_space_hisc() {
	return hisc_table_space;
}
#endif

// sets the current map
void set_map(Tmap *m) {
	map = m;
}

// returns the player actor
Tactor *get_alex() {
	return &actor[0];
}

/// load the level filenames
void load_level_files(const char *filename) {
	FILE *fp;
	char buf[1024];
	int mode = 0;
	char *ret;
	char *path;
	
	// get path to maps
	path = strdup(filename);
	path = dirname(path);

	// reset counters
	num_levels = 0;

	// open level file
	fp = fopen(filename, "rt");
	if (!fp) {
		printf("%s not found\n", filename);
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
		printf("#start# not found\n");
		log2file("  *** #start# not found");
	}
		
	// read level lines until #end#
	while(ret != NULL && mode == 1) {
		// read levels until #end# token is found
		if (!strncmp("#end#", buf, 5)) {
			mode = 2;
		}
		else {
			char full_path_to_map[2048];

			clear_trailing_whitespace(buf);
			sprintf(full_path_to_map, "%s%s", path, buf);
			level_files[num_levels] = strdup(full_path_to_map);

			// test if map exists
			if (exists(level_files[num_levels])) {
				num_levels ++;
				log2file("  <%s> found", buf);
			}
			else {
				printf("ALEX4:\n<%s> not found - skipping\n", level_files[num_levels]);
				log2file("  *** <%s> not found - skipping", level_files[num_levels]);
			}
		}

		ret = fgets(buf, 1024, fp);  // read a line
	}

	free(path);

	// close file
	fclose(fp);

	return;
}

// draws the status bar
void draw_status_bar(BITMAP *bmp, int y) {
	int i;

	rectfill(bmp, 0, y, 160, y+10, 1);
	draw_sprite_h_flip(bmp, bitmaps[I_HERO_NORM], 0, y+1); 
	textprintf_ex(bmp, 9, y+1, 4, -1, " :%d", player.lives);

	for(i = 0; i < player.health; i ++)
		draw_sprite(bmp, bitmaps[I_HEART2], 40 + 10 * i, y-3);

	draw_sprite(bmp, bitmaps[I_EGG], 80, y-5); 
	textprintf_ex(bmp, 85, y+1, 4, -1, " :%d", player.ammo);

	textprintf_right_ex(bmp, 158, y+1, 4, -1, "%d", player.score);
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
	//blit(bitmaps[I_BG0], bmp, 0, 0, x0, 0, 160, 120);
	//blit(bitmaps[I_BG0], bmp, 0, 0, x0 + 160, 0, 160, 120);
	draw_sprite(bmp, bitmaps[I_BG0], x0, 0);
	draw_sprite(bmp, bitmaps[I_BG0], x0 + 160, 0);
	draw_sprite(bmp, bitmaps[I_BG1], x1, 120 - bitmaps[I_BG1]->h);
	draw_sprite(bmp, bitmaps[I_BG1], x1 + 160, 120 - bitmaps[I_BG1]->h);
	draw_sprite(bmp, bitmaps[I_BG2], x2, 120 - bitmaps[I_BG2]->h);
	draw_sprite(bmp, bitmaps[I_BG2], x2 + 160, 120 - bitmaps[I_BG2]->h);

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
#if ENABLE_EDITOR
	else { 		/////////////// EDIT stats
		int mx = GetMouseX() / (GetScreenW() / SCREEN_W);
		int my = GetMouseY() / (GetScreenH() / SCREEN_H);
		draw_edit_mode(bmp, map, mx, my);
	}
#endif
}

// invoked when game looses focus
void display_switch_out(void) {
	if (got_sound) {
		pause_music(true);
		rest(100);
	}
}

// invoked when game regains focus
void display_switch_in(void) {
	pause_music(false);
}

// init the game
int init_game(const char *map_file) {
	int i;
	char filename[512];

	log2file("\nInit routines:");

	// load options
	log2file(" loading options");
	reset_options(&options);
#ifdef __unix__
	char *homedir = get_homedir();
	snprintf(filename, sizeof(filename), "%s/.alex4/config",
		homedir? homedir:".");
#else
	strcpy(filename, "config.ini");
#endif
	if(!load_options(&options, filename))
		log2file("  *** failed, using defaults");
	
	// install timers
	log2file(" installing timers");
	install_timers();
	
	// init gfx
	log2file(" entering gfx mode set in config (%dx%d %s)",
		options.width, options.height, options.fullscreen ? "full" : "win");
	make_sdl_window(&options);

	// set win title (no! really???)
	log2file(" setting window title");
	set_window_title("loading...");

	// allocating memory 
	log2file(" allocating memory for off screen buffers");
	swap_screen = create_bitmap(160, 120);
	if (swap_screen == NULL) {
		log2file("  *** failed");
		printf("ALEX4:\nFailed to allocate memory for swap screen.\n");
		return false;
	}

	log2file(" allocating memory for high score table(s)");
	hisc_table = make_hisc_table();
	if (hisc_table == NULL) {
		log2file("  *** failed");
		printf("ALEX4:\nFailed to allocate memory for high score list.\n");
		return false;
	}

#ifdef ENABLE_SHOOTER
	hisc_table_space = make_hisc_table();
	if (hisc_table_space == NULL) {
		log2file("  *** failed");
		printf("ALEX4:\nFailed to allocate memory for high score list.\n");
		return false;
	}
#endif

	// show initial loading screen
	clear_bitmap(swap_screen);
	textout_centre_ex(swap_screen, "loading...", 320, 200, 1, -1);
	blit_to_screen(swap_screen);

	// load data
	log2file(" loading data");
	if (!exists(DATADIR "/data.zip")) {
		log2file("  *** failed");
		printf("ALEX4:\n data file %s not found.\n", DATADIR "/data.zip");
		return false;
	}
	if (!load_datafile(DATADIR "/data.zip")) {
		log2file("  *** failed");
		printf("ALEX4:\nFailed to load data.\n");
		return false;
	}

	// font
	prepare_font(F_GAME);

	// loading highscores
	bool high_ok = true;
	log2file(" loading hiscores");
#ifdef __unix__
	snprintf(filename, sizeof(filename), "%s/.alex4/highscore",
		homedir? homedir:".");
#else
	strcpy(filename, "highscore.dat");
#endif
	if (!exists(filename)) high_ok = false;
	if(high_ok)
		high_ok = load_hisc_table(hisc_table, filename);

#ifdef ENABLE_SHOOTER
	if(high_ok) {
#ifdef __unix__
		strcat(filename, "-shooter")
#else
		strcpy(filename, "highscore-shooter.dat");
#endif
		high_ok = load_hisc_table(hisc_table_space, filename);
	}
#endif

	if(!high_ok) {
		log2file("  *** failed, resetting");
		reset_hisc_table(hisc_table, "alex", 25000, 5000);
		sort_hisc_table(hisc_table);
#ifdef ENABLE_SHOOTER
		reset_hisc_table(hisc_table_space, "Lola", 3000000, 600000);
		sort_hisc_table(hisc_table_space);
#endif
	}

	// show splash screen
	clear_to_color(swap_screen, 3);

	BITMAP *bmp = bitmaps[I_FLD_LOGO];
	draw_character_ex(swap_screen, bmp, 80 - bmp->w / 2 + 0, 50 + 1, 1);
	draw_character_ex(swap_screen, bmp, 80 - bmp->w / 2, 50, 4);
	
	blit_to_screen(swap_screen);
	// show a bit
	//rest(400);

	// load maps
	if (playing_original_game) {
		log2file(" using original maps");
		num_levels = MAP_MAX;

		if (!maps) {
			log2file("  *** failed");
			printf("ALEX4:\nFailed to load original maps.\n");
			return false;
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
				printf("ALEX4:\nCustom map file must\nhold at least one\nlegal map file.\n");
				return false;
			}
		}
		else {
			log2file("  *** file not found: %s", map_file);
			printf("ALEX4:\nCustom map file not found:\n%s\n", map_file);
			return false;
		}		
	}

	// init control
	log2file(" initializing controls and joystick/gamepad");
	init_control(&ctrl);
	update_sdl_keyboard();

	// install sound
	log2file(" installing sound/music");
#ifdef NO_SOUND
	log2file("  *** disabled");
	got_sound = 0;
#else
	Mix_Init(MIX_INIT_MOD);
	if(Mix_OpenAudio(options.sound_freq, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, options.buffer_size) != 0) {
		log2file("  *** failed");
		got_sound = 0;
	} else {
		Mix_AllocateChannels(16);
		got_sound = 1;

		load_sfx();
		load_music(MSC_GAME);

		set_sound_volume(options.sample_volume);
		set_music_volume(options.music_volume);
	}
#endif

	// misc
	log2file(" initializing scroller");
	init_scroller(&hscroll, scroller_message, 160, 10, true);

	// done!
	play_sound_id(S_STARTUP); 
	wait_key(2);
	fade_out_pal(swap_screen, 100);


	init_ok = 1;
	log2file(" init OK!");
	return true;
}

// uninits the game
void uninit_game() {
	int i;
	FILE *pf;
#ifdef __unix__
	char filename[512];
	char *homedir = get_homedir();
#endif

	log2file("\nExit routines:");

	log2file(" unloading datafiles");
	unload_data();

	log2file(" destroying temporary map");
	if (map != NULL) destroy_map(map);

	log2file(" freeing level names");
	for(i = 0; i < num_levels; i ++)
		free(level_files[i]);

	// only save if everything was inited ok!
	if (init_ok) {
		log2file(" saving options");
#ifdef __unix__
		snprintf(filename, sizeof(filename), "%s/.alex4/config",
			homedir? homedir:".");
#else
		strcpy(filename, "config.ini");
#endif
		save_options(&options, filename);

#ifdef __unix__
		snprintf(filename, sizeof(filename), "%s/.alex4/highscore",
			homedir? homedir:".");
#else
		strcpy(filename, "highscore.dat");
#endif
		save_hisc_table(hisc_table, filename);
#ifdef ENABLE_SHOOTER
#ifdef __unix__
		strcat(filename, "-shooter")
#else
		strcpy(filename, "highscore-shooter.dat");
#endif
		save_hisc_table(hisc_table_space, pf);
#endif
	}

	if (got_sound) {
		log2file(" stopping music");
		stop_music();
		log2file(" freeing sounds");
		free_sfx();
		Mix_CloseAudio();
	}

	log2file(" exiting SDL");
#ifndef NO_SOUND
	Mix_Quit();
#endif
	SDL_Quit();
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
	textout_ex(bmp, txt, x+1, y, 1, -1);
	textout_ex(bmp, txt, x-1, y, 1, -1);
	textout_ex(bmp, txt, x, y+1, 1, -1);
	textout_ex(bmp, txt, x, y-1, 1, -1);
	textout_ex(bmp, txt, x, y, 4, -1);
}

// draws centered text with an outline
void textout_outline_center(BITMAP *bmp, const char *txt, int cx, int y) {
	int x = cx - text_length(txt) / 2;
	textout_outline(bmp, txt, x, y);
}

// plays the let's go sequence
void show_lets_go() {
	BITMAP *go = bitmaps[I_LETSGO];
	int x = -go->w;
	int mode = 0;
	int wait = 0;
	int y = 120, ty = 60;
	int dy = 0;

	set_window_title(map->name);

	cycle_count = 0;
	while(mode != 3) {
		// Let other processes play
		while(cycle_count == 0)
			rest(1);

		// do logic
		while(cycle_count > 0) {
			logic_count ++;

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

		// draw stuff
		draw_frame(swap_screen, 1);
		draw_sprite(swap_screen, go, x, 35);
		textout_outline_center(swap_screen, map->name, 80, y);
		blit_to_screen(swap_screen);
	
	}
}

// shows the game over sign sequence
void show_game_over() {
	BITMAP *go = bitmaps[I_GAME_OVER];
	
	set_window_title("GAME OVER");

	int x = -go->w;
	int mode = 0;
	int wait = 0;

	cycle_count = 0;
	while(mode != 3) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		// do logic
		while(cycle_count > 0) {
			logic_count ++;
			update_sdl_keyboard();

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
	BITMAP *head = bitmaps[I_FLD_HEAD];

	set_window_title("GOOD ENDING");

	blit(bitmaps[I_INTRO_BG], bmp, 0, 0, 0, 0, 160, 120);

	// FIXME: STUB ending
	draw_sprite(bmp, head, 64, 6);
	#if 0
	r = 70 + fixtoi(20 * fixcos(itofix(game_count >> 1)) + 20 * fixsin(itofix((int)(game_count / 2.7))) );
	for(i = 0; i < 256; i += 32) 
		draw_sprite(bmp, head, 80 - head->w/2 + fixtoi(r * fixcos(itofix(game_count + i))), 60 - head->h/2 + fixtoi(r * fixsin(itofix(game_count + i))));
	#endif

	draw_sprite_h_flip(bmp, bitmaps[I_ALEX], 60, 40);
	draw_sprite(bmp, bitmaps[I_LOLA], 84, 40);

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
	fade_in_pal(swap_screen, 100);
	game_count = tmp;

	cycle_count = 0;
	while(!done) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// poll user
			poll_control(&ctrl);
			if (is_fire(&ctrl) || is_jump(&ctrl) || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_ESC]) 
				done = 1;

			// decrease time
			if (--wait < 0) done = 1;

			// move on
			cycle_count --;
		}

		// draw stuff
		draw_custom_ending(swap_screen);
		blit_to_screen(swap_screen);
	}
}

// draws the scoring sequence at end of level
// used by show_cutscene(..)
void draw_cutscene(BITMAP *bmp, int org_level, int _level, int _lives, int _stars, int _cherries) {
	BITMAP *go = bitmaps[I_LEVELCOMPLETE];
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
	start_music(MOD_LEVEL_DONE);

	// create cutscene screene
	blit(swap_screen, swap2, 0, 0, 0, 0, 160, 120);
	transform_bitmap(swap2, -1);
	draw_cutscene(bmp, level, _level, _lives, _stars, _cherries);

	// scroll bmp onto swap_screen
	cycle_count = 0;
	while(mode != 3) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		// do logic
		while(cycle_count > 0) {
			logic_count ++;
			my_counter ++;
			poll_control(&ctrl);

			if (((mode == 1) && (keypressed() || is_fire(&ctrl) || is_jump(&ctrl))) || my_counter > 200) {
				mode = 2;
			}
			
			// move text
			if (mode == 0) x += 8;
			if (x == 0 && mode == 0) {
				mode = 1;
			}

			// count stats
			if (mode == 2 && !(game_count % 4)) {
				int a = 0;
				if (_level)    { player.score +=  100; _level    -= 100; a++; }
				if (_lives)    { player.score +=  100; _lives    -= 100; a++; }
				if (_stars)    { player.score +=  100; _stars    -= 100; a++; }
				if (_cherries) { player.score +=   10; _cherries -=  10; a++; }
				if (!a) mode = 3;

				play_sound_id(S_POINT);

				draw_cutscene(bmp, level, _level, _lives, _stars, _cherries);
			}

			// move on
			cycle_count --;
		}

		// draw stuff
		blit(swap2, swap_screen, 0, 0, 0, 0, 160, 120);
		draw_status_bar(swap_screen, 110);
		draw_sprite(swap_screen, bmp, x, 0);
		blit_to_screen(swap_screen);
	}

	wait_key(5);

	stop_music();
	destroy_bitmap(bmp);
	destroy_bitmap(swap2);
}

// shows a highscore table
void show_scores(int space, Thisc *table) {
	DATAFILE *df = NULL;
	BITMAP *bg = NULL;

#ifdef ENABLE_SHOOTER
	if (space) {
		// get space bg
		packfile_password(init_string);
		df = load_datafile_object(DATADIR "a45.dat", "BG1");
		packfile_password(NULL);
		if (df != NULL)	{
			bg = df->dat;
		}
		else
			msg_box("ooga");
	}
#endif

	if (bg == NULL || !space)
		blit(bitmaps[I_INTRO_BG], swap_screen, 0, 0, 0, 0, 160, 120);
	else {
		clear_to_color(swap_screen, 1);
		blit(bg, swap_screen, 0, 0, 0, 0, 160, 120);
	}

	textout_outline_center(swap_screen, "High scores", 80, 8);
	textout_outline_center(swap_screen, "Press any key", 80, 100);
	draw_hisc_table(table, swap_screen, 10, 30, (space ? 4 : 1), !space);

	blit_to_screen(swap_screen);
	fade_in_pal(swap_screen, 100);

	poll_control(&ctrl);
	while(!is_jump(&ctrl) && !keypressed()) {
		poll_control(&ctrl);
	}
	play_sound_id(S_MENU);

	fade_out_pal(swap_screen, 100);

#ifdef ENABLE_SHOOTER
	// clean up
	if (df != NULL) unload_datafile_object(df);
#endif
}

// draws the level selector
void draw_select_starting_level(BITMAP *bmp, int level, int min, int max) {
	BITMAP *stuff = create_bitmap(40, 10);
	char buf[80];
	int xpos = 2;

	blit(bitmaps[I_ALEX_BG], bmp, 0, 0, 0, 0, 160, 112);
	rectfill(bmp, 0, 112, 160, 120, 2);

	sprintf(buf, "%s %d %s", (level > min ? "<" : " "), level, (level < max ? ">" : " "));
	clear_bitmap(stuff);
	textout_centre_ex(stuff, buf, stuff->w/2 + 1, 1, 2, -1);
	textout_centre_ex(stuff, buf, stuff->w/2, 0, 1, -1);
	stretch_sprite(bmp, stuff, 80 - 4*stuff->w/2, 30, 4*stuff->w, 4*stuff->h);

	textout_centre_ex(bmp, "SELECT START LEVEL", 80, 90, 1, -1);
	textout_centre_ex(bmp, "SELECT START LEVEL", 79, 89, 4, -1);

#if ENABLE_SHOOTER
	if (options.one_hundred) {
		if (game_count & 32 || game_count & 16) draw_sprite(bmp, bitmaps[I_SHIP100], xpos, 2);
	}
	else
#endif
	{
		if (options.stars[level - 1]) {
			draw_sprite(bmp, bitmaps[I_STAR], xpos, 2);
			draw_sprite(bmp, bitmaps[I_ALL100], xpos + 4, 14);
			xpos += 20;
		}
		if (options.cherries[level - 1]) {
			draw_sprite(bmp, bitmaps[I_CHERRY], xpos, 2);
			draw_sprite(bmp, bitmaps[I_ALL100], xpos + 4, 14);
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
	fade_in_pal(swap_screen, 100);

	cycle_count = 0;
	while(!done) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		// do logic
		while(cycle_count > 0) {
			logic_count ++;

			// check controls
			poll_control(&ctrl);
			if (is_right(&ctrl) && start_level < options.max_levels + 1 && !counter) {
				start_level ++;
				play_sound_id(S_MENU);
			}

			if (is_left(&ctrl) && start_level > 1 && !counter) {
				start_level --;
				play_sound_id(S_MENU);
			}

			if (is_jump(&ctrl) || is_fire(&ctrl)) {
				done = 1;
				play_sound_id(S_MENU);
			}
			if (keypressed()) {
				int scancode = readkey();
				if (scancode == KEY_SPACE || scancode == KEY_ENTER) {
					done = 1;
					play_sound_id(S_MENU);
				}
				if (scancode == KEY_F1 && options.one_hundred) {
					done = 1;
					start_level = -100;
					play_sound_id(S_MENU);
				}
				if (scancode == KEY_ESC) {
					done = 1;
					start_level = -1;
					play_sound_id(S_MENU);
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
	actor[0].frames[0] = I_HERO000;
	actor[0].frames[1] = I_HERO001;
	actor[0].frames[2] = I_HERO002;
	actor[0].frames[3] = I_HERO003;
	actor[0].frames[4] = I_HERO_NORM;
	actor[0].num_frames = 4;
	actor[0].frame = 0;
	actor[0].anim_counter = 0;
	actor[0].anim_max = 4;
}

// tidies up after a map has been used
void deinit_map(void) {
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
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY1;
					a->num_frames = 4;					
					a->frames[0] = I_ENEMY1_01;
					a->frames[1] = I_ENEMY1_02;
					a->frames[2] = I_ENEMY1_03;
					a->frames[3] = I_ENEMY1_04;
					a->flat_frame = I_ENEMY1_05;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags = ACF_JUMPABLE | ACF_FLATABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY2) { // big human
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY2;
					a->num_frames = 4;					
					a->frames[0] = I_ENEMY2_01;
					a->frames[1] = I_ENEMY2_02;
					a->frames[2] = I_ENEMY2_03;
					a->frames[3] = I_ENEMY2_04;
					a->flat_frame = I_ENEMY2_05;
					a->w = 12; a->h = 19;
					a->ox = 2; a->oy = 5;
					a->flags = ACF_JUMPABLE | ACF_FLATABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY3) { // crusher
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY3;
					a->num_frames = 1;					
					a->frames[0] = I_ENEMY3;
					a->w = 30; a->h = 16;
					a->ox = 1; a->oy = 112-16;
					a->flags = ACF_HURTS;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY4) { // spike fish
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY4;
					a->num_frames = 1;					
					a->frames[0] = I_ENEMY4;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags =  ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY5) { // yelly fish
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY5;
					a->num_frames = 1;					
					a->frames[0] = I_ENEMY5_01;
					a->w = 12; a->h = 14;
					a->ox = 2; a->oy = 2;
					a->flags = ACF_JUMPABLE | ACF_HURTS | ACF_SHOOTABLE | ACF_ROLLABLE;
					m->num_enemies ++;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_ENEMY6) { // cannon
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_ENEMY6;
					a->num_frames = 0;					
					a->flags = 0;
				}
			}
			if (m->dat[x + y * m->width].type == MAP_GUARD1) { // spike-car
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_GUARD1;
					a->energy = 100;
					a->num_frames = 4;					
					a->hit_offset = 4;					
					a->frames[0] = I_GUARD1_1;
					a->frames[1] = I_GUARD1_2;
					a->frames[2] = I_GUARD1_3;
					a->frames[3] = I_GUARD1_4;
					a->w = 32; a->h = 32;
					a->ox = 0; a->oy = 0;
					a->flags = ACF_HURTS | ACF_ROLLABLE_BACK;
					m->num_enemies ++;
					a->sound = S_ENGINE;
					play_sound_id_ex(a->sound, 100, 1000, 1);
				}
			}
			if (m->dat[x + y * m->width].type == MAP_GUARD2) { // spike-jumper-crusher
				Tactor *a = make_actor(actor, x << 4, (y << 4) + 16);
				if (a != NULL) {
					a->type = MAP_GUARD2;
					a->energy = 150;
					a->num_frames = 2;					
					a->frames[0] = I_GUARD2_1A;
					a->frames[1] = I_GUARD2_1B;
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
void new_level(const char *fname, int level_id, int draw) {
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
			log2file(" getting script map %d from datafile", atoi(fname));
			map = load_scriptmap_from_memory(atoi(fname));
		}
	}
	else {
		// get map from data file
		log2file(" getting map %d from datafile", level_id);
		map = load_map_from_memory(level_id);
	}

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
		fade_in_pal(swap_screen, 100);
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
					set_bullet(b, (int)player.actor->x + 7, (int)player.actor->y - 14, (player.actor->direction ? 4 : -4), 0, bitmaps[I_EGG2], 0);
					player.ammo --;
					play_sound_id(S_SPIT);
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
		if (is_ground(map, player.actor->x + x1, player.actor->y - 1))  { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound_id(S_HIT); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x2, player.actor->y - 1))  { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound_id(S_HIT); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x1, player.actor->y - 15)) { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound_id(S_HIT); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
		if (is_ground(map, player.actor->x + x2, player.actor->y - 15)) { player.actor->x = old_x; if (player.actor->status == AC_BALL) play_sound_id(S_HIT); player.actor->status = (player.actor->status == AC_BALL ? AC_NORM : player.actor->status); };
	
		
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
			play_sound_id(S_JUMP);
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
				play_sound_id(S_EAT);
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
				play_sound_id(S_HEART);
			}
			if (mp->item == MAP_STAR) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.score += 100;
				player.stars_taken ++;
				play_sound_id(S_STAR);

				if (player.stars == player.stars_taken) {
					Tparticle *p;
					p = get_free_particle(particle, MAX_PARTICLES);
					if (p != NULL) {
						set_particle(p, (tx << 4) + 4, ty << 4, 0, -0.5, 0, 50, I_ALL100);
					}
				}
			}
			if (mp->item == MAP_CHERRY) {
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.score += 10;
				player.cherries_taken ++;
				play_sound_id(S_CHERRY);
 
				if (player.cherries == player.cherries_taken) {
					Tparticle *p;
					p = get_free_particle(particle, MAX_PARTICLES);
					if (p != NULL) {
						set_particle(p, (tx << 4) + 8, ty << 4, 0, -0.5, 0, 50, I_ALL100);
					}
				}
			}
			else if (mp->item == MAP_1UP) {
				create_burst(particle, (tx << 4) + 7, (ty << 4) + 7, 4, 32, 0, -1);
				mp->item = 0;
				if (player.actor->status != AC_BALL) player.actor->status = AC_EAT;
				player.eat_counter = 0;
				player.lives ++;
				play_sound_id(S_XTRALIFE);
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
				play_sound_id(S_DIE);
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
				play_sound_id(S_CRUSH);
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
						play_sound_id(S_TURN);
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
						play_sound_id(S_KILL);
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
						if (player.actor->status == AC_DEAD) play_sound_id(S_DIE);
						else play_sound_id(S_HURT);
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
							set_particle(p, actor[e].x + (actor[e].w >> 1), actor[0].y, 0, 0, 0, 5, I_PARTICLE_BOPP);
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
							player.jumping = true;
							play_sound_id(S_JUMP);
							if (actor[e].mode == 0) {
								actor[e].mode = 4;
								actor[e].counter = 0;
							}
						}
						else {
							player.dy = -5;
							//play_sound((actor[e].type == MAP_ENEMY5 ? sfx[S_KILL] : sfx[S_STOMP]));
							play_sound_id(S_STOMP);
							if (is_jump(&ctrl) ) {
								player.dy = -20;	// jump!
								player.jumping = true;
								play_sound_id(S_JUMP);
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
									play_sound_id(S_KILL);	
								}
								else if (actor[e].flags & ACF_ROLLABLE_BACK) {
									if (actor[e].direction == actor[0].direction) {
										actor[e].energy --;
										actor[e].is_hit = 1;
										play_sound_id(S_HIT);	
									}
									else wounded = 1;
								}
								else wounded = 1;
							}
							else wounded = 1;

							if (!player.wounded && wounded) {
								clear_bitmap(swap_screen);
								wound_player(&player);
								if (player.actor->status == AC_DEAD) {
									actor[0].direction = actor[e].direction;
									play_sound_id(S_DIE);
								}
								else {
									play_sound_id(S_HURT);
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
							play_sound_id(S_EAT);
						}
					}
				}
			}
		}
	}
}

// calculates camera pos for map m considering player p
void calculate_camera_pos(Tplayer *p, Tmap *m) {
	static int camera_type = 1;

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
	if (!editing) pause_music(true);
	play_sound_id(S_PAUSE);

	// darken screen
	transform_bitmap(bg, -1);
	blit_to_screen(bg);
	
	// show text
	textout_outline_center(bg, (editing ? "<<< EDITOR PAUSED >>>" : "<<< GAME PAUSED >>>"), 80, 40);
	textout_outline_center(bg, "esc to quit", 80, 60);
	textout_outline_center(bg, "any key to resume", 80, 70);
	blit_to_screen(bg);

	// wait to release esc
	while(key[KEY_ESC])
		update_sdl_keyboard();
	
	// wait for user input
	while(!done) {
		poll_control(&ctrl);
		if (is_fire(&ctrl) || is_jump(&ctrl)) done = 1;
		if (keypressed()) done = 1;
		if (key[KEY_ESC]) done = -1;
		rest(20);
	}

	if (done == -1) {
		game_status = GS_QUIT_GAME;
		play_sound_id(S_MENU);
	}
	else {
		play_sound_id(S_PAUSE);
	}

	if (!editing) pause_music(false);

	return done;
}

// play the game!
int play(void) {
	int i;
	int playing_go_music = 0;

	game_status = GS_OK;
	cycle_count = 0;
	while(game_status == GS_OK) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		//  do logic
		while(cycle_count > 0) {
			logic_count ++;
			update_sdl_keyboard();

			// check if user wants to enter edit mode
			if (!playing_original_game && !editing) {
#if ENABLE_EDITOR

				if (key[KEY_F1]) {		// toggle edit on/off
					editing = 1;
					stop_music();
					set_edit_mode(EDIT_MODE_DRAW);
					log2file("Entering EDIT MODE");
				}
#endif
			}
			
			// check player moves
			if (!editing) {
				update_player();
	
				// dead?
				if (player.actor->status == AC_DEAD && !playing_go_music) {
					start_music((player.lives ? MOD_PLAYER_DIES : MOD_GAME_OVER));
					playing_go_music = 1;
				}
				
				if (player.actor->y > 160) {
					if (player.actor->status != AC_DEAD) {
						kill_player(&player);
						player.actor->y = 150;
						player.actor->dy = -20;
						play_sound_id(S_DIE);
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
#if ENABLE_EDITOR
			else { 			// edit stuff
				int mx = GetMouseX() / (GetScreenW() / 160);
				int my = GetMouseY() / (GetScreenH() / 120);

				player.actor->x = map->start_x * 16;
				player.actor->y = map->start_y * 16 + 16;
				if (player.actor->x < 0) {
					player.actor->x = -map->start_x * 16;
					player.actor->direction = 0;
				}
				else {
					player.actor->direction = 1;
				}
				update_edit_mode(map, swap_screen, mx, my, MouseBtn());
			}
#endif
			// COMMON STUFF
			if (key[KEY_ESC]) {
				do_pause_menu(swap_screen);
				cycle_count = 0;
			}
			if (key[KEY_F12]) {
				take_screenshot(swap_screen);
				while(key[KEY_F12])
					update_sdl_keyboard();
				cycle_count = 0;
			}
			
			cycle_count --;
		}

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

	blit(bitmaps[I_ALEX_BG], bmp, 0, 0, 0, 0, 160, 112);
	rectfill(bmp, 0, 112, 160, 120, 2);

	w = bitmaps[I_ALEX_LOGO]->w;
	h = bitmaps[I_ALEX_LOGO]->h;
	draw_sprite(bmp, bitmaps[I_ALEX_LOGO], 80 - w/2, 30 - h/2);

	draw_scroller(&hscroll, bmp, 0, 110);

	y = 60;
	x = 50;
	textout_ex(bmp, start_string, x+1, y+1, 1, -1);
	textout_ex(bmp, start_string, x, y, 4, -1);

	y += step;
	textout_ex(bmp, "HIGH SCORES", x+1, y+1, 1, -1);
	textout_ex(bmp, "HIGH SCORES", x, y, 4, -1);

	y += step;
#if ENABLE_EDITOR
	textout_ex(bmp, "EDITOR", x+1, y+1, 1, -1);
	textout_ex(bmp, "EDITOR", x, y, 4, -1);
#else
	textout_ex(bmp, "EDITOR", x+1, y+1, 1, -1);
	textout_ex(bmp, "EDITOR", x, y, 1, -1);
#endif

	y += step;
	textout_ex(bmp, "QUIT", x+1, y+1, 1, -1);
	textout_ex(bmp, "QUIT", x, y, 4, -1);

	// FIXME: wobble
	//draw_sprite(bmp, bitmaps[I_POINTER], x - 25 + fixtoi(3 * fixcos(itofix(tick << 2))), 44 + menu_choice * step);
	draw_sprite(bmp, bitmaps[I_POINTER], x - 25 + (int)(3 * cosf((tick << 2)*(float)M_PI/180)), 44 + menu_choice * step);
}

// gets a string from the user
// used for getting highscore names
int get_string(BITMAP *bmp, char *string, int max_size, int pos_x, int pos_y, int colour, Tcontrol *pad) {
	int i = 0, c;
	BITMAP *block = create_bitmap(text_length("w")*max_size + 2, text_height() + 2);
	char letters[] = "_abcdefghijklmnopqrstuvwxyz {}";
	int current_letter = 0;
	int max_letter = strlen(letters) - 1;
	int print_delay = 0;
	
	if (block == NULL)
		return 1;

	blit(bmp, block, pos_x - 1, pos_y - 1, 0, 0, block->w, block->h);

	while(1) {
		cycle_count = 0;
		string[i] = letters[current_letter];
		string[i + 1] = '\0';
		blit(block, bmp, 0, 0, pos_x - 1, pos_y - 1, block->w, block->h);
		textout_ex(bmp, string, pos_x, pos_y, colour, -1);
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

		while(!cycle_count)
			rest(1);
	}
}

// lets the player enter a name for highscore use (or what ever)
void get_player_name(char *name) {
	blit(bitmaps[I_INTRO_BG], swap_screen, 0, 0, 0, 0, 160, 120);
	transform_bitmap(swap_screen, -1);
	textout_outline_center(swap_screen, "Congratulations,", 80, 8);
	textout_outline_center(swap_screen, "You've got", 80, 19);
	textout_outline_center(swap_screen, "a high score!", 80, 30);
	textout_outline_center(swap_screen, "Enter your name:", 80, 55);
	blit_to_screen(swap_screen);
	fade_in_pal(swap_screen, 100);
	get_string(swap_screen, name, 10, 50, 80, 4, &ctrl);
}

// title and menu
int do_main_menu() {
	int status = GS_OK;
	int count = 0;
	int tick = 0;

	log2file("\nRunning main menu:");
	set_window_title("MAIN MENU");

	draw_title(swap_screen, tick);
	blit_to_screen(swap_screen);
	fade_in_pal(swap_screen, 100);

	cycle_count = 0;
	while(status == GS_OK) {
		// let other processes play
		while(cycle_count == 0)
			rest(1);

		//  do logic
		while(cycle_count > 0) {
			logic_count ++;
			tick ++;
			if (tick > 360) tick = 0;

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
						play_sound_id(S_MENU);
					}
					if (menu_choice == 2) {
						log2file(" scores selected");
						status = GS_SCORES;
						play_sound_id(S_MENU);
					}
					if (menu_choice == 3) {
						log2file(" edit selected");
#if ENABLE_EDITOR
						status = GS_EDIT;
						play_sound_id(S_MENU);
#else
						play_sound_id(S_STOMP);
#endif
					}

					if (menu_choice == 4) {
						log2file(" quit selected");
						status = GS_QUIT_MENU;
						play_sound_id(S_MENU);
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
					play_sound_id(S_MENU);
					count = 10;
				}

				// movements in the menu
				if (key[KEY_UP] || is_up(&ctrl)) { 
					menu_choice --; 
					if (menu_choice < 1) menu_choice = 4;
					play_sound_id(S_MENU); 
					count = 10; 
				}
				if (key[KEY_DOWN] || is_down(&ctrl)) {
					menu_choice ++; 
					if (menu_choice > 4) menu_choice = 1;
					play_sound_id(S_MENU); 
					count = 10; 
				}
			}

			// releasing all these keys, and you cen press them again
			if (!is_any(&ctrl) && !key[KEY_UP] && !key[KEY_ESC] && !key[KEY_DOWN] && !key[KEY_SPACE] && !key[KEY_ENTER]) count = 0;

			// shortcuts to gfx modes
			if (key[KEY_1]) {
				while(key[KEY_1])
					update_sdl_keyboard();
				IncreaseZoom(&options);
			}
			if (key[KEY_2]) {
				while(key[KEY_2])
					update_sdl_keyboard();
				DecreaseZoom(&options);
			}
			if (key[KEY_3]) {
				while(key[KEY_3])
					update_sdl_keyboard();
				// unused
			}
			if (key[KEY_4]) {
				while(key[KEY_4])
					update_sdl_keyboard();
				ToggleFullScreen(&options);
			}

			cycle_count --;
		}

		// draw 
		frame_count ++;
		draw_title(swap_screen, tick);
		blit_to_screen(swap_screen);
	}

#if ENABLE_EDITOR
	// user selected EDIT
	if (status == GS_EDIT) {
		stop_music();
		fix_gui_colors();
		log2file("\nEntering editor:");
		fade_out_pal(swap_screen, 100);

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
			
			editing = true;
			set_edit_mode(EDIT_MODE_DRAW);
			set_edit_path_and_file("new.map");

			draw_frame(swap_screen, 1);
			blit_to_screen(swap_screen);
			fade_in_pal(swap_screen, 100);
			status = play();
			deinit_map();
		}
		else {
			log2file(" *** failed");
		}
		start_music(MOD_MENU_SONG);
	}
	else
#endif
	if (status == GS_PLAY) {  //// user selected PLAY
		int level = 0;

		log2file("\nStarting new game:");
		fade_out_pal(swap_screen, 100);

		game_status = GS_OK;
		editing = false;
		new_game(1);

		// select starting level
		if (options.max_levels && playing_original_game) {
			level = select_starting_level() - 1;
			fade_out_pal(swap_screen, 100);

			if (level == -2) {  // esc
				log2file(" cancelled");
				game_status = status = GS_QUIT_GAME;
			}

#ifdef ENABLE_SHOOTER
			if (level == -101) {  // shooter
				log2file(" shooter selected");
				stop_music();
				start_shooter(&ctrl, got_sound);
				game_status = status = GS_QUIT_GAME;
				start_music(MOD_MENU_SONG);
			}
#endif
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
#ifdef ENABLE_EDITOR
				set_edit_path_and_file(level_files[level]);
#endif
				new_level(level_files[level], -1, 1);
			}

			if (map->boss_level) start_music(MOD_BOSS_SONG);
			else start_music(MOD_LEVEL_SONG);

			// actual game starts here
			show_lets_go();
			status = play();
			// done playing level

			deinit_map();

			// act on different outcomes
			if (status == GS_GAME_DIED) {
				log2file(" player died");
				if (player.lives == 0) {
					game_status = GS_GAMEOVER;
				}
				else {
					game_status = GS_OK;
					fade_out_pal(swap_screen, 200);
				}
			}
			else if (status == GS_QUIT_GAME) {
				log2file(" player quit");
				stop_music();
				fade_out_pal(swap_screen, 100);
				start_music(MOD_MENU_SONG);
			}
			else {
#ifdef __unix__
				char filename[512];
				char *homedir = get_homedir();
#endif
				log2file(" level complete");
				stop_music();
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
				fade_out_pal(swap_screen, 100);

				// save options
				log2file(" saving options");
#ifdef __unix__
				snprintf(filename, sizeof(filename),
					"%s/.alex4/config",
					homedir? homedir:".");
#else
				strcpy(filename, "config.ini");
#endif
				save_options(&options, filename);
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
					set_window_title("OUTRO");
					start_music(MOD_OUTRO_SONG);
					run_script(SCR_OUTRO);
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
				fade_out_pal(swap_screen, 100);

				log2file(" player qualified for highscore (%d, %d)", post.level, post.score);

				// get player name
				get_player_name(post.name);
				log2file(" score logged as <%s>", post.name);

				enter_hisc_table(hisc_table, post);
				sort_hisc_table(hisc_table);
			}

			start_music(MOD_MENU_SONG);
			status = GS_SCORES;									
		}
		
	}

	// show highscores? 
	// can be entered from both menu and end of game
	if (status == GS_SCORES) {
		fade_out_pal(swap_screen, 100);
		log2file(" showing scores");
		show_scores(0, hisc_table);
#ifdef ENABLE_SHOOTER
		if (options.one_hundred) {
			show_scores(1, hisc_table_space);
		}
#endif
	}
	
	// user quit
	if (status == GS_QUIT_MENU) {
		stop_music();
#ifndef NO_CREDITS
		fade_out_pal(swap_screen, 100);
		// show bye bye screen
		blit(bitmaps[I_INTRO_BG], swap_screen, 0, 0, 0, 0, 160, 120);
		transform_bitmap(swap_screen, -1);
		textout_outline_center(swap_screen, "Thanks for playing!", 80, 12);
		textout_outline_center(swap_screen, "Design, Code, GFX:", 80, 40);
		textout_outline_center(swap_screen, "Johan Peitz", 80, 52);
		textout_outline_center(swap_screen, "MUSIC, SFX:", 80, 66);
		textout_outline_center(swap_screen, "Anders Svensson", 80, 80);
		textout_outline_center(swap_screen, "SDL2 port, maintenance", 80, 94);
		textout_outline_center(swap_screen, "carstene1ns", 80, 106);
		blit_to_screen(swap_screen);
		fade_in_pal(swap_screen, 100);
		cycle_count = 0;
		while(!key[KEY_ESC] && cycle_count < 200) {
			update_sdl_keyboard();
			rest(50);
		}
#endif
		fade_out_pal(swap_screen, 100);
		clear_screen();
	}

	return status;
}

// main
int main(int argc, char **argv) {
	int i;
	char full_path[1024];
#ifndef __unix__
	char *working_directory;
#else
	char *homedir = get_homedir();
#endif

	// init
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

#ifdef __unix__
	// start logfile
	snprintf(full_path, sizeof(full_path), "%s/.alex4",
		homedir? homedir:".");
	check_and_create_dir(full_path);
	strcat(full_path, "/log.txt");
	log_fp = fopen(full_path, "wt");
#else
	// get working directory
	get_executable_name(full_path, 1024);
	working_directory = strdup(full_path)
	working_directory = dirname(working_directory);
	chdir(working_directory);

	// start logfile
	log_fp = fopen("log.txt", "wt");
#endif
	if (log_fp) {
		fprintf(log_fp, "Alex 4 (%s) - log file\n-------------------\n", GAME_VERSION_STR);
	}

	// log program arguments
	log2file("Game started with the following commands:");
	for(i = 0; i < argc; i ++) {
		log2file("   %s", argv[i]);
	}
#ifndef __unix__
	log2file("Working directory is:\n   %s", working_directory);
	free(working_directory);
#endif

	// test wether to play real game
	// or custom levels
	if (argc == 1) playing_original_game = true;
	else playing_original_game = false;

	// init game
	if (init_game((playing_original_game ? "maps from datafile please" : argv[1]))) {
#ifndef NO_INTRO
		if (playing_original_game) {
			set_window_title("INTRO");
			start_music(MOD_INTRO_SONG);
			run_script(SCR_INTRO);
			stop_music();
		}
#endif
		start_music(MOD_MENU_SONG);
		while(do_main_menu() != GS_QUIT_MENU);
	}
	else {
		log2file("*** init failed!");
		msg_box("ALEX4:\nFailed to start game.");
	}

	// tidy up
	uninit_game();
	log2file("\nDone...\n");
	if (log_fp)
		fclose(log_fp);

	return 0;
}
