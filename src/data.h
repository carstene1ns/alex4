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

#ifndef DATA_H
#define DATA_H

// the generated header
#include "data_files.h"
#include "port.h"

typedef struct _df {
	void *dat;
	size_t size;
} DATAFILE;

bool load_datafile(const char* filename);
void unload_data();

// global pointers to data files
extern BITMAP **bitmaps;
extern DATAFILE *fonts;
extern DATAFILE *maps;
extern DATAFILE *scripts;
extern DATAFILE *scriptmaps;
extern DATAFILE *sounds;
extern DATAFILE *musics;

// scripts (hardcoded)
#define SCR_INTRO 0
#define SCR_OUTRO 1
#define SCR_MAX 2

#endif
