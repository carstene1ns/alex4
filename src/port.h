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

#ifndef PORT_H
#define PORT_H

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include "options.h"

#include "fixed_math.h"
#include "sdl/port.h"

#define stricmp strcasecmp

#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#define CLAMP(x,y,z) MAX((x), MIN((y), (z)))

#define SCREEN_W 160
#define SCREEN_H 120
#define TRUE true
#define FALSE false

// gfx
BITMAP *create_bitmap(int w, int h);
void destroy_bitmap(BITMAP *bmp);
BITMAP *load_bmp_from_mem(void *data, size_t size);
void draw_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y); 
void draw_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);
void draw_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);
void draw_sprite_vh_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);
void rectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
void line(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
#define masked_blit blit
void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
void putpixel(BITMAP *bmp, int x, int y, int color);
void set_clip_rect(BITMAP *bmp, int x1, int y1, int x2, int y2);
void reset_clip_rect(BITMAP *bmp);
void rotate_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int angle);
void rotate_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int angle);
void stretch_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int w, int h);
void clear_bitmap(BITMAP* bmp);
void clear_screen();
void clear_to_color(BITMAP* bmp, int color);
void draw_character_ex(BITMAP *bmp, BITMAP *sprite, int x, int y, int color);

void draw_tile(BITMAP *bmp, int tile_id, int x, int y); 

extern int gui_bg_color;
extern int gui_fg_color;

void switch_palette(bool colorize);

// text
void prepare_font(int id);
void textprintf_ex_helper(int pos, BITMAP *bmp, int x, int y, int color, int bg, const char *fmt, ...);
void textout_ex_helper(int pos, BITMAP *bmp, const char *s, int x, int y, int color, int bg);
#define textout_ex(bmp, s, x, y, color, bg) \
	textout_ex_helper(-1, bmp, s, x, y, color, bg)
#define textout_centre_ex(bmp, s, x, y, color, bg) \
	textout_ex_helper(0, bmp, s, x, y, color, bg)
#define textprintf_ex(bmp, x, y, color, bg, fmt, ...) \
	textprintf_ex_helper(-1, bmp, x, y, color, bg, fmt, __VA_ARGS__)
#define textprintf_centre_ex(bmp, x, y, color, bg, fmt, ...) \
	textprintf_ex_helper(0, bmp, x, y, color, bg, fmt, __VA_ARGS__)
#define textprintf_right_ex(bmp, x, y, color, bg, fmt, ...) \
	textprintf_ex_helper(1, bmp, x, y, color, bg, fmt, __VA_ARGS__)
int text_length(const char *txt);
int text_height();

// file
int exists(const char *f);

// key
bool keypressed();
int readkey();
void wait_key(int seconds);

int GetMouseX();
int GetMouseY();
bool MouseBtn();
int GetScreenW();
int GetScreenH();

void rest(int ms);

void fade_in_pal(BITMAP *bmp, int delay);
void fade_out_pal(BITMAP *bmp, int delay);
void fade_in_pal_black(BITMAP *bmp, int delay);
void fade_out_pal_black(BITMAP *bmp, int delay);

void blit_to_screen(BITMAP *bmp);
void fix_gui_colors();

void set_window_title(const char *title);

void IncreaseZoom(Toptions *o);
void DecreaseZoom(Toptions *o);
void ToggleFullScreen(Toptions *o);

void transform_bitmap(BITMAP *bmp, int steps);

void take_screenshot_platform(BITMAP *bmp, const char *filename);
void msg_box(const char *str);

#endif
