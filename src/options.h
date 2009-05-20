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
 
 
 

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "allegro.h"

// number of levels to count cherries/stars on
#define MAX_LEVELS		64

// the options struct
typedef struct {
	int max_levels;
	int cherries[MAX_LEVELS];
	int stars[MAX_LEVELS];
	int use_vsync;
	int one_hundred;
} Toptions;


// functions
void save_options(Toptions *o, PACKFILE *fp);
void load_options(Toptions *o, PACKFILE *fp);
void reset_options(Toptions *o);

#endif