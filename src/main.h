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
 
 
 
#ifndef _MAIN_H_
#define _MAIN_H_

#include "allegro.h"
#include "aldumb.h"
#include "map.h"
#include "hisc.h"
#include "actor.h"
#include "control.h"

// version string
#define		GAME_VERSION_STR	"v1.1"

// sample shortcuts
#define		SMPL_STARTUP	0
#define		SMPL_POINT		1
#define		SMPL_JUMP		2
#define		SMPL_MASH		3
#define		SMPL_EAT		4
#define		SMPL_SPIT		5
#define		SMPL_A_DIE		6
#define		SMPL_HIT		7
#define		SMPL_CRUSH		8
#define		SMPL_E_DIE		10
#define		SMPL_HEART		11
#define		SMPL_HURT		12
#define		SMPL_XTRALIFE	13
#define		SMPL_CHERRY		14
#define		SMPL_MENU		15
#define		SMPL_SHOOT		16
#define		SMPL_SPIN		17
#define		SMPL_STAR		18
#define		SMPL_CRUSH_LAND	19
#define		SMPL_PAUSE		20
#define		SMPL_ENGINE		21
#define		SMPL_CHOPPER	22
#define		SMPL_SHIP		23
#define		SMPL_TALK		24
#define		SMPL_BEAM		25


// functions
char *get_init_string();
void textout_outline_center(BITMAP *bmp, const char *txt, int cx, int y);
void log2file(char *format, ...);
int do_pause_menu(BITMAP *bg);
void take_screenshot(BITMAP *bmp);
void set_map(Tmap *m);
void msg_box(char *str);
void new_game(int reset_player_data);
void new_level(char *fname, int level_id, int draw);
Tactor *get_alex();
void draw_frame(BITMAP *bmp, int draw_status_bar);
void blit_to_screen(BITMAP *bmp);
void fade_in_pal(int delay);
void fade_out_pal(int delay);
void fade_in_pal_black(int delay, AL_DUH_PLAYER *duh_player);
void fade_out_pal_black(int delay, AL_DUH_PLAYER *duh_player);
void play_sound_id(int id);
void play_sound_id_ex(int id, int vol, int freq, int loop);
void adjust_sound_id_ex(int id, int x);
void stop_sound_id(int id);
int get_string(BITMAP *bmp, char *string, int max_size, FONT *f, int pos_x, int pos_y, int colour, Tcontrol *pad);
BITMAP *get_gameover_sign();
BITMAP *get_letsgo_sign();
Thisc *get_space_hisc();
void show_scores(int space, Thisc *table);
void poll_music();


// a little bounding box quickie
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
                                                        ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))


#endif