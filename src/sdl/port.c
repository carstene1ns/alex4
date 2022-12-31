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

#include <SDL.h>
#include <miniz.h>
#include <stb_image.h>
#include "../port.h"
#include "data.h"
#include "main.h"
#include "sound.h"

SDL_Window* window;
SDL_Renderer* renderer;

// use sdl

// original colors
SDL_Color _palette[] = {
	{0x3C, 0x8E, 0xAA, 0xFF}, // (white)
	{0x0C, 0x45, 0x00, 0xFF}, // (red)
	{0x51, 0x7D, 0x45, 0xFF}, // (lime)
	{0x9A, 0xB6, 0x8A, 0xFF}, // (yellow)
	{0xDF, 0xEF, 0xD3, 0xFF}, // (blue)
	{0xFF, 0x00, 0x00, 0x99}  // red
};

// font info
struct {
	SDL_Texture *texture;
    int char_width;
    int char_height;
    SDL_Rect charRects[16*16];
} font;

// format of tileset.bmp
struct {
	int count;
	int rows, cols;
	int width, height;
} tileset_info = {
	.count = 96,
	.rows = 10, .cols = 10,
	.width = 16, .height = 16
};

// gfx
BITMAP *create_bitmap(int w, int h) {
	BITMAP *b = (BITMAP *)malloc(sizeof (BITMAP));
	b->h = h;
	b->w = w;
	b->tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);

	if (!b->tex) printf("texture broken: %s\n", SDL_GetError());

	return b; 
}
void destroy_bitmap(BITMAP *bmp) {
	SDL_DestroyTexture(bmp->tex);
	free(bmp);
}

BITMAP *load_bmp_from_mem(void *data, size_t size) {
	BITMAP *b = (BITMAP *)malloc(sizeof (BITMAP));
	if(!b) return NULL;

	unsigned char* image = NULL;
	int width, height, channels;

	image = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
	if(!image) {
		printf("png error\n");
		free(b);
		return NULL;
	}
	//if(channels != 4) printf("png not 32 bit, but %d\n", channels*8);

	b->tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
	if (!b->tex) {
		printf("texture broken: %s\n", SDL_GetError());
		free(b);
		stbi_image_free(image);
		return NULL;
	}

	if(SDL_UpdateTexture(b->tex, NULL, image, width * 4)) {
		printf("texture broken: %s\n", SDL_GetError());
		SDL_DestroyTexture(b->tex);
		free(b);
		stbi_image_free(image);
	}

	stbi_image_free(image);

	//SDL_SetTextureBlendMode(b->tex, SDL_BLENDMODE_ADD);

	b->h = height;
	b->w = width;

	return b; 	
}

void draw_tile(BITMAP *bmp, int tile_id, int x, int y) {
	if (tile_id < 0 || tile_id >= tileset_info.count) {
		printf("invalid tile id (%d)!\n");
		return;
	}

	int row = tile_id / tileset_info.cols;
	int col = tile_id - row * tileset_info.cols;

	int tile_x = col * tileset_info.width;
	int tile_y = row * tileset_info.height;

	blit(bitmaps[I_TILESET], bmp, tile_x, tile_y, x, y, tileset_info.width, tileset_info.height);
}

void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) {
	SDL_Rect s;
	s.x = source_x;
	s.w = width;
	s.y = source_y;
	s.h = height;

	SDL_Rect d;
	d.x = dest_x;
	d.y = dest_y;
	d.w = width;
	d.h = height;

	SDL_SetRenderTarget(renderer, dest->tex);
	SDL_RenderCopy(renderer, source->tex, &s, &d);
}
void stretch_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int w, int h) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = w;
	d.h = h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopy(renderer, sprite->tex, NULL, &d);
}
void draw_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopy(renderer, sprite->tex, NULL, &d);
}
void draw_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopyEx(renderer, sprite->tex, NULL, &d, 0, NULL, SDL_FLIP_VERTICAL);
}
void draw_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopyEx(renderer, sprite->tex, NULL, &d, 0, NULL, SDL_FLIP_HORIZONTAL);
}
void draw_sprite_vh_flip(BITMAP *bmp, BITMAP *sprite, int x, int y) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopyEx(renderer, sprite->tex, NULL, &d, 0, NULL, SDL_FLIP_HORIZONTAL|SDL_FLIP_VERTICAL);
}
void rotate_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int angle) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopyEx(renderer, sprite->tex, NULL, &d, angle, NULL, SDL_FLIP_NONE);
}
void rotate_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int angle) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopyEx(renderer, sprite->tex, NULL, &d, angle, NULL, SDL_FLIP_VERTICAL);
}

void rectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	SDL_Rect r;
	r.x = x1;
	r.w = x2-x1;
	r.y = y1;
	r.h = y2-y1;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, _palette[color].r, _palette[color].g, _palette[color].b, 0xFF);
	SDL_RenderFillRect(renderer, &r);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 0x00);
}

void line(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, _palette[color].r, _palette[color].g, _palette[color].b, 0xFF);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 0x00);
}

void rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
	SDL_Rect r;
	r.x = x1;
	r.w = x2-x1;
	r.y = y1;
	r.h = y2-y1;

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, _palette[color].r, _palette[color].g, _palette[color].b, 0xFF);
	SDL_RenderDrawRect(renderer, &r);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 0x00);
}
void putpixel(BITMAP *bmp, int x, int y, int color) {
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, _palette[color].r, _palette[color].g, _palette[color].b, 0xFF);
	SDL_RenderDrawPoint(renderer, x, y);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 0x00);
}

void set_clip_rect(BITMAP *bmp, int x1, int y1, int x2, int y2) {
	SDL_Rect r;
	r.x = x1;
	r.w = x2-x1;
	r.y = y1;
	r.h = y2-y1;
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderSetClipRect(renderer, &r);
}

void reset_clip_rect(BITMAP *bmp) {
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderSetClipRect(renderer, NULL);
}

void clear_bitmap(BITMAP* bmp) {
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
}

void clear_screen() {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
}

void clear_to_color(BITMAP* bmp, int color) {
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, _palette[color].r, _palette[color].g, _palette[color].b, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x0, 0x00, 0x00);
}

void draw_character_ex(BITMAP *bmp, BITMAP *sprite, int x, int y, int color) {
	SDL_Rect d;
	d.x = x;
	d.y = y;
	d.w = sprite->w;
	d.h = sprite->h;

	SDL_SetTextureColorMod(sprite->tex, _palette[color].r, _palette[color].g, _palette[color].b);
	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_RenderCopy(renderer, sprite->tex, NULL, &d);
	SDL_SetTextureColorMod(sprite->tex, 0xFF, 0xFF, 0xFF);
}

// text
void prepare_font(int id) {
    SDL_RWops* rw = SDL_RWFromConstMem(fonts[id].dat, fonts[id].size);
    SDL_Surface* surface = SDL_LoadBMP_RW(rw, 1);
    if (!surface) return;

    SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(surface);
    if (!converted_surface) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, converted_surface->w, converted_surface->h);
    if (!texture) {
        SDL_FreeSurface(converted_surface);
        return;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Copy font image into texture pixels
    void* texture_pixels = NULL;
    int texture_pitch = 0;
    SDL_LockTexture(texture, &converted_surface->clip_rect, &texture_pixels, &texture_pitch);

    memcpy(texture_pixels, converted_surface->pixels, converted_surface->pitch * converted_surface->h);

    // Make background transparent
    uint32_t foreground_color = SDL_MapRGB(converted_surface->format, 0xFF, 0xFF, 0xFF);
    uint32_t background_color = SDL_MapRGB(converted_surface->format, 0x00, 0x00, 0x00);
    uint32_t transparent_color = SDL_MapRGBA(converted_surface->format, 0xFF, 0xFF, 0xFF, 0x00);

    uint32_t* pixels = (uint32_t*)texture_pixels;
    int32_t pixel_count = (texture_pitch / 4) * converted_surface->h;
    SDL_FreeSurface(converted_surface);

    for (int i = 0; i < pixel_count; ++i) {
        if (pixels[i] == background_color) {
            pixels[i] = transparent_color;
        } else {
            pixels[i] = foreground_color;
        }
    }

    SDL_UnlockTexture(texture);
    font.texture = texture;

    // Build array of SDL_Rect
    font.char_width = 7;
    font.char_height = 8;
    int padding_width = 9;
    int padding_height = 8;

    int char_code = 0;
	// empty first 31 (control chars)
    for (int c = 0; c < 32; ++c) {
        font.charRects[char_code].x = 0;
        font.charRects[char_code].y = 0;
        font.charRects[char_code].w = 0;
        font.charRects[char_code].h = 0;
        ++char_code;
    }

    for (int r = 0; r < 14; ++r) {
        for (int c = 0; c < 16; ++c) {
            font.charRects[char_code].x = 1 + font.char_width * c + (padding_width * c);
            font.charRects[char_code].y = 1 + font.char_height * r + (padding_height * r);
            font.charRects[char_code].w = font.char_width;
            font.charRects[char_code].h = font.char_height;
            ++char_code;
        }
    }

    // width fixup
    font.charRects['W'].w += 2;
    font.charRects['w'].w += 2;
    font.charRects['M'].w += 2;
    font.charRects['m'].w += 2;
}

