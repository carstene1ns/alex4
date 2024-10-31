
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

const char *_image_mono_mapping[I_MAX] = {
	"images_mono/alex.bmp", "images_mono/alex_bg.bmp",
	"images_mono/alex_logo.bmp", "images_mono/all100.bmp",
	"images_mono/bg0.bmp", "images_mono/bg1.bmp", "images_mono/bg2.bmp",
	"images_mono/bulb_bl.bmp", "images_mono/bulb_br.bmp",
	"images_mono/bulb_tl.bmp", "images_mono/bulb_tr.bmp",
	"images_mono/bulba_bl.bmp", "images_mono/bulba_br.bmp",
	"images_mono/bulba_tl.bmp", "images_mono/bulba_tr.bmp",
	"images_mono/bullet_1.bmp", "images_mono/bullet_2.bmp",
	"images_mono/cage_big.bmp", "images_mono/cage_sml1.bmp",
	"images_mono/cage_sml2.bmp", "images_mono/chain.bmp",
	"images_mono/cherry.bmp", "images_mono/chopper1.bmp",
	"images_mono/chopper2.bmp", "images_mono/chopper3.bmp",
	"images_mono/chopper4.bmp", "images_mono/drumcan.bmp",
	"images_mono/egg.bmp", "images_mono/egg2.bmp", "images_mono/enemy1_01.bmp",
	"images_mono/enemy1_02.bmp", "images_mono/enemy1_03.bmp",
	"images_mono/enemy1_04.bmp", "images_mono/enemy1_05.bmp",
	"images_mono/enemy2_01.bmp", "images_mono/enemy2_02.bmp",
	"images_mono/enemy2_03.bmp", "images_mono/enemy2_04.bmp",
	"images_mono/enemy2_05.bmp", "images_mono/enemy3.bmp",
	"images_mono/enemy4.bmp", "images_mono/enemy5_01.bmp",
	"images_mono/enemy5_02.bmp", "images_mono/enemy6.bmp",
	"images_mono/fld_head.bmp", "images_mono/fld_logo.bmp",
	"images_mono/game_over.bmp", "images_mono/guard1_1.bmp",
	"images_mono/guard1_2.bmp", "images_mono/guard1_3.bmp",
	"images_mono/guard1_4.bmp", "images_mono/guard1_5.bmp",
	"images_mono/guard1_6.bmp", "images_mono/guard1_7.bmp",
	"images_mono/guard1_8.bmp", "images_mono/guard2_1a.bmp",
	"images_mono/guard2_1b.bmp", "images_mono/guard2_2.bmp",
	"images_mono/guard2_3.bmp", "images_mono/guard2_4.bmp",
	"images_mono/guard2_5.bmp", "images_mono/guard2_6.bmp",
	"images_mono/heart.bmp", "images_mono/heart2.bmp",
	"images_mono/hero000.bmp", "images_mono/hero001.bmp",
	"images_mono/hero002.bmp", "images_mono/hero003.bmp",
	"images_mono/hero_ball.bmp", "images_mono/hero_eat.bmp",
	"images_mono/hero_full.bmp", "images_mono/hero_jump.bmp",
	"images_mono/hero_norm.bmp", "images_mono/hero_spit.bmp",
	"images_mono/intro_bg.bmp", "images_mono/letsgo.bmp",
	"images_mono/levelcomplete.bmp", "images_mono/lola.bmp",
	"images_mono/oneup.bmp", "images_mono/particle_bopp.bmp",
	"images_mono/particle_dust.bmp", "images_mono/pointer.bmp",
	"images_mono/ship0.bmp", "images_mono/ship1.bmp",
	"images_mono/ship100.bmp", "images_mono/smoke1.bmp",
	"images_mono/smoke2.bmp", "images_mono/smoke3.bmp",
	"images_mono/smoke4.bmp", "images_mono/smoke5.bmp",
	"images_mono/smoke6.bmp", "images_mono/smoke7.bmp",
	"images_mono/smoke8.bmp", "images_mono/spin1.bmp", "images_mono/spin2.bmp",
	"images_mono/spin3.bmp", "images_mono/spin4.bmp", "images_mono/star.bmp",
	"images_mono/tileset.bmp", "images_mono/ufo0.bmp", "images_mono/ufo1.bmp",
	"images_mono/ufo_big.bmp", "images_mono/ufobeam.bmp",
	"images_mono/water.bmp", "images_mono/water_surface.bmp"
};

