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
 *    Written by Johan Peitz (https://johanpeitz.com/)        *
 **************************************************************
 *    This source code is released under the The GNU          *
 *    General Public License (GPL). Please refer to the       *
 *    document license.txt in the source directory or         *
 *    http://www.gnu.org for license information.             *
 **************************************************************/


INTRODUCTION

Nine times out of ten when I start to write a game, I have planned to make a
small game. Nine times out of ten, the game grows halfway through the code into
something too big for the code to hold. Usually at this point, I am too lazy to
refactor the code so that it can handle the scaling. Instead I hack. Hacking
is fun, but not very good practice. Still, if it looks right on the screen,
then it is right. Right? :)

Anyway, Alex4 started as a small game and grew from there. Hence, the code is
not exactly great at some places. Still I think it is quite ok in most places.
What I am trying to say is that if you are going to learn from this code, do
not take up the bad parts, and do not look at it as the only way. It is just
my way.


LICENSE

Alex the Allegator 4 is a small platform game, created to match the retro style
of Nintendo GameBoy games. It is Copyright (C) Johan Peitz, Free Lunch Design.

This source code is free; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This source code is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
St, Fifth Floor, Boston, MA 02110-1301, USA.


COMPILING

If you are not content with just reading the code and want to compile it, here
is how. First you need the libraries that Alex4 uses. Allegro and DUMB, get
them  on their web sites:

	http://liballeg.org/old.html (>4.2, <5) and http://dumb.sf.net

How to install and configure the libraries for your platform should be covered
in the help files that come with the download of each library.

There, having installed libraries, just compile all the source files. Make sure
that you maintain the directory structure so that the c-files can find the
h-files that they need (mostly in the data/ directory). A basic Makefile is
provided for Unix-like platforms.

Please bear in mind that this code is supplied for learning and it can not be
supported (by Free Lunch Design) in any way.


AUTHORS AND CONTACT

Free Lunch Design was an independent game developer from Sweden. Their aim was
to create small, high quality games, where gameplay is in focus. It has closed
down in late 2016, their website is defunct nowadays.

Alex the Allegator 4 was written and designed by Johan Peitz. Johan Peitz also
drew the graphics. The fabulous music was composed by Anders Svensson who also
created the sound effects.


                                   - the end -
