
/*****************************************
 * data_files.h - Part of alex4 project. *
 *                                       *
 * GENERATED header, do not edit!        *
 *****************************************/

#ifndef DATA_FILES_H
#define DATA_FILES_H

// PUBLIC enums

enum _images {
	I_ALEX = 0, I_ALEX_BG, I_ALEX_LOGO, I_ALL100, I_BG0, I_BG1, I_BG2,
	I_BULB_BL, I_BULB_BR, I_BULB_TL, I_BULB_TR, I_BULBA_BL, I_BULBA_BR,
	I_BULBA_TL, I_BULBA_TR, I_BULLET_1, I_BULLET_2, I_CAGE_BIG, I_CAGE_SML1,
	I_CAGE_SML2, I_CHAIN, I_CHERRY, I_CHOPPER1, I_CHOPPER2, I_CHOPPER3,
	I_CHOPPER4, I_DRUMCAN, I_EGG, I_EGG2, I_ENEMY1_01, I_ENEMY1_02,
	I_ENEMY1_03, I_ENEMY1_04, I_ENEMY1_05, I_ENEMY2_01, I_ENEMY2_02,
	I_ENEMY2_03, I_ENEMY2_04, I_ENEMY2_05, I_ENEMY3, I_ENEMY4, I_ENEMY5_01,
	I_ENEMY5_02, I_ENEMY6, I_FLD_HEAD, I_FLD_LOGO, I_GAME_OVER, I_GUARD1_1,
	I_GUARD1_2, I_GUARD1_3, I_GUARD1_4, I_GUARD1_5, I_GUARD1_6, I_GUARD1_7,
	I_GUARD1_8, I_GUARD2_1A, I_GUARD2_1B, I_GUARD2_2, I_GUARD2_3, I_GUARD2_4,
	I_GUARD2_5, I_GUARD2_6, I_HEART, I_HEART2, I_HERO000, I_HERO001, I_HERO002,
	I_HERO003, I_HERO_BALL, I_HERO_EAT, I_HERO_FULL, I_HERO_JUMP, I_HERO_NORM,
	I_HERO_SPIT, I_INTRO_BG, I_LETSGO, I_LEVELCOMPLETE, I_LOLA, I_ONEUP,
	I_PARTICLE_BOPP, I_PARTICLE_DUST, I_POINTER, I_SHIP0, I_SHIP1, I_SHIP100,
	I_SMOKE1, I_SMOKE2, I_SMOKE3, I_SMOKE4, I_SMOKE5, I_SMOKE6, I_SMOKE7,
	I_SMOKE8, I_SPIN1, I_SPIN2, I_SPIN3, I_SPIN4, I_STAR, I_TILESET, I_UFO0,
	I_UFO1, I_UFO_BIG, I_UFOBEAM, I_WATER, I_WATER_SURFACE, 
	// end marker
	I_MAX
};

enum _sounds {
	S_CHERRY = 0, S_CHOPPER, S_CRUSH, S_DIE, S_EAT, S_ENERGY, S_ENGINE,
	S_HEART, S_HIT, S_HURT, S_IMPACT, S_JUMP, S_KILL, S_MENU, S_PAUSE, S_POINT,
	S_SHIP, S_SHOOT, S_SPIT, S_STAR, S_STARTUP, S_STOMP, S_TALK, S_TURN,
	S_XTRALIFE, 
	// end marker
	S_MAX
};

enum _music {
	MSC_GAME = 0, 
	// end marker
	MSC_MAX
};

enum _maps {
	MAP_01_LEVEL1 = 0, MAP_02_LEVEL2, MAP_03_LEVEL3, MAP_04_LEVEL4,
	MAP_05_GUARDIAN1, MAP_06_LEVEL5, MAP_07_LEVEL6, MAP_08_LEVEL7,
	MAP_09_LEVEL8, MAP_10_LEVEL9, MAP_11_LEVEL10, MAP_12_GUARDIAN2, 
	// end marker
	MAP_MAX
};

enum _fonts {
	F_GAME = 0, 
	// end marker
	F_MAX
};

// PRIVATE mappings

#ifdef INSIDE_DATA

