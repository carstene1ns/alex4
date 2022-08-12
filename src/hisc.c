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

#include <string.h>
#include <stdlib.h>
#include "port.h"
#include "hisc.h"

// creates a table to work with
Thisc* make_hisc_table() {
	Thisc *tmp;

	tmp = malloc(MAX_SCORES*sizeof(Thisc));
	if (!tmp) return NULL;

	return tmp;
}

// destroys a table 
void destroy_hisc_table(Thisc *table) {
	free(table);
}

// check if provided score is allowed to enter the table
int qualify_hisc_table(Thisc *table, Thisc post) {
	int i;
	if (post.score == 0) return 0;
	for (i=0;i<MAX_SCORES;i++) {
		if (post.score >= table[i].score) return 1;
	}
	return 0;
}
 
// sorts the table
void sort_hisc_table(Thisc *table) {
	int i,j;
	Thisc tmp;

	for (i=1;i<MAX_SCORES;i++) {
		j=i;
		tmp = table[i];
		while (j!=0 && tmp.score > table[j-1].score) {
			table[j]= table[j-1];
			j--;
		}
		table[j] = tmp;
	}
}

// Replaces the lowest score with the specified
void enter_hisc_table(Thisc *table, Thisc post) { 
	unsigned int lo = table[0].score;
	int loID = 0;
	int i;

	// find lowest score
	for (i = 0; i < MAX_SCORES; i ++) {
		if (table[i].score <= lo) {
			loID = i;
			lo = table[i].score;
		}
	}

	table[loID] = post;
}

// Resets the table to the values specified
void reset_hisc_table(Thisc *table, const char *name, int hi, int lo) {
	int i;
	int d = (hi-lo)/(MAX_SCORES - 1);
	int acc = lo;

	for (i=0;i<MAX_SCORES;i++) {
		strcpy(table[i].name, name);
		table[i].level = i * 2 + 2;
		table[i].score = acc;
		acc += d;
	}
}

// generates a checksum for the entry
int generate_checksum(Thisc *entry) {
	int i = entry->score * 37;
	char *s = entry->name;

	for(; *s; s++) i = 131*i + *s;

	return i;
}

// loads table from disk, returns 1 on success
bool load_hisc_table(Thisc *table, const char *filename) {
	int i;
	bool ok = true;
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");

	for(i=0; i<MAX_SCORES; i++) {
		int c_disk, c_real;
		// load entry
		SDL_RWread(rw, &table[i].name, sizeof(table[i].name), 1);
		SDL_ReadLE16(rw); // 2 bytes padding
		table[i].score = SDL_ReadLE32(rw);
		table[i].level = SDL_ReadLE32(rw);
		// load checksum
		c_disk = SDL_ReadLE32(rw);
		// generate check sum
		c_real = generate_checksum(&table[i]);
		// compare checksums
		if (c_real != c_disk) ok = false; // tampered with
	}
	SDL_RWclose(rw);

	return ok;
}

// saves table to disk
void save_hisc_table(Thisc *table, const char *filename) {
	int i;
	SDL_RWops* rw = SDL_RWFromFile(filename, "wb");

	for(i=0; i<MAX_SCORES; i++) {
		int checksum;
		// save entry
		SDL_RWwrite(rw, &table[i].name, sizeof(table[i].name), 1);
		SDL_WriteLE16(rw, 0); // 2 bytes padding
		SDL_WriteLE32(rw, table[i].score);
		SDL_WriteLE32(rw, table[i].level);
		// generate check sum
		checksum = generate_checksum(&table[i]);
		// save checksum
		SDL_WriteLE32(rw, checksum);
	}
	SDL_RWclose(rw);
}

// draws a single hisc post
void draw_hisc_post(Thisc *table, BITMAP *bmp, int x, int y, int color, int show_level) {
	textprintf_ex(bmp, x, y, color, -1, "%s", table->name);
	if (show_level) textprintf_right_ex(bmp, x+80, y, color, -1, "%2d", table->level);
	textprintf_right_ex(bmp, x+140, y, color, -1, "%d", table->score);
}

// draws the entire table
void draw_hisc_table(Thisc *table, BITMAP *bmp, int x, int y, int color, int show_level) {
	int i;

	for(i=0;i<MAX_SCORES;i++)
		draw_hisc_post(&table[i], bmp, x, y + 11 * i, color, show_level);
}