const char *_image_color_mapping[I_MAX] = {
	"images_color/alex.bmp", "images_color/alex_bg.bmp",
	"images_color/alex_logo.bmp", "images_color/all100.bmp",
	"images_color/bg0.bmp", "images_color/bg1.bmp", "images_color/bg2.bmp",
	"images_color/bulb_bl.bmp", "images_color/bulb_br.bmp",
	"images_color/bulb_tl.bmp", "images_color/bulb_tr.bmp",
	"images_color/bulba_bl.bmp", "images_color/bulba_br.bmp",
	"images_color/bulba_tl.bmp", "images_color/bulba_tr.bmp",
	"images_color/bullet_1.bmp", "images_color/bullet_2.bmp",
	"images_color/cage_big.bmp", "images_color/cage_sml1.bmp",
	"images_color/cage_sml2.bmp", "images_color/chain.bmp",
	"images_color/cherry.bmp", "images_color/chopper1.bmp",
	"images_color/chopper2.bmp", "images_color/chopper3.bmp",
	"images_color/chopper4.bmp", "images_color/drumcan.bmp",
	"images_color/egg.bmp", "images_color/egg2.bmp",
	"images_color/enemy1_01.bmp", "images_color/enemy1_02.bmp",
	"images_color/enemy1_03.bmp", "images_color/enemy1_04.bmp",
	"images_color/enemy1_05.bmp", "images_color/enemy2_01.bmp",
	"images_color/enemy2_02.bmp", "images_color/enemy2_03.bmp",
	"images_color/enemy2_04.bmp", "images_color/enemy2_05.bmp",
	"images_color/enemy3.bmp", "images_color/enemy4.bmp",
	"images_color/enemy5_01.bmp", "images_color/enemy5_02.bmp",
	"images_color/enemy6.bmp", "images_color/fld_head.bmp",
	"images_color/fld_logo.bmp", "images_color/game_over.bmp",
	"images_color/guard1_1.bmp", "images_color/guard1_2.bmp",
	"images_color/guard1_3.bmp", "images_color/guard1_4.bmp",
	"images_color/guard1_5.bmp", "images_color/guard1_6.bmp",
	"images_color/guard1_7.bmp", "images_color/guard1_8.bmp",
	"images_color/guard2_1a.bmp", "images_color/guard2_1b.bmp",
	"images_color/guard2_2.bmp", "images_color/guard2_3.bmp",
	"images_color/guard2_4.bmp", "images_color/guard2_5.bmp",
	"images_color/guard2_6.bmp", "images_color/heart.bmp",
	"images_color/heart2.bmp", "images_color/hero000.bmp",
	"images_color/hero001.bmp", "images_color/hero002.bmp",
	"images_color/hero003.bmp", "images_color/hero_ball.bmp",
	"images_color/hero_eat.bmp", "images_color/hero_full.bmp",
	"images_color/hero_jump.bmp", "images_color/hero_norm.bmp",
	"images_color/hero_spit.bmp", "images_color/intro_bg.bmp",
	"images_color/letsgo.bmp", "images_color/levelcomplete.bmp",
	"images_color/lola.bmp", "images_color/oneup.bmp",
	"images_color/particle_bopp.bmp", "images_color/particle_dust.bmp",
	"images_color/pointer.bmp", "images_color/ship0.bmp",
	"images_color/ship1.bmp", "images_color/ship100.bmp",
	"images_color/smoke1.bmp", "images_color/smoke2.bmp",
	"images_color/smoke3.bmp", "images_color/smoke4.bmp",
	"images_color/smoke5.bmp", "images_color/smoke6.bmp",
	"images_color/smoke7.bmp", "images_color/smoke8.bmp",
	"images_color/spin1.bmp", "images_color/spin2.bmp",
	"images_color/spin3.bmp", "images_color/spin4.bmp",
	"images_color/star.bmp", "images_color/tileset.bmp",
	"images_color/ufo0.bmp", "images_color/ufo1.bmp",
	"images_color/ufo_big.bmp", "images_color/ufobeam.bmp",
	"images_color/water.bmp", "images_color/water_surface.bmp"
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

