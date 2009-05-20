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
#include "options.h"


// saves the data structure to disk
void save_options(Toptions *o, PACKFILE *fp) {
	pack_fwrite(o, sizeof(Toptions), fp);
}

// loads the data structure from disk
void load_options(Toptions *o, PACKFILE *fp) {
	pack_fread(o, sizeof(Toptions), fp);
}

// resets all data
void reset_options(Toptions *o) {
	int i;

	o->use_vsync = 0;
	o->max_levels = 0;
	
	for(i = 0; i < MAX_LEVELS; i ++) {
		o->cherries[i] = 0;
		o->stars[i] = 0;
	}
}
