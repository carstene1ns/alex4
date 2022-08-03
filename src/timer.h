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

#ifndef TIMERS_H
#define TIMERS_H

// the variables used by the timers
extern volatile int frame_count;
extern volatile int fps;
extern volatile int logic_count;
extern volatile int lps;
extern volatile int cycle_count;
extern volatile int game_count;

// functions
int install_timers();
Uint32 fps_counter(Uint32 interval, void *param);
Uint32 cycle_counter(Uint32 interval, void *param);

#endif
