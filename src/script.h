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

#ifndef SCRIPT_H
#define SCRIPT_H

// struct holding script data
typedef struct {
	char name[16];
	int x, y, vx, vy;		// position and velocity
	BITMAP *bmp[4];
	int frames;
	int dir;
	int line;
	struct Tscript_object *line_from;
	struct Tscript_object *line_to;

	struct Tscript_object *next;
} Tscript_object;

// max number of sounds played by the script
#define MAX_SCRIPT_SOUNDS	16
// array holding the sounds ids
extern int active_sounds[MAX_SCRIPT_SOUNDS];

// functions
int run_script(int id);
void draw_speak_bulb(BITMAP *bmp, int src_x, int src_y, int up, int left, int lines, char **rows, int arrow);

#endif
