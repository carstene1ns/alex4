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
 
 
 

#include <string.h>
#include "allegro.h"
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
void reset_hisc_table(Thisc *table, char *name, int hi, int lo) {
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
int load_hisc_table(Thisc *table, PACKFILE *fp) {
	int i;
	int ok = 1;

	for(i=0; i<MAX_SCORES; i++) {
		int c_disk, c_real;
		// load entry
		pack_fread(&table[i], sizeof(Thisc), fp);
		// load checksum
		pack_fread(&c_disk, sizeof(int), fp);
		// generate check sum
		c_real = generate_checksum(&table[i]);
		// compare checksums
		if (c_real != c_disk) ok = 0; // tampered with
	}

	return ok;
}


// saves table to disk
void save_hisc_table(Thisc *table, PACKFILE *fp) {
	int i;

	for(i=0; i<MAX_SCORES; i++) {
		int checksum;
		// save entry
		pack_fwrite(&table[i], sizeof(Thisc), fp);
		// generate check sum
		checksum = generate_checksum(&table[i]);
		// save checksum
		pack_fwrite(&checksum, sizeof(int), fp);
	}
}

// draws a single hisc post
void draw_hisc_post(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level) {
	textprintf(bmp, fnt, x, y, color, "%s", table->name);
	if (show_level) textprintf_right(bmp, fnt, x+80, y, color, "%2d", table->level);
	textprintf_right(bmp, fnt, x+140, y, color, "%d", table->score);
}

// draws the entire table
void draw_hisc_table(Thisc *table, BITMAP *bmp, FONT *fnt, int x, int y, int color, int show_level) {
	int i;

	for(i=0;i<MAX_SCORES;i++)
		draw_hisc_post(&table[i], bmp, fnt, x, y + 11 * i, color, show_level);
}




