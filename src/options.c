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
#include "options.h"
#include "iniparser.h"
#include "port.h"

// helpers

#define MAKE_INIBOOL(x) x ? "true" : "false"

static const char* ini_video_preamble =
	"##############################################################\n#\n"
	"# Configuration file for Alex 4\n#\n"
	"##############################################################\n#\n"
	"# If you lose control of the contents of this file and want to\n"
	"# restore to the defaults, you can just delete it.\n#\n"
	"##############################################################\n\n"
	"# These options control the startup resolution\n"
	"# 640x480 non-fullscreen is a good default.";
static const char* ini_sound_preamble =
	"# These options control the sound and music\n"
	"# volume is 0-100. 44100Hz at 1024 samples is a good default.";
static const char* ini_game_preamble =
	"# These options control the game behaviour\n"
	"# Better not mess with them.";

// saves the data structure to disk
void save_options(Toptions *o, const char *file) {
	#define N 12
	char val[N];
	dictionary *ini = dictionary_new(140); // 11 options, 64*2 level stats, ...
	if(!ini) return;

	FILE *fp = fopen(file, "w");
	if (!fp) return;

	iniparser_set(ini, "video", NULL);
	iniparser_set(ini, "video:fullscreen", MAKE_INIBOOL(o->fullscreen));
	snprintf(val, N, "%d", o->width);
	iniparser_set(ini, "video:width", val);
	snprintf(val, N, "%d", o->height);
	iniparser_set(ini, "video:height", val);
	iniparser_set(ini, "video:vsync", MAKE_INIBOOL(o->use_vsync));
	iniparser_set(ini, "video:colorize", MAKE_INIBOOL(o->colorize));

	iniparser_set(ini, "sound", NULL);
	iniparser_set(ini, "sound:enable", MAKE_INIBOOL(o->sound_enable));
	snprintf(val, N, "%d", o->buffer_size);
	iniparser_set(ini, "sound:buffer_size", val);
	snprintf(val, N, "%d", o->sound_freq);
	iniparser_set(ini, "sound:frequency", val);
	snprintf(val, N, "%d", o->sample_volume);
	iniparser_set(ini, "sound:sample_volume", val);
	snprintf(val, N, "%d", o->music_volume);
	iniparser_set(ini, "sound:music_volume", val);

	iniparser_set(ini, "game", NULL);
	snprintf(val, N, "%d", o->max_levels);
	iniparser_set(ini, "game:max_levels", val);

	// only add them, if levels have been cleared
	for (int i = 0; i < MAX_LEVELS; ++i) {
		#define O 24
		char opt[O];

		if (o->cherries[i] > 0) {
			snprintf(opt, O, "game:level%02d_cherries", i);
			snprintf(val, N, "%d", o->cherries[i]);
			iniparser_set(ini, opt, val);
		}
		if(o->stars[i] > 0) {
			snprintf(opt, O, "game:level%02d_stars", i);
			snprintf(val, N, "%d", o->stars[i]);
			iniparser_set(ini, opt, val);
		}
		#undef O
	}
	if (o->one_hundred)
		iniparser_set(ini, "game:one_hundred", MAKE_INIBOOL(o->one_hundred));

	// save
	fprintf(fp, ini_video_preamble);
	iniparser_dumpsection_ini(ini, "video", fp);
	fprintf(fp, ini_sound_preamble);
	iniparser_dumpsection_ini(ini, "sound", fp);
	fprintf(fp, ini_game_preamble);
	iniparser_dumpsection_ini(ini, "game", fp);

	iniparser_freedict(ini);
	fclose(fp);
	#undef N
}

// loads the data structure from disk
bool load_options(Toptions *o, const char *file) {
	int tmp;
	dictionary *ini = iniparser_load(file);
	if(!ini) return false;

	o->fullscreen = iniparser_getboolean(ini, "video:fullscreen", o->fullscreen);
	o->width = iniparser_getint(ini, "video:width", o->width);
	o->height = iniparser_getint(ini, "video:height", o->height);
	o->use_vsync = iniparser_getboolean(ini, "video:vsync", o->use_vsync);
	o->colorize = iniparser_getboolean(ini, "video:colorize", o->colorize);

	o->sound_enable = iniparser_getboolean(ini, "sound:enable", o->sound_enable);
	o->buffer_size = iniparser_getint(ini, "sound:buffer_size", o->buffer_size);
	o->sound_freq = iniparser_getint(ini, "sound:frequency", o->sound_freq);
	tmp = iniparser_getint(ini, "sound:sample_volume", o->sample_volume);
	o->sample_volume = CLAMP(tmp, 0, 100);
	tmp = iniparser_getint(ini, "sound:music_volume", o->music_volume);
	o->music_volume = CLAMP(tmp, 0, 100);

	o->max_levels = iniparser_getint(ini, "game:max_levels", o->max_levels);
	for (int i = 0; i < MAX_LEVELS; ++i) {
		#define O 24
		char opt[O];

		snprintf(opt, O, "game:level%02d_cherries", i);
		o->cherries[i] = iniparser_getint(ini, opt, o->cherries[i]);

		snprintf(opt, O, "game:level%02d_stars", i);
		o->stars[i] = iniparser_getint(ini, opt, o->stars[i]);
		#undef O
	}
	o->one_hundred = iniparser_getboolean(ini, "game:one_hundred", o->one_hundred);

	iniparser_freedict(ini);
	return true;
}

// resets all data
void reset_options(Toptions *o) {
	o->fullscreen = false;
	o->width = SCREEN_W;
	o->height = SCREEN_H;
	o->use_vsync = true;
	o->colorize = false;

	o->sound_enable = true;
	o->buffer_size = 1024;
	o->sound_freq = SOUND_FREQ;
	o->sample_volume = 100;
	o->music_volume = 100;

	o->max_levels = 0;
	for(int i = 0; i < MAX_LEVELS; i ++) {
		o->cherries[i] = 0;
		o->stars[i] = 0;
	}
	o->one_hundred = false;
}
