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
 
 
 

#include "allegro.h"
#include "timer.h"

// keeps track of frames each second
void fps_counter(void) {
	fps = frame_count;
	frame_count = 0;
	lps = logic_count;
	logic_count = 0;
}
END_OF_FUNCTION(fps_counter);


// keeps track of internal game speed
void cycle_counter(void) {
	cycle_count++;
	game_count++;
}
END_OF_FUNCTION(game_counter);


// initiates the timers
int install_timers() {
	install_timer();
	LOCK_VARIABLE(cycle_count);
	LOCK_VARIABLE(logic_count);
	LOCK_VARIABLE(lps);
	LOCK_VARIABLE(fps);
	LOCK_VARIABLE(frame_count);
	LOCK_FUNCTION(fps_counter);
	install_int(fps_counter, 1000);
	fps = 0;
	frame_count = 0;
	cycle_count = 0;
	LOCK_FUNCTION(cycle_counter);
	install_int(cycle_counter, 20);

	game_count ++;

	return TRUE;
}