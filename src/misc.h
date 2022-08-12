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

#ifndef MISC_H
#define MISC_H

#include "port.h"

void log2file(const char *format, ...);
void take_screenshot(BITMAP *bmp);
void clear_trailing_whitespace(char *data);

// a little bounding box quickie
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) \
                           (!( ((x1)>=(x2)+(w2)) \
                            || ((x2)>=(x1)+(w1)) \
                            || ((y1)>=(y2)+(h2)) \
                            || ((y2)>=(y1)+(h1)) ))

#endif
