```
**************************************************************
*         _____    __                       _____            *
*        /  _  \  |  |    ____  ___  ___   /  |  |           *
*       /  /_\  \ |  |  _/ __ \ \  \/  /  /   |  |_          *
*      /    |    \|  |__\  ___/  >    <  /    ^   /          *
*      \____|__  /|____/ \___  >/__/\_ \ \____   |           *
*              \/            \/       \/      |__|           *
*                                                            *
**************************************************************
```

# INTRODUCTION

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

# LICENSE

Alex the Allegator 4 is a small platform game, created to match the retro style
of Nintendo GameBoy games.
It is Copyright (C) 2003 Johan Peitz (https://johanpeitz.com/), Free Lunch Design.

This source code is released under the The GNU General Public License (GPL).
Please refer to the document [LICENSE.md](LICENSE.md) for license information.

# COMPILING

If you are not content with just reading the code and want to compile it, here
is how. First you need the libraries that Alex4 uses, SDL and SDL_mixer,
version 2. See https://libsdl.org.

How to install and configure the libraries for your platform should be covered
in the help files that come with the download of each library.

There, having installed libraries, just use CMake to compile all the source
files. It should work for Unix-like platforms and Windows:

```cmake
cmake -B build-dir .
cmake --build build-dir
cmake --install build-dir
```

Please bear in mind that this code is supplied for learning and it can not be
supported (by Free Lunch Design) in any way. However, you can open issues in
the repository of the SDL2 port: https://github.com/carstene1ns/alex4 

# AUTHORS AND CONTACT

Free Lunch Design was an independent game developer from Sweden. Their aim was
to create small, high quality games, where gameplay is in focus. It has closed
down in late 2016, their website is defunct nowadays.

Alex the Allegator 4 was written and designed by Johan Peitz. Johan Peitz also
drew the graphics. The fabulous music was composed by Anders Svensson who also
created the sound effects.
The SDL2 port was done by carstene1ns, who is not affiliated with FLD in any
way, but maintains the code nowadays.