void textout_ex_helper(int pos, BITMAP *bmp, const char *s, int x, int y, int color, int bg) {
	if (!font.texture) {
        return;
    }

    if(bg != -1) printf("FIXME: ignoring bg\n");

    // left
    int current_x = x;
    int current_y = y;
    // center
    if (pos == 0) {
    	current_x = x - text_length(s) / 2;
    } else if (pos == 1) {
    	// right
    	current_x = x - text_length(s);
    }

	SDL_SetTextureColorMod(font.texture, _palette[color].r, _palette[color].g, _palette[color].b);
	SDL_SetRenderTarget(renderer, bmp->tex);

    size_t length = strlen(s);
    for (size_t i = 0; i < length; ++i) {
    	int32_t code = s[i];
        switch (code) {
        case '\n':
        	printf("enter?\n");
            current_x = x;
            current_y += font.char_height + 1;
            break;
        default: {
            SDL_Rect r = {current_x, current_y, font.charRects[code].w, font.charRects[code].h};
            SDL_RenderCopy(renderer, font.texture, &font.charRects[code], &r);
            current_x += font.charRects[code].w;
            break;
        	}
    	}
    }

	SDL_SetTextureColorMod(font.texture, 0xFF, 0xFF, 0xFF);
}

void textprintf_ex_helper(int pos, BITMAP *bmp, int x, int y, int color, int bg, const char *fmt, ...) {
	char buf[100];
	va_list arglist;

	va_start(arglist, fmt);
	vsnprintf(buf, 100, fmt, arglist);
	va_end(arglist);

	textout_ex_helper(pos, bmp, buf, x, y, color, bg);
}

int text_length(const char *txt) {
	return strlen(txt) * font.char_width;
}
int text_height() {
	return font.char_height;
}

// keyboard

const Uint8 *key;

void update_platform_controls() {
	SDL_PumpEvents();
	key = SDL_GetKeyboardState(NULL);
}

//SDL_Scancode sdl_key = 0;

bool keypressed() {


	return false;
}

int readkey() {
	return 0;
}

// file
int exists(const char *f) {
	return (access(f, F_OK) == 0);
}

int GetMouseX() {
	int x;
	SDL_GetMouseState(&x, NULL);
	return x;
}
int GetMouseY() {
	int y;
	SDL_GetMouseState(NULL, &y);
	return y;
}
bool MouseBtn() {
	return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK);
}

// waits for user to strike a key, or x seconds
void wait_key(int seconds) {
	SDL_Event event;

	SDL_WaitEventTimeout(&event, seconds/1000);
}

void fade_helper(BITMAP *bmp, bool fadein, Uint8 col, int delay) {
	int steps = delay / 10;
	int alpha_step = 0xFF / steps;

	SDL_SetTextureColorMod(bmp->tex, col, col, col);
	SDL_SetTextureBlendMode(bmp->tex, SDL_BLENDMODE_BLEND);

	for (int i = 0; i <= steps; i++) {
		if (i != 0) SDL_Delay(10);

		Uint8 alpha = fadein ? alpha_step * i : alpha_step * (steps - i);
		SDL_SetTextureAlphaMod(bmp->tex, alpha);

		blit_to_screen(bmp);
	}

	SDL_SetTextureAlphaMod(bmp->tex, 0xFF);
	SDL_SetTextureColorMod(bmp->tex, 0xFF, 0xFF, 0xFF);
	SDL_SetTextureBlendMode(bmp->tex, SDL_BLENDMODE_NONE);
}

// fades in from white to 4 color palette
void fade_in_pal(BITMAP *bmp, int delay) {
	fade_helper(bmp, true, 0xFF, delay);
}
// fades 4 color palette to white
void fade_out_pal(BITMAP *bmp, int delay) {
	fade_helper(bmp, false, 0xFF, delay);
}
// fade in from black to 4 colors pal
void fade_in_pal_black(BITMAP *bmp, int delay) {
	fade_helper(bmp, true, 0x00, delay);
}
// fades 4 color palette to black
void fade_out_pal_black(BITMAP *bmp, int delay) {
	fade_helper(bmp, false, 0x00, delay);
}

