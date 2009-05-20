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
 
 
 


 
#include <stdio.h>
#include <string.h>

#include "allegro.h"
#include "scroller.h"


// initiates a scroller
// the input text (t) will be messed up if horiz = 0 (vertical scroller)
void init_scroller(Tscroller *sc, FONT *f, char *t, int w, int h, int horiz) {
	sc->fnt = f;
	sc->font_height = text_height(sc->fnt);
	sc->height = h;
	sc->horizontal = horiz;
	sc->text = t;
	sc->width = w;	
	if (sc->horizontal) {
		sc->length = text_length(sc->fnt, sc->text);
		sc->offset = sc->width;
	}
	else {
		int i;
		int len = strlen(sc->text);

		sc->lines[0] = sc->text;
		sc->rows = 1;
		for(i=0;i<len;i++) 
			if (sc->text[i] == '\n' && sc->rows < MAX_LINES_IN_SCROLLER) {
				sc->lines[sc->rows] = &sc->text[i+1];
				sc->rows ++;
				sc->text[i] = '\0';
			}

		sc->offset = sc->height;
	}
}


// draws the scroller
void draw_scroller(Tscroller *sc, BITMAP *bmp, int x, int y) {
	if (sc->horizontal) {
		if (sc->offset < -sc->length) return;
		if (sc->offset > sc->width) return;
		set_clip(bmp, x, y, x + sc->width, y + sc->height);
		textout(bmp, sc->fnt, sc->text, x + sc->offset + 1, y + 1, 1);
		textout(bmp, sc->fnt, sc->text, x + sc->offset, y, 3);
		set_clip(bmp, 0, 0, bmp->w-1, bmp->h-1);
	}
	else {
		int i;
		if (sc->offset < -sc->rows * sc->font_height) return;
		if (sc->offset > sc->height) return;
		set_clip(bmp, x, y, x + sc->width, y + sc->height);
		for(i=0;i<sc->rows;i++) {
			if (i * sc->font_height + sc->offset <= sc->height) 
				if ((i+1) * sc->font_height + sc->offset >= 0)
					textout_centre(bmp, sc->fnt, sc->lines[i], x+(sc->width>>1) , i * sc->font_height + y + sc->offset, -1);
		}
		set_clip(bmp, 0, 0, bmp->w-1, bmp->h-1);
	}
}


// scrolls the text #step steps
void scroll_scroller(Tscroller *sc, int step) {
	sc->offset += step;
}

// returns TRUE if the scroller is visible in it's windows
int scroller_is_visible(Tscroller *sc) {
	if (sc->horizontal) {
		if (sc->offset < -sc->length) return FALSE;
		if (sc->offset > sc->width) return FALSE;
	}
	else {
		if (sc->offset < -sc->rows * sc->font_height) return FALSE;
		if (sc->offset > sc->height) return FALSE;
	}

	return TRUE;
}


// restarts the offset
void restart_scroller(Tscroller *sc){
	if (sc->horizontal)
		sc->offset = sc->width;
	else
		sc->offset = sc->height;
}

