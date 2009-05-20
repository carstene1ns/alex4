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
 
 
 
#ifndef _SCROLLER_H_
#define _SCROLLER_H_

#include "allegro.h"

// maximum lines in a vertical scroller
#define MAX_LINES_IN_SCROLLER	512			// used for vert-scrolling


// struct for scroller
typedef struct {
	int horizontal;			// TRUE if the scroller is horiz. FALSE o/w
	char *text;				// ptr to actual text
	FONT *fnt;				// ptr to font to use
	int font_height;		
	int width, height;		// size of window
	int offset;				
	int rows;
	int length;
	char *lines[MAX_LINES_IN_SCROLLER];
} Tscroller;


// functions
void init_scroller(Tscroller *sc, FONT *f, char *t, int w, int h, int horiz);
void draw_scroller(Tscroller *sc, BITMAP *bmp, int x, int y);
void scroll_scroller(Tscroller *sc, int step);
int scroller_is_visible(Tscroller *sc);
void restart_scroller(Tscroller *sc);



#endif