// blits anything to screen
void blit_to_screen(BITMAP *bmp) {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, bmp->tex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

#if 0
	// fix palette
	for(i = 0; i < 256; i ++) {
		org_pal[i].r = ((RGB *)data[0].dat)[i].r;
		org_pal[i].g = ((RGB *)data[0].dat)[i].g;
		org_pal[i].b = ((RGB *)data[0].dat)[i].b;
	}

	// fix some palette entries
	((RGB *)data[0].dat)[0].r = 0;
	((RGB *)data[0].dat)[0].g = 0;
	((RGB *)data[0].dat)[0].b = 0;
	fix_gui_colors();
	set_palette(data[0].dat);
#endif

// sets up the gui colors
void fix_gui_colors() {
#if 0
	((RGB *)data[0].dat)[255].r = 0;
	((RGB *)data[0].dat)[255].g = 0;
	((RGB *)data[0].dat)[255].b = 0;
	((RGB *)data[0].dat)[254].r = 63;
	((RGB *)data[0].dat)[254].g = 63;
	((RGB *)data[0].dat)[254].b = 63;
	gui_fg_color = 255;
	gui_bg_color = 254;
#endif
}

// lighten or darken a 4 color bitmap
void transform_bitmap(BITMAP *bmp, int steps) {
	if (steps != -1) {
		printf("unknown transform!"); return;
	}

	SDL_SetRenderTarget(renderer, bmp->tex);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x80);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void init_platform() {
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
}

void uninit_platform() {
	SDL_Quit();
}

bool fullscreen = false;
int zoom = 1;

void make_window(Toptions *o) {
	int window_flags = 0;
	if (o->fullscreen) {
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		fullscreen = true;
	}
	window = SDL_CreateWindow(GAME_TITLE_STR,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		o->width, o->height, window_flags);
	int renderer_flags = SDL_RENDERER_TARGETTEXTURE;
	if (o->use_vsync)
		renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

	zoom = MIN(o->width / SCREEN_W, o->height / SCREEN_H);

	// important: blocky sharp images
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

void set_window_title(const char *title) {
	if (strlen(title) > 0) {
		char t[100] = GAME_TITLE_STR;
		strcat(t, " - ");
		strcat(t, title);
		SDL_SetWindowTitle(window, t);
	} else
		SDL_SetWindowTitle(window, GAME_TITLE_STR);
}

void rest(int ms) {
	SDL_Delay(ms);
}

int GetScreenW() {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	return w;
}
int GetScreenH() {
	int h;
	SDL_GetWindowSize(window, NULL, &h);
	return h;
}

void SetZoom(int z, Toptions *o) {
	if(fullscreen) ToggleFullScreen(o);

	zoom = CLAMP(1, z, 10);
	o->width = SCREEN_W * zoom;
	o->height = SCREEN_H * zoom;

	SDL_SetWindowSize(window, o->width, o->height);
}

void IncreaseZoom(Toptions *o) {
	SetZoom(zoom + 1, o);
}
void DecreaseZoom(Toptions *o) {
	SetZoom(zoom - 1, o);
}

void ToggleFullScreen(Toptions *o) {
	Uint32 flags = SDL_GetWindowFlags(window);

	if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
		SDL_SetWindowFullscreen(window, 0);
		fullscreen = false;
		SetZoom(zoom, o);
	} else {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		fullscreen = true;
	}

	o->fullscreen = fullscreen;
}

// shows a little message
void msg_box(const char *str) {
	pause_music(true);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Alex 4: Message", str, NULL);
	pause_music(false);
}

void take_screenshot_platform(BITMAP *bmp, const char *filename) {
	SDL_Surface *tmp = SDL_CreateRGBSurface(0, bmp->w, bmp->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	if(!tmp) return;

	SDL_Texture *old = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, bmp->tex);

	if(SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, tmp->pixels, tmp->pitch)) {
		SDL_SetRenderTarget(renderer, old);
		SDL_FreeSurface(tmp);
		return;
	}

	size_t len;
	void *png = tdefl_write_image_to_png_file_in_memory(tmp->pixels, bmp->w, bmp->h, 4, &len);
	SDL_SetRenderTarget(renderer, old);
	SDL_FreeSurface(tmp);
	if (!png) return;

	FILE *fp = fopen(filename, "wb");
	fwrite(png, len, 1, fp);
	fclose(fp);

	mz_free(png);
}
