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
	int i;
	pack_iputl(o->max_levels, fp);
	for (i = 0; i < MAX_LEVELS; ++i)
		pack_iputl(o->cherries[i], fp);
	for (i = 0; i < MAX_LEVELS; ++i)
		pack_iputl(o->stars[i], fp);
	pack_iputl(o->use_vsync, fp);
	pack_iputl(o->one_hundred, fp);
}

// loads the data structure from disk
void load_options(Toptions *o, PACKFILE *fp) {
	int i;
	o->max_levels = pack_igetl(fp);
	for (i = 0; i < MAX_LEVELS; ++i)
		o->cherries[i] = pack_igetl(fp);
	for (i = 0; i < MAX_LEVELS; ++i)
		o->stars[i] = pack_igetl(fp);
	o->use_vsync = pack_igetl(fp);
	o->one_hundred = pack_igetl(fp);
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
