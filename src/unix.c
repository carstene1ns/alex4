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

#ifdef __unix__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

char *get_homedir(void)
{
	struct passwd *pw;
	char *home;

	home = getenv("HOME");
	if (home)
		return home;

	if (!(pw = getpwuid(getuid())))
	{
		fprintf(stderr, "Who are you? Not found in passwd database!!\n");
		return NULL;
	}

	return pw->pw_dir;
}

//-----------------------------------------------------------------------------
int check_and_create_dir(const char *name)
{
	struct stat stat_buffer;

	if (stat(name, &stat_buffer))
	{
		/* error check if it doesn't exist or something else is wrong */
		if (errno == ENOENT)
		{
			/* doesn't exist letts create it ;) */
			if (mkdir(name, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH))
				{
					fprintf(stderr, "Error creating dir %s", name);
					perror(" ");
					return -1;
				}
		}
		else
		{
			/* something else went wrong yell about it */
			fprintf(stderr, "Error opening %s", name);
			perror(" ");
			return -1;
		}
	}
	else
	{
		/* file exists check it's a dir otherwise yell about it */
		if (!S_ISDIR(stat_buffer.st_mode))
		{
			fprintf(stderr,"Error %s exists but isn't a dir\n", name);
			return -1;
		}
	}
	return 0;
}

#endif