const char *_image_mapping[I_MAX] = {
	"images/alex.png", "images/alex_bg.png", "images/alex_logo.png",
	"images/all100.png", "images/bg0.png", "images/bg1.png", "images/bg2.png",
	"images/bulb_bl.png", "images/bulb_br.png", "images/bulb_tl.png",
	"images/bulb_tr.png", "images/bulba_bl.png", "images/bulba_br.png",
	"images/bulba_tl.png", "images/bulba_tr.png", "images/bullet_1.png",
	"images/bullet_2.png", "images/cage_big.png", "images/cage_sml1.png",
	"images/cage_sml2.png", "images/chain.png", "images/cherry.png",
	"images/chopper1.png", "images/chopper2.png", "images/chopper3.png",
	"images/chopper4.png", "images/drumcan.png", "images/egg.png",
	"images/egg2.png", "images/enemy1_01.png", "images/enemy1_02.png",
	"images/enemy1_03.png", "images/enemy1_04.png", "images/enemy1_05.png",
	"images/enemy2_01.png", "images/enemy2_02.png", "images/enemy2_03.png",
	"images/enemy2_04.png", "images/enemy2_05.png", "images/enemy3.png",
	"images/enemy4.png", "images/enemy5_01.png", "images/enemy5_02.png",
	"images/enemy6.png", "images/fld_head.png", "images/fld_logo.png",
	"images/game_over.png", "images/guard1_1.png", "images/guard1_2.png",
	"images/guard1_3.png", "images/guard1_4.png", "images/guard1_5.png",
	"images/guard1_6.png", "images/guard1_7.png", "images/guard1_8.png",
	"images/guard2_1a.png", "images/guard2_1b.png", "images/guard2_2.png",
	"images/guard2_3.png", "images/guard2_4.png", "images/guard2_5.png",
	"images/guard2_6.png", "images/heart.png", "images/heart2.png",
	"images/hero000.png", "images/hero001.png", "images/hero002.png",
	"images/hero003.png", "images/hero_ball.png", "images/hero_eat.png",
	"images/hero_full.png", "images/hero_jump.png", "images/hero_norm.png",
	"images/hero_spit.png", "images/intro_bg.png", "images/letsgo.png",
	"images/levelcomplete.png", "images/lola.png", "images/oneup.png",
	"images/particle_bopp.png", "images/particle_dust.png",
	"images/pointer.png", "images/ship0.png", "images/ship1.png",
	"images/ship100.png", "images/smoke1.png", "images/smoke2.png",
	"images/smoke3.png", "images/smoke4.png", "images/smoke5.png",
	"images/smoke6.png", "images/smoke7.png", "images/smoke8.png",
	"images/spin1.png", "images/spin2.png", "images/spin3.png",
	"images/spin4.png", "images/star.png", "images/tileset.png",
	"images/ufo0.png", "images/ufo1.png", "images/ufo_big.png",
	"images/ufobeam.png", "images/water.png", "images/water_surface.png"
};

const char *_sound_mapping[S_MAX] = {
	"sfx_22/cherry.wav", "sfx_22/chopper.wav", "sfx_22/crush.wav",
	"sfx_22/die.wav", "sfx_22/eat.wav", "sfx_22/energy.wav",
	"sfx_22/engine.wav", "sfx_22/heart.wav", "sfx_22/hit.wav",
	"sfx_22/hurt.wav", "sfx_22/impact.wav", "sfx_22/jump.wav",
	"sfx_22/kill.wav", "sfx_22/menu.wav", "sfx_22/pause.wav",
	"sfx_22/point.wav", "sfx_22/ship.wav", "sfx_22/shoot.wav",
	"sfx_22/spit.wav", "sfx_22/star.wav", "sfx_22/startup.wav",
	"sfx_22/stomp.wav", "sfx_22/talk.wav", "sfx_22/turn.wav",
	"sfx_22/xtralife.wav"
};

const char *_music_mapping[MSC_MAX] = {
	"music/GAME.MOD"
};

const char *_map_mapping[MAP_MAX] = {
	"maps/01_level1.map", "maps/02_level2.map", "maps/03_level3.map",
	"maps/04_level4.map", "maps/05_guardian1.map", "maps/06_level5.map",
	"maps/07_level6.map", "maps/08_level7.map", "maps/09_level8.map",
	"maps/10_level9.map", "maps/11_level10.map", "maps/12_guardian2.map"
};

const char *_font_mapping[F_MAX] = {
	"font/GAME.bmp"
};

#endif // INSIDE_DATA

#endif // DATA_FILES_H

