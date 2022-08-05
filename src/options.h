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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

// number of levels to count cherries/stars on
#define MAX_LEVELS		64

// the options struct
typedef struct {
	// graphics
	bool fullscreen;
	int width;
	int height;
	bool use_vsync;
	// sound
	bool sound_enable;
	int buffer_size;
	int sound_freq;
	int sample_volume;
	int music_volume;
	// game
	int max_levels;
	int cherries[MAX_LEVELS];
	int stars[MAX_LEVELS];
	bool one_hundred;
} Toptions;

// functions
void save_options(Toptions *o, const char *file);
bool load_options(Toptions *o, const char *file);
void reset_options(Toptions *o);

#endif
