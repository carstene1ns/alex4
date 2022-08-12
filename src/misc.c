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

#include <stdarg.h>
#include "miniz.h"
#include "misc.h"
#include "sound.h"

extern FILE* log_fp;

// loggs the text to the text file
void log2file(const char *format, ...) {
	va_list ptr; /* get an arg pointer */
	
	if (log_fp) {
		/* initialize ptr to point to the first argument after the format string */
		va_start(ptr, format);
 
		/* Write to logfile. */
		vfprintf(log_fp, format, ptr); // Write passed text.
		fprintf(log_fp, "\n"); // New line..
 
		va_end(ptr);
 
		fflush(log_fp);
	}

}

// saves a screenshot
void take_screenshot(BITMAP *bmp) { 
	static int number = 0;
	char buf[256];
	int ok = 0;

	// check if the file name allready exists
	do {
		sprintf(buf, "a4_%03d.png", number ++);
		if (!exists(buf)) ok = 1;
		if (number > 999) return;
	} while(!ok);

	take_screenshot_platform(bmp, buf);
}

// removes trailing white space from a null terminated string
void clear_trailing_whitespace(char *data) {
	unsigned int i;

	for(i = 0; i < strlen(data); i++) {
		if (data[i] == ' ' || data[i] == '\t' || data[i] == '\n' || data[i] == '\r')
			data[i] = 0;
	}
}
