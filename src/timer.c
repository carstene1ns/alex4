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

#include "port.h"
#include "timer.h"

// the variables used by the timers
volatile int frame_count;
volatile int fps;
volatile int logic_count;
volatile int lps;
volatile int cycle_count;
volatile int game_count;

SDL_TimerID fps_timer, cycle_timer;

// keeps track of frames each second
Uint32 fps_counter(Uint32 interval, void *param) {
	fps = frame_count;
	frame_count = 0;
	lps = logic_count;
	logic_count = 0;

	return interval;
}

// keeps track of internal game speed
Uint32 cycle_counter(Uint32 interval, void *param) {
	cycle_count++;
	game_count++;

	return interval;
}

// initiates the timers
bool install_timers() {
	fps = 0;
	frame_count = 0;
	cycle_count = 0;
	game_count = 0;

	fps_timer = SDL_AddTimer(1000, fps_counter, NULL);
	cycle_timer = SDL_AddTimer(20, cycle_counter, NULL);

	return true;
}

void stop_timers() {
	SDL_RemoveTimer(fps_timer);
	SDL_RemoveTimer(cycle_timer);
}
