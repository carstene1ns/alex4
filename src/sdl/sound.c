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

#include <SDL_mixer.h>
#include "port.h"

#include "sound.h"
#include "misc.h"
#include "data.h"
#include "options.h"

// sound fx
struct _sfx {
	Mix_Chunk *chunk;
	int chan;
} sfx[S_MAX] = { 0 };

Mix_Music *music = NULL;
bool sounds_loaded = false;
bool music_loaded = false;

bool audio_open = false;

#define MAX_VOL 100 // actually MIX_MAX_VOLUME is 128
#define MAX_PAN 255
int music_vol = MAX_VOL;
int sound_vol = MAX_VOL;

bool init_sound(Toptions *o) {
	if(Mix_Init(MIX_INIT_MOD) != MIX_INIT_MOD)
		printf("error loading mod music support: %s\n", Mix_GetError());
	if(Mix_OpenAudio(o->sound_freq, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, o->buffer_size) != 0) {
		return false;
	}

	Mix_AllocateChannels(16);
	audio_open = true;
	return true;
}

void uninit_sound() {
	if (audio_open) {
		Mix_CloseAudio();
		audio_open = false;
	}
	Mix_Quit();
}

void ResetChan(int channel) {
	for(int i = 0; i < S_MAX; i++) {
		if (sfx[i].chan == channel) {
			sfx[i].chan = -1;
			return;
		}
	}
}

void load_sfx() {
	log2file(" loading sounds");

	for(int i = 0; i < S_MAX; i++) {
		SDL_RWops *rw = SDL_RWFromConstMem(sounds[i].dat, sounds[i].size);
		sfx[i].chunk = Mix_LoadWAV_RW(rw, 1);
		sfx[i].chan = -1;
	}
	Mix_ChannelFinished(ResetChan);
	sounds_loaded = true;
}

void load_music(int id) {
	log2file(" loading music");

	if (music_loaded) {
		printf("error: music already loaded\n");
		return;
	}

	SDL_RWops *rw = SDL_RWFromConstMem(musics[id].dat, musics[id].size);
	music = Mix_LoadMUSType_RW(rw, MUS_MOD, 1);
	if (!music) {
		printf("error loading music: %s\n", Mix_GetError());
		return;
	}
	music_loaded = true;
}

void unload_music() {
	if(!music_loaded) return;

	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music_loaded = false;
}

void free_sfx() {
	if(!sounds_loaded) return;

	// all
	Mix_HaltChannel(-1);

	for(int i = 0; i < S_MAX; i++) {
		if (sfx[i].chunk != NULL)
			Mix_FreeChunk(sfx[i].chunk);
	}
	sounds_loaded = false;
}

void set_sound_volume(int vol) {
	sound_vol = CLAMP(vol, 0, MAX_VOL);
}

void set_music_volume(int vol) {
	music_vol = CLAMP(vol, 0, MAX_VOL);
}

// plays a sample using user settings (from an index)
void play_sound_id_ex(int id, int vol, int freq, int loop) {
	if(!sounds_loaded || !sfx[id].chunk) return;

	int v = sound_vol * (float)vol/MAX_VOL;
	// FIXME: ignoring pitch
	if (freq != 1000)
		printf("ignoring pitch!\n");
	int chan = Mix_PlayChannel(-1, sfx[id].chunk, loop == 1 ? -1 : 0);
	Mix_Volume(chan, v);
	Mix_SetPanning(chan, MAX_PAN, MAX_PAN); // unregister effect
	sfx[id].chan = chan;

#ifdef DEBUG_SOUND
	printf("Playing sound %d on channel %d\n", id, chan);
#endif
}

// plays a sample using default settings (from an index)
void play_sound_id(int id) {
	play_sound_id_ex(id, MAX_VOL, 1000, 0);
}

// stops a sample (providing an id)
void stop_sound_id(int id) {
	if(!sounds_loaded) return;

	if (sfx[id].chan != -1) {
		Mix_HaltChannel(sfx[id].chan);
		return;
	}

#ifdef DEBUG_SOUND
	printf("Cannot stop sound %d: not playing!\n", id);
#endif
}

// adjusts a sample (from an index) according to player position
void adjust_sound_id_ex(int id, int player_x, int x) {
	if(!sounds_loaded) return;

	int vol = MAX(MAX_VOL - ABS(player_x - x) / 2, 0);
	int v = sound_vol * (float)vol/MAX_VOL;
	int pan = CLAMP(0, 128 + x - player_x, 255);
	if (sfx[id].chan != -1) {
		Mix_Volume(sfx[id].chan, v);
		Mix_SetPanning(sfx[id].chan, 255-pan, pan);
		return;
	}

#ifdef DEBUG_SOUND
	printf("Cannot adjust sound %d: not playing!\n", id);
#endif
}

// stops any mod playing
void stop_music(void) {
	if(!music_loaded) return;

	Mix_HaltMusic();
}

// starts the mod at position x
void start_music(int startorder) {
	if(!music_loaded) return;

	stop_music();

	Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(music_vol);
	Mix_ModMusicJumpToOrder(startorder);
}

void pause_music(bool p) {
	if(!music_loaded) return;

	if (p)
		Mix_PauseMusic();
	else
		Mix_ResumeMusic();
}
