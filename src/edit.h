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
 
 
 
#ifndef _EDIT_H_
#define _EDIT_H_

#include <allegro.h>
#include "map.h"

// the editor can be in one of the following modes
#define EDIT_MODE_DRAW		1
#define EDIT_MODE_SELECT	2
#define EDIT_MODE_STATS		3

// functions
void set_edit_mode(int mode);
char *get_edit_path_and_file();
void set_edit_path_and_file(char *str);
void draw_edit_mode(BITMAP *bmp, Tmap *map, int mx, int my);
void update_edit_mode(Tmap *map, BITMAP *bmp, int mx, int my, int mb);

#endif