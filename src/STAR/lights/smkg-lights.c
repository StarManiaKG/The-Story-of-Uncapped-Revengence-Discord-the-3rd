// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-lights.c
/// \brief TSoURDt3rd's dynamic light data

#include "smkg-lights.h"
#include "smkg-coronas.h"

#include "../../i_video.h"
#include "../../v_video.h"

#ifdef HWRENDER
#include "../../hardware/hw_glob.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define ALPHA_ENABLED

// ------------------------ //
//        Functions
// ------------------------ //

// -----------------------
//              Rendering
// -----------------------

boolean L_RenderPlayerLight(mobj_t *mobj)
{
	player_t *player = NULL;

	if (mobj->sprite != SPR_PLAY)
		return false;

	if (mobj->target && mobj->target->player)
		player = mobj->target->player;
	else if (mobj->tracer && mobj->tracer->player)
		player = mobj->tracer->player;
	else
		player = mobj->player;

	if (player != NULL)
		return (player->powers[pw_super]);
	return (mobj->frame & SPR2F_SUPER);
}

// -----------------------
//               Coloring
// -----------------------

boolean L_UseObjectColor(mobj_t *mobj, RGBA_t *rgba_table, UINT8 *alpha, boolean dynamic)
{
	RGBA_t new_color = V_GetColor(skincolors[mobj->color].ramp[8]);

	if (mobj->color == SKINCOLOR_NONE)
	{
		return false;
	}

#ifdef HWRENDER
	if (rendermode == render_opengl)
	{
		RGBA_t *palette = HWR_GetTexturePalette();
		new_color = palette[skincolors[mobj->color].ramp[8]];
	}
#endif

	if (rgba_table != NULL && new_color.rgba)
	{
		if (dynamic)
		{
			INT32 rgba_value = (INT32)new_color.rgba;
			rgba_table->rgba = LONG(rgba_value);
		}
		else
			rgba_table->rgba = new_color.rgba;
	}

#ifdef ALPHA_ENABLED
	if (alpha != NULL)
		(*alpha) = new_color.s.alpha;
#endif

	return true;
}

boolean L_UseEmeraldLight(mobj_t *mobj, RGBA_t *rgba_table, UINT8 *alpha, boolean dynamic)
{
	light_t *light_to_use = NULL;

	switch (mobj->type)
	{
		case MT_EMERALD1: light_to_use = &lspr[GREENSHINE_L]; break;
		case MT_EMERALD2: light_to_use = &lspr[PINKSHINE_L]; break;
		case MT_EMERALD3: light_to_use = &lspr[BLUESHINE_L]; break;
		case MT_EMERALD4: light_to_use = &lspr[LBLUESHINE_L]; break;
		case MT_EMERALD5: light_to_use = &lspr[ORANGESHINE_L]; break;
		case MT_EMERALD6: light_to_use = &lspr[REDSHINE_L]; break;
		case MT_EMERALD7: light_to_use = &lspr[GREYSHINE_L]; break;
		default:
		{
			switch (mobj->state-states)
			{
				case S_CEMG1: case S_ORBITEM1: light_to_use = &lspr[GREENSHINE_L]; break;
				case S_CEMG2: case S_ORBITEM2: light_to_use = &lspr[PINKSHINE_L]; break;
				case S_CEMG3: case S_ORBITEM3: light_to_use = &lspr[BLUESHINE_L]; break;
				case S_CEMG4: case S_ORBITEM4: light_to_use = &lspr[LBLUESHINE_L]; break;
				case S_CEMG5: case S_ORBITEM5: light_to_use = &lspr[ORANGESHINE_L]; break;
				case S_CEMG6: case S_ORBITEM6: light_to_use = &lspr[REDSHINE_L]; break;
				case S_CEMG7: case S_ORBITEM7: light_to_use = &lspr[GREYSHINE_L]; break;
				case S_ORBITEM8: light_to_use = &lspr[BLACKSHIELD_L]; break;
				default: return false;
			}
			break;
		}
	}

	if (rgba_table != NULL)
	{
		if (dynamic)
			rgba_table->rgba = LONG(light_to_use->dynamic_color);
		else
			rgba_table->rgba = light_to_use->corona_color;
	}

#ifdef ALPHA_ENABLED
	if (alpha != NULL)
	{
#if 0
		(*alpha) = V_GetColor(skincolors[mobj->color].ramp[8]).s.alpha;
#else
		if (rendermode == render_soft)
		{
			(*alpha) = V_GetColor(skincolors[mobj->color].ramp[8]).s.alpha;
		}
#ifdef HWRENDER
		else if (rendermode == render_opengl)
		{
			RGBA_t *palette = HWR_GetTexturePalette();
			(*alpha) = palette[skincolors[mobj->color].ramp[8]].s.alpha;
		}
#endif
#endif
	}
#endif

	return true;
}

//Hurdler: now we can change those values via FS :)
// RGBA( r, g, b, a )
// Indexed by lightspritenum_s
light_t lspr[NUMLIGHTS] =
{	// type       		offset x,   y  		coronas color,	c_radius,	light color,	l_radius, 	sqr radius computed at init, impl_flags,		corona_rendering_routine, corona_coloring_routine
	// default
	{UNDEFINED_SPR,		0.0f,		0.0f, 	0x00000000,		60.0f,		0x00000000,		100.0f,		0.0f, 0,					NULL, NULL},

	// weapons
	// RINGSPARK_L
	{LIGHT_SPR,      	0.0f,   	0.0f, 	0x0000e0ff,  	16.0f, 		0x0000e0ff,  	 32.0f, 	0.0f, 0,					NULL, NULL}, // Tails 09-08-2002
	// PLAYER_L
	// DYNLIGHT_SPR // LIGHT_SPR
	{LIGHT_SPR,   	0.0f,   	0.0f, 	0xff00e0ff,  	32.0f, 		0xff00e0ff, 	128.0f, 	0.0f, 0,			L_RenderPlayerLight, L_UseObjectColor}, // Tails 09-08-2002
	// SUPERSPARK_L
	{LIGHT_SPR,      	0.0f,   	0.0f, 	0xe0ffffff,   	 8.0f, 		0xe0ffffff,  	 64.0f, 	0.0f, 0,					NULL, NULL},
	// INVINCIBLE_L
	{DYNLIGHT_SPR,   	0.0f,   	0.0f, 	0x10ffaaaa,  	16.0f, 		0x10ffaaaa, 	128.0f, 	0.0f, 0,					NULL, NULL},
	// GREENSHIELD_L
	{DYNLIGHT_SPR,		0.0f,		0.0f,	0x602b7337,		/*32.0f*/64.0f,		0x602b7337,		128.0f,		0.0f, 0,					NULL, NULL},
	// BLUESHIELD_L
	{DYNLIGHT_SPR,		0.0f,		0.0f,	0x60cb0000,		/*32.0f*/64.0f,		0x60cb0000,		128.0f,		0.0f, 0,					NULL, NULL},

	// tall lights
	// YELLOWSHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x601f7baf,  /*32.0f*/64.0f, 0x601f7baf, 128.0f, 0.0f, 0,					NULL, NULL},

	// REDSHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x600000cb,  /*32.0f*/64.0f, 0x600000cb, 128.0f, 0.0f, 0,				NULL, NULL},

	// BLACKSHIELD_L // Black light? lol
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60010101,  /*32.0f*/64.0f, 0x60ff00ff, 128.0f, 0.0f, 0,				NULL, NULL},

	// WHITESHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60ffffff,  /*32.0f*/64.0f, 0x60ffffff, 128.0f, 0.0f, 0,				NULL, NULL},

	// SMALLREDBALL_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x606060f0,   0.0f, 0x302070ff,  32.0f, 0.0f, 0,				NULL, NULL},

	// small lights
	// RINGLIGHT_L
	//{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60b0f0f0,   0.0f, 0x30b0f0f0, 100.0f, 0.0f, 0,				NULL, NULL},
	{
		//DYNLIGHT_SPR,
		LIGHT_SPR,
		0.0f, 0.0f,
		0x0000e0ff, 25.0f,
		0x0000e0ff, 25.0f, 0.0f,
		0,
		NULL, NULL
	},

	// GREENSMALL_L
	{    LIGHT_SPR,   0.0f,  14.0f, 0x6070ff70,  60.0f, 0x4070ff70, 100.0f, 0.0f, 0,					NULL, NULL},
	// REDSMALL_L
	{    LIGHT_SPR,   0.0f,  14.0f, 0x705070ff,  60.0f, 0x405070ff, 100.0f, 0.0f, 0,					NULL, NULL},

	// type       offset x,   y  coronas color, c_size,light color,l_radius, sqr radius computed at init
	// GREENSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff00ff00,  64.0f, 0xff00ff00, 256.0f, 0.0f, 0,					NULL, NULL},
	// ORANGESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff0080ff,  64.0f, 0xff0080ff, 256.0f, 0.0f, 0,					NULL, NULL},
	// PINKSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffe080ff,  64.0f, 0xffe080ff, 256.0f, 0.0f, 0,					NULL, NULL},
	// BLUESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffff0000,  64.0f, 0xffff0000, 256.0f, 0.0f, 0,					NULL, NULL},
	// REDSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff0000ff,  64.0f, 0xff0000ff, 256.0f, 0.0f, 0,					NULL, NULL},
	// LBLUESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffff8080,  64.0f, 0xffff8080, 256.0f, 0.0f, 0,					NULL, NULL},
	// GREYSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffe0e0e0,  64.0f, 0xffe0e0e0, 256.0f, 0.0f, 0,					NULL, NULL},

	// monsters
	// REDBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f, 0x606060ff,   0.0f, 0x606060ff, 100.0f, 0.0f, 0,				NULL, NULL},
	// GREENBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f,	0x6060ff60, 120.0f, 0x6060ff60, 100.0f, 0.0f, 0,					NULL, NULL},
	// BLUEBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f,	0x60ff6060, 120.0f, 0x60ff6060, 100.0f, 0.0f, 0,					NULL, NULL},

	// misc.
	// NIGHTSLIGHT_L
	{    LIGHT_SPR,   0.0f,   6.0f, 0x60ffffff,  16.0f, 0x30ffffff,  32.0f, 0.0f, 0,					NULL, NULL},

	// JETLIGHT_L
	{DYNLIGHT_SPR,   0.0f,   6.0f, 0x60ffaaaa,  48.0f/*16.0f*/, 0x30ffaaaa,  64.0f, 0.0f, 0,				NULL, NULL},

	// GOOPLIGHT_L
	{DYNLIGHT_SPR,   0.0f,   6.0f, 0x60ff00ff,  16.0f, 0x30ff00ff,  32.0f, 0.0f, 0,					NULL, NULL},

	// STREETLIGHT_L
	{ LIGHT_SPR,      0.0f,   0.0f, 0xe0ffffff,  64.0f, 0xe0ffffff, 384.0f, 0.0f, 0,						NULL, NULL},

	{	// ROCKET_L
		ROCKET_SPR,
		0.0f, 0.0f,
		0x606060f0, 20.0f,
		0x4020f7f7, 120.0f, 0.0f,
		0,
		NULL, NULL
	},
	{	// ROCKETEXP_L
		//DYNLIGHT_SPR,
		LIGHT_SPR,
		0.0f, 0.0f,
		0x606060f0, 20.0f,
		0x4020f7f7, 200.0f, 0.0f,
		0,
		NULL, NULL
	},

	{	// EMERALD_L
		LIGHT_SPR,
		0.0f, 14.0f,
		0x6070ff70, 60.0f,
		0x4070ff70, 100.0f, 0.0f,
		0,
		NULL, L_UseEmeraldLight
	},

	// GENERIC_MOBJLIGHT_L
	{LIGHT_SPR,      0.0f,   0.0f, 0xe0ffffff,  64.0f, 0xe0ffffff, 384.0f, 0.0f, 0,						NULL, L_UseObjectColor},

	// weapons
	// LT_PLASMA
//    { DYNLIGHT_SPR,  0.0f,   0.0f, 0x60ff7750,  24.0f, 0x20f77760,  80.0f },
	/*{ DYNLIGHT_SPR,  0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x20)},  80.0f },
	// LT_PLASMAEXP
//    { DYNLIGHT_SPR,  0.0f,   0.0f, 0x60ff7750,  24.0f, 0x40f77760, 120.0f },
	{ DYNLIGHT_SPR,  0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x40)}, 120.0f },
	// LT_ROCKET
//    { ROCKET_SPR,   0,   0, 0x606060f0,  20, 0x4020f7f7, 120 },
	{ ROCKET_SPR,   0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x40)}, 120.0f },
	// LT_ROCKETEXP
//    { DYNLIGHT_SPR,  0,   0, 0x606060f0,  20, 0x6020f7f7, 200 },
	{ DYNLIGHT_SPR,  0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x60)}, 200.0f },
	// LT_BFG
//    { DYNLIGHT_SPR,  0,   0, 0x6077f777, 120, 0x8060f060, 200 },
	{ DYNLIGHT_SPR,  0, {RGBA(0x77,0xf7,0x77,0x60)}, 120.0f, {RGBA(0x60,0xf0,0x60,0x80)}, 200.0f },
	// LT_BFGEXP
//    { DYNLIGHT_SPR,  0,   0, 0x6077f777, 120, 0x6060f060, 400 },
	{ DYNLIGHT_SPR,  0, {RGBA(0x77,0xf7,0x77,0x60)}, 120.0f, {RGBA(0x60,0xf0,0x60,0x60)}, 400.0f },

	// tall lights
	// LT_BLUETALL
//    { LIGHT_SPR,    0,  27, 0x80ff7070,  75, 0x40ff5050, 100 },
//    { LIGHT_SPR,    0,27, {RGBA(0x70,0x70,0xff,0x80)},  75.0f, {RGBA(0x50,0x50,0xff,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,27, {RGBA(0x70,0x70,0xff,0x80)},  75.0f, {RGBA(0x50,0x50,0xff,0x40)}, 100.0f },
	// LT_GREENTALL
//    { LIGHT_SPR,    0,  27, 0x5060ff60,  75, 0x4070ff70, 100 },
//    { LIGHT_SPR,    0,27, {RGBA(0x60,0xff,0x60,0x50)},  75.0f, {RGBA(0x70,0xff,0x70,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,27, {RGBA(0x60,0xff,0x60,0x50)},  75.0f, {RGBA(0x70,0xff,0x70,0x40)}, 100.0f },
	// LT_REDTALL
//    { LIGHT_SPR,    0,  27, 0x705070ff,  75, 0x405070ff, 100 },
//    { LIGHT_SPR,    0,27, {RGBA(0xff,0x70,0x50,0x70)},  75.0f, {RGBA(0xff,0x70,0x50,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,27, {RGBA(0xff,0x70,0x50,0x70)},  75.0f, {RGBA(0xff,0x70,0x50,0x40)}, 100.0f },

	// small lights
	// LT_BLUESMALL
//    { LIGHT_SPR,    0,  14, 0x80ff7070,  60, 0x40ff5050, 100 },
//    { LIGHT_SPR,    0,14, {RGBA(0x70,0x70,0xff,0x80)},  60.0f, {RGBA(0x50,0x50,0xff,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,14, {RGBA(0x70,0x70,0xff,0x80)},  60.0f, {RGBA(0x50,0x50,0xff,0x40)}, 100.0f },
	// LT_GREENSMALL
//    { LIGHT_SPR,    0,  14, 0x6070ff70,  60, 0x4070ff70, 100 },
//    { LIGHT_SPR,    0,14, {RGBA(0x70,0xff,0x70,0x60)},  60.0f, {RGBA(0x70,0xff,0x70,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,14, {RGBA(0x70,0xff,0x70,0x60)},  60.0f, {RGBA(0x70,0xff,0x70,0x40)}, 100.0f },
	// LT_REDSMALL
//    { LIGHT_SPR,    0,  14, 0x705070ff,  60, 0x405070ff, 100 },
//    { LIGHT_SPR,    0,14, {RGBA(0xff,0x70,0x50,0x70)},  60.0f, {RGBA(0xff,0x70,0x50,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,14, {RGBA(0xff,0x70,0x50,0x70)},  60.0f, {RGBA(0xff,0x70,0x50,0x40)}, 100.0f },

	// other lights
	// LT_TECHLAMP
//    { LIGHT_SPR,    0,  33, 0x80ffb0b0,  75, 0x40ffb0b0, 100 },
//    { LIGHT_SPR,    0,33, {RGBA(0xb0,0xb0,0xff,0x80)},  75.0f, {RGBA(0xb0,0xb0,0xff,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_lamp,    0,33, {RGBA(0xb0,0xb0,0xff,0x80)},  75.0f, {RGBA(0xb0,0xb0,0xff,0x40)}, 100.0f },
	// LT_TECHLAMP2
//    { LIGHT_SPR,    0,  33, 0x80ffb0b0,  75, 0x40ffb0b0, 100 },
//    { LIGHT_SPR,    0,26, {RGBA(0xb0,0xb0,0xff,0x80)},  60.0f, {RGBA(0xb0,0xb0,0xff,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_lamp,    0,26, {RGBA(0xb0,0xb0,0xff,0x80)},  60.0f, {RGBA(0xb0,0xb0,0xff,0x40)}, 100.0f },
	// LT_COLUMN
//    { LIGHT_SPR,    3,  19, 0x80b0f0f0,  60, 0x40b0f0f0, 100 },
//    { LIGHT_SPR,    3,19, {RGBA(0xf0,0xf0,0xb0,0x80)},  60.0f, {RGBA(0xf0,0xf0,0xb0,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_lamp,    3,19, {RGBA(0xf0,0xf0,0xb0,0x80)},  60.0f, {RGBA(0xf0,0xf0,0xb0,0x40)}, 100.0f },
	// LT_CANDLE
//    { LIGHT_SPR,    0,   6, 0x60b0f0f0,  20, 0x30b0f0f0,  30 },
//    { LIGHT_SPR,    0, 6, {RGBA(0xf0,0xf0,0xb0,0x60)},  20.0f, {RGBA(0xf0,0xf0,0xb0,0x30)},  30.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0, 6, {RGBA(0xf0,0xf0,0xb0,0x60)},  20.0f, {RGBA(0xf0,0xf0,0xb0,0x30)},  30.0f },
	// LT_CANDLEABRE
//    { LIGHT_SPR,    0,  30, 0x60b0f0f0,  60, 0x30b0f0f0, 100 },
//    { LIGHT_SPR,    0,30, {RGBA(0xf0,0xf0,0xb0,0x60)},  60.0f, {RGBA(0xf0,0xf0,0xb0,0x30)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,    0,30, {RGBA(0xf0,0xf0,0xb0,0x60)},  60.0f, {RGBA(0xf0,0xf0,0xb0,0x30)}, 100.0f },

	// monsters
	// LT_REDBALL
//    { DYNLIGHT_SPR,   0,   0, 0x606060f0,   0, 0x302070ff, 100 },
	{ DYNLIGHT_SPR,   0, {RGBA(0xf0,0x60,0x60,0x60)},   0.0f, {RGBA(0xff,0x70,0x20,0x30)}, 100.0f },
	// LT_GREENBALL
//    { DYNLIGHT_SPR,   0,   0, 0x6077f777, 120, 0x3060f060, 100 },
	{ DYNLIGHT_SPR,   0, {RGBA(0x77,0xf7,0x77,0x60)}, 120, {RGBA(0x60,0xf0,0x60,0x30)}, 100.0f },
	// LT_ROCKET2
//    { DYNLIGHT_SPR,   0,   0, 0x606060f0,  20, 0x4020f7f7, 120 },
	{ DYNLIGHT_SPR,   0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x40)}, 120.0f },

	// weapons
	// LT_FX03
//    { DYNLIGHT_SPR,   0,   0, 0x6077ff50,  24, 0x2077f760,  80 },
	{ DYNLIGHT_SPR,   0, {RGBA(0x50,0xff,0x77,0x60)},  24.0f, {RGBA(0x60,0xf7,0x77,0x20)},  80.0f },
	// LT_FX17
//    { DYNLIGHT_SPR,   0,   0, 0x60ff7750,  24, 0x40f77760,  80 },
	{ DYNLIGHT_SPR,   0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x40)},  80.0f },
	// LT_FX00
//    { DYNLIGHT_SPR,   0,   0, 0x602020ff,  24, 0x302020f7,  80 },
	{ DYNLIGHT_SPR,   0, {RGBA(0xff,0x20,0x20,0x60)},  24.0f, {RGBA(0xf7,0x20,0x20,0x30)},  80.0f },
	// LT_FX08
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x4020c0f7, 120 },
	{ ROCKET_SPR,    0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xc0,0x20,0x40)}, 120.0f },
	// LT_FX04
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x2020c0f7, 120 },
	{ ROCKET_SPR,    0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xc0,0x20,0x20)}, 120.0f },
	// LT_FX02
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x1720f7f7, 120 },
	{ ROCKET_SPR,    0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x17)}, 120.0f },

	//lights
	// LT_WTRH
//    { DYNLIGHT_SPR,   0,  68, 0x606060f0,  60, 0x4020a0f7, 100 },
	{ DYNLIGHT_SPR,   0,68, {RGBA(0xf0,0x60,0x60,0x60)},  60.0f, {RGBA(0xf7,0xa0,0x20,0x40)}, 100.0f },
	// LT_SRTC
//    { DYNLIGHT_SPR,   0,  27, 0x606060f0,  60, 0x4020a0f7, 100 },
	{ DYNLIGHT_SPR,   0,27, {RGBA(0xf0,0x60,0x60,0x60)},  60.0f, {RGBA(0xf7,0xa0,0x20,0x40)}, 100.0f },
	// LT_CHDL
//    { DYNLIGHT_SPR,   0,  -8, 0x606060f0,  60, 0x502070f7, 100 },
	{ DYNLIGHT_SPR,   0,-8, {RGBA(0xf0,0x60,0x60,0x60)},  60.0f, {RGBA(0xf7,0x70,0x20,0x50)}, 100.0f },
	// LT_KFR1
//    { DYNLIGHT_SPR,   0,  27, 0x606060f0,  60, 0x4020a0f7, 100 },
//    { DYNLIGHT_SPR,   0,27, {RGBA(0xf0,0x60,0x60,0x60)},  60.0f, {RGBA(0xf7,0xa0,0x20,0x40)}, 100.0f },
	{ DYNLIGHT_SPR|CORONA_SPR|SPLT_fire,   0,27, {RGBA(0xf0,0x60,0x60,0x60)},  60.0f, {RGBA(0xf7,0xa0,0x20,0x40)}, 100.0f },*/
};

// sprite light indirection
// Indexed according to lightspritenum_s
light_t *t_lspr[NUMSPRITES] =
{
	&lspr[NOLIGHT],				// SPR_NULL - invisible object
	&lspr[NOLIGHT],				// SPR_UNKN

	&lspr[GENERIC_MOBJLIGHT_L],	// SPR_THOK - Thok! mobj
	&lspr[PLAYER_L],			// SPR_PLAY

	// Enemies
	&lspr[NOLIGHT],				// SPR_POSS - Crawla (Blue)
	&lspr[NOLIGHT],				// SPR_SPOS - Crawla (Red)
	&lspr[NOLIGHT],				// SPR_FISH - SDURF
	&lspr[NOLIGHT],				// SPR_BUZZ - Buzz (Gold)
	&lspr[NOLIGHT],				// SPR_RBUZ - Buzz (Red)
	&lspr[NOLIGHT],				// SPR_JETB - Jetty-Syn Bomber
	&lspr[NOLIGHT],				// SPR_JETG - Jetty-Syn Gunner
	&lspr[NOLIGHT],				// SPR_CCOM - Crawla Commander
	&lspr[NOLIGHT],				// SPR_DETN - Deton
	&lspr[NOLIGHT],				// SPR_SKIM - Skim mine dropper
	&lspr[NOLIGHT],				// SPR_TRET - Industrial Turret
	&lspr[NOLIGHT],				// SPR_TURR - Pop-Up Turret
	&lspr[NOLIGHT],				// SPR_SHRP - Sharp
	&lspr[NOLIGHT],				// SPR_CRAB - Crushstacean
	&lspr[NOLIGHT],				// SPR_CR2B - Banpyura
	&lspr[NOLIGHT],				// SPR_CSPR - Banpyura spring
	&lspr[NOLIGHT],				// SPR_JJAW - Jet Jaw
	&lspr[NOLIGHT],				// SPR_SNLR - Snailer
	&lspr[NOLIGHT],				// SPR_VLTR - BASH
	&lspr[NOLIGHT],				// SPR_PNTY - Pointy
	&lspr[NOLIGHT],				// SPR_ARCH - Robo-Hood
	&lspr[NOLIGHT],				// SPR_CBFS - Castlebot Facestabber
	&lspr[JETLIGHT_L],			// SPR_STAB - Castlebot Facestabber spear aura
	&lspr[NOLIGHT],				// SPR_SPSH - Egg Guard
	&lspr[NOLIGHT],				// SPR_ESHI - Egg Guard's shield
	&lspr[NOLIGHT],				// SPR_GSNP - Green Snapper
	&lspr[NOLIGHT],	 			// SPR_GSNL - Green Snapper leg
	&lspr[NOLIGHT],				// SPR_GSNH - Green Snapper head
	&lspr[NOLIGHT],				// SPR_MNUS - Minus
	&lspr[NOLIGHT],				// SPR_MNUD - Minus dirt
	&lspr[NOLIGHT],				// SPR_SSHL - Spring Shell
	&lspr[NOLIGHT],				// SPR_UNID - Unidus
	&lspr[NOLIGHT],				// SPR_CANA - Canarivore
	&lspr[NOLIGHT],				// SPR_CANG - Canarivore gas
	&lspr[REDBALL_L],			// SPR_PYRE - Pyre Fly
	&lspr[REDBALL_L],			// SPR_PTER - Pterabyte
	&lspr[REDBALL_L],			// SPR_DRAB - Dragonbomber

	// Generic Boss Items
	&lspr[GENERIC_MOBJLIGHT_L],	// GENERIC_MOBJLIGHT_L	// JETLIGHT_L 	// SPR_JETF - Boss jet fumes

	// Boss 1 (Greenflower)
	&lspr[NOLIGHT],				// SPR_EGGM - Boss 1
	&lspr[NOLIGHT],				// SPR_EGLZ - Boss 1 Junk

	// Boss 2 (Techno Hill)
	&lspr[NOLIGHT],				// SPR_EGGN - Boss 2
	&lspr[NOLIGHT],				// SPR_TANK - Boss 2 Junk
	&lspr[GOOPLIGHT_L],			// SPR_GOOP - Boss 2 Goop

	// Boss 3 (Deep Sea)
	&lspr[NOLIGHT], 			// SPR_EGGO - Boss 3
	&lspr[NOLIGHT], 			// SPR_SEBH - Boss 3 Junk
	&lspr[NOLIGHT], 			// SPR_FAKE - Boss 3 Fakemobile
	&lspr[LBLUESHINE_L],		// SPR_SHCK - Boss 3 Shockwave

	// Boss 4 (Castle Eggman)
	&lspr[NOLIGHT],				// SPR_EGGP
	&lspr[REDBALL_L],			// SPR_EFIR - Boss 4 jet flame
	&lspr[NOLIGHT],				// SPR_EGR1 - Boss 4 Spectator Eggrobo

	// Boss 5 (Arid Canyon)
	&lspr[PLAYER_L],			// SPR_FANG - replaces EGGQ
	&lspr[NOLIGHT],				// SPR_BRKN
	&lspr[NOLIGHT],				// SPR_WHAT
	&lspr[INVINCIBLE_L],		// SPR_VWRE
	&lspr[INVINCIBLE_L],		// SPR_PROJ - projector light
	&lspr[REDBALL_L],			// SPR_FBOM
	&lspr[NOLIGHT],				// SPR_FSGN
	&lspr[REDSHINE_L],			// SPR_BARX - bomb explosion (also used by barrel)
	&lspr[NOLIGHT],				// SPR_BARD - bomb dust (also used by barrel)

	// Boss 6 (Red Volcano)
	&lspr[NOLIGHT],				// SPR_EEGR

	// Boss 7 (Dark City)
	&lspr[NOLIGHT],				// SPR_BRAK
	&lspr[GOOPLIGHT_L],			// SPR_BGOO - Goop
	&lspr[NOLIGHT],				// SPR_BMSL

	// Boss 8 (Egg Rock)
	&lspr[NOLIGHT],				// SPR_EGGT

	// Cy-Brak-Demon; uses SPR_BRAK as well, but has some extras
	&lspr[ROCKET_L],			// SPR_RCKT - Rockets!
	&lspr[LBLUESHINE_L],		// SPR_ELEC - Electricity!
	&lspr[NOLIGHT],				// SPR_TARG - Targeting reticules!
	&lspr[ROCKET_L],			// SPR_NPLM - Big napalm bombs!
	&lspr[ROCKET_L],			// SPR_MNPL - Mini napalm bombs!

	// Metal Sonic
	&lspr[PLAYER_L],			// SPR_METL
	&lspr[PINKSHINE_L],			// SPR_MSCF
	&lspr[PINKSHINE_L],			// SPR_MSCB

	// Collectible Items
	&lspr[RINGLIGHT_L],			// SPR_RING
	&lspr[GENERIC_MOBJLIGHT_L],	// SPR_TRNG - Team Rings
	&lspr[ORANGESHINE_L], 		// SPR_TOKE - Special Stage Token
	&lspr[REDBALL_L], 			// SPR_RFLG - Red CTF Flag
	&lspr[BLUEBALL_L],			// SPR_BFLG - Blue CTF Flag
	&lspr[NOLIGHT],	// BLUEBALL_L // NOLIGHT			// SPR_SPHR - Sphere
	&lspr[RINGLIGHT_L],			// SPR_NCHP - NiGHTS chip
	&lspr[RINGLIGHT_L],			// SPR_NSTR - NiGHTS star
	&lspr[GENERIC_MOBJLIGHT_L],	// SPR_EMBM - Emblem
	&lspr[EMERALD_L],			// SPR_CEMG - Chaos Emeralds
	&lspr[GREENSHINE_L],		// SPR_SHRD - Emerald Hunt

	// Interactive Objects
	&lspr[BLUEBALL_L],			// SPR_BBLS - water bubble source
	&lspr[NOLIGHT],				// SPR_SIGN - Level end sign
	&lspr[NOLIGHT],				// SPR_SPIK - Spike Ball
	&lspr[REDSHINE_L],			// SPR_SFLM - Spin fire
	&lspr[GENERIC_MOBJLIGHT_L], // SPR_TFLM - Spin fire (team)
	&lspr[NOLIGHT],				// SPR_USPK - Floor spike
	&lspr[NOLIGHT],				// SPR_WSPK - Wall spike
	&lspr[NOLIGHT],				// SPR_WSPB - Wall spike base
	&lspr[STREETLIGHT_L],		// SPR_STPT - Starpost
	&lspr[SUPERSPARK_L],		// SPR_BMNE - Big floating mine
	&lspr[NOLIGHT],				// SPR_PUMI - Rollout Rock

	// Monitor Boxes
	&lspr[NOLIGHT],				// SPR_MSTV - MiSc TV sprites
	&lspr[NOLIGHT],				// SPR_XLTV - eXtra Large TV sprites

	&lspr[NOLIGHT],				// SPR_TRRI - Red team:  10 RIngs
	&lspr[NOLIGHT],				// SPR_TBRI - Blue team: 10 RIngs

	&lspr[NOLIGHT],				// SPR_TVRI - 10 RIng
	&lspr[NOLIGHT],				// SPR_TVPI - PIty shield
	&lspr[NOLIGHT],				// SPR_TVAT - ATtraction shield
	&lspr[NOLIGHT],				// SPR_TVFO - FOrce shield
	&lspr[NOLIGHT],				// SPR_TVAR - ARmageddon shield
	&lspr[NOLIGHT],				// SPR_TVWW - WhirlWind shield
	&lspr[NOLIGHT],				// SPR_TVEL - ELemental shield
	&lspr[NOLIGHT],				// SPR_TVSS - Super Sneakers
	&lspr[NOLIGHT],				// SPR_TVIV - InVincibility
	&lspr[NOLIGHT],				// SPR_TV1U - 1Up
	&lspr[NOLIGHT],				// SPR_TV1P - 1uP (textless)
	&lspr[NOLIGHT],				// SPR_TVEG - EGgman
	&lspr[NOLIGHT],				// SPR_TVMX - MiXup
	&lspr[NOLIGHT],				// SPR_TVMY - MYstery
	&lspr[NOLIGHT],				// SPR_TVGV - GraVity boots
	&lspr[NOLIGHT],			 	// SPR_TVRC - ReCycler
	&lspr[NOLIGHT],				// SPR_TV1K - 1,000 points  (1 K)
	&lspr[NOLIGHT],				// SPR_TVTK - 10,000 points (Ten K)
	&lspr[NOLIGHT],				// SPR_TVFL - FLame shield
	&lspr[NOLIGHT],				// SPR_TVBB - BuBble shield
	&lspr[NOLIGHT],				// SPR_TVZP - Thunder shield (ZaP)

	// Projectiles
	&lspr[NOLIGHT],				// SPR_MISL
#if 0
	&lspr[SMALLREDBALL_L],		// SPR_LASR - GFZ3 laser
	&lspr[REDSHINE_L],			// SPR_LASF - GFZ3 laser flames
#else
	&lspr[NOLIGHT],				// SPR_LASR - GFZ3 laser
	&lspr[SMALLREDBALL_L],		// SPR_LASF - GFZ3 laser flames
#endif
	&lspr[NOLIGHT],				// SPR_TORP - Torpedo
	&lspr[BLUESHINE_L],			// SPR_ENRG - Energy ball
	&lspr[NOLIGHT],				// SPR_MINE - Skim mine
	&lspr[NOLIGHT],				// SPR_JBUL - Jetty-Syn Bullet
	&lspr[SMALLREDBALL_L],		// SPR_TRLS
	&lspr[NOLIGHT],				// SPR_CBLL - Cannonball
	&lspr[NOLIGHT],				// SPR_AROW - Arrow
	&lspr[NOLIGHT],				// SPR_CFIR - Colored fire of various sorts

	// The letter
	&lspr[NOLIGHT],				// SPR_LETR

	// Tutorial scenery
	&lspr[NOLIGHT],				// SPR_TUPL
	&lspr[NOLIGHT],				// SPR_TUPF

	// Greenflower Scenery
	&lspr[NOLIGHT],				// SPR_FWR1
	&lspr[NOLIGHT],				// SPR_FWR2 - GFZ Sunflower
	&lspr[NOLIGHT],				// SPR_FWR3 - GFZ budding flower
	&lspr[NOLIGHT],				// SPR_FWR4
	&lspr[NOLIGHT],				// SPR_BUS1 -GFZ Bush w/ berries
	&lspr[NOLIGHT],				// SPR_BUS2 - GFZ Bush w/o berries
	&lspr[NOLIGHT],				// SPR_BUS3 - GFZ Bush w/ BLUE berries
	// Trees (both GFZ and misc)
	&lspr[NOLIGHT],				// SPR_TRE1 - GFZ
	&lspr[NOLIGHT],				// SPR_TRE2 - Checker
	&lspr[NOLIGHT],				// SPR_TRE3 - Frozen Hillside
	&lspr[NOLIGHT],				// SPR_TRE4 - Polygon
	&lspr[NOLIGHT],				// SPR_TRE5 - Bush tree
	&lspr[NOLIGHT],				// SPR_TRE6 - Spring tree

	// Techno Hill Scenery
	&lspr[NOLIGHT],				// SPR_FWR5 - THZ1 Steam Flower
	&lspr[NOLIGHT],				// SPR_THZP - THZ1 Spin flower (red)
	&lspr[NOLIGHT],				// SPR_FWR6 - THZ1 Spin flower (yellow)
	&lspr[NOLIGHT],				// SPR_THZT - Steam Whistle tree/bush
	&lspr[REDBALL_L],			// SPR_ALRM - THZ2 Alarm

	// Deep Sea Scenery
	&lspr[NOLIGHT],				// SPR_GARG - Deep Sea Gargoyle
	&lspr[NOLIGHT],				// SPR_SEWE - Deep Sea Seaweed
	&lspr[NOLIGHT],				// SPR_DRIP - Dripping water
	&lspr[NOLIGHT],				// SPR_CORL - Coral
	&lspr[BLUESHINE_L],			// SPR_BCRY - Blue Crystal
	&lspr[NOLIGHT],				// SPR_KELP - Kelp
	&lspr[NOLIGHT],				// SPR_ALGA - Animated algae top
	&lspr[NOLIGHT],				// SPR_ALGB - Animated algae segment
	&lspr[NOLIGHT],				// SPR_DSTG - DSZ Stalagmites
	&lspr[LBLUESHINE_L],		// SPR_LIBE - DSZ Light beam

	// Castle Eggman Scenery
	&lspr[NOLIGHT],				// SPR_CHAN - CEZ Chain
	&lspr[REDBALL_L],			// SPR_FLAM - Flame
	&lspr[NOLIGHT],				// SPR_ESTA - Eggman esta una estatua!
	&lspr[NOLIGHT],				// SPR_SMCH - Small Mace Chain
	&lspr[NOLIGHT],				// SPR_BMCH - Big Mace Chain
	&lspr[NOLIGHT],				// SPR_SMCE - Small Mace
	&lspr[NOLIGHT],				// SPR_BMCE - Big Mace
	&lspr[NOLIGHT],				// SPR_BSPB
	&lspr[NOLIGHT],				// SPR_YSPB - Yellow spring on a ball
	&lspr[NOLIGHT],				// SPR_RSPB - Red spring on a ball
	&lspr[REDBALL_L],			// SPR_SFBR - Small Firebar
	&lspr[REDBALL_L],			// SPR_BFBR - Big Firebar
	&lspr[NOLIGHT],				// SPR_BANR - Banner/pole
	&lspr[NOLIGHT],				// SPR_PINE - Pine Tree
	&lspr[NOLIGHT],				// SPR_CEZB - Bush
	&lspr[REDBALL_L],			// SPR_CNDL - Candle/pricket
	&lspr[NOLIGHT],				// SPR_FLMH - Flame holder
	&lspr[REDBALL_L],			// SPR_CTRC - Fire torch
	&lspr[NOLIGHT],				// SPR_CFLG - Waving flag/segment
	&lspr[NOLIGHT],				// SPR_CSTA - Crawla statue
	&lspr[NOLIGHT],				// SPR_CBBS - Facestabber statue
	&lspr[NOLIGHT],				// SPR_CABR - Brambles

	// Arid Canyon Scenery
	&lspr[NOLIGHT],				// SPR_BTBL - Big tumbleweed
	&lspr[NOLIGHT],				// SPR_STBL - Small tumbleweed
	&lspr[NOLIGHT],				// SPR_CACT - Cacti
	&lspr[NOLIGHT],				// SPR_WWSG - Caution Sign
	&lspr[NOLIGHT],				// SPR_WWS2 - Cacti Sign
	&lspr[NOLIGHT],				// SPR_WWS3 - Sharp Turn Sign
	&lspr[SMALLREDBALL_L],		// SPR_OILL - Oil lamp
	&lspr[ORANGESHINE_L],		// SPR_OILF - Oil lamp flare
	&lspr[NOLIGHT],				// SPR_BARR - TNT barrel
	&lspr[NOLIGHT],				// SPR_REMT - TNT proximity shell
	&lspr[NOLIGHT],				// SPR_TAZD - Dust devil
	&lspr[NOLIGHT],				// SPR_ADST - Arid dust
	&lspr[NOLIGHT],				// SPR_MCRT - Minecart
	&lspr[REDBALL_L],			// SPR_MCSP - Minecart spark
	&lspr[NOLIGHT],				// SPR_SALD - Saloon door
	&lspr[NOLIGHT],				// SPR_TRAE - Train cameo locomotive
	&lspr[NOLIGHT],				// SPR_TRAI - Train cameo wagon
	&lspr[NOLIGHT],				// SPR_STEA - Train steam

	// Red Volcano Scenery
	&lspr[REDBALL_L],			// SPR_FLME - Flame jet
	&lspr[REDBALL_L],			// SPR_DFLM - Blade's flame
	&lspr[REDBALL_L],			// SPR_LFAL - Lavafall
	&lspr[NOLIGHT],				// SPR_JPLA - Jungle palm
	&lspr[REDBALL_L],			// SPR_TFLO - Torch flower
	&lspr[NOLIGHT],				// SPR_WVIN - Wall vines

	// Dark City Scenery

	// Egg Rock Scenery

	// Christmas Scenery
	&lspr[NOLIGHT],				// SPR_XMS1 - Christmas Pole
	&lspr[NOLIGHT],				// SPR_XMS2 - Candy Cane
	&lspr[NOLIGHT],				// SPR_XMS3 - Snowman
	&lspr[STREETLIGHT_L],		// SPR_XMS4 - Lamppost
	&lspr[NOLIGHT],				// SPR_XMS5 - Hanging Star
	&lspr[NOLIGHT],				// SPR_XMS6 - Mistletoe
	&lspr[NOLIGHT],				// SPR_SNTT
	&lspr[NOLIGHT],				// SPR_SSTT
	&lspr[NOLIGHT],				// SPR_FHZI - FHZ Ice
	&lspr[NOLIGHT],				// SPR_ROSY

	// Halloween Scenery
	&lspr[NOLIGHT],				// SPR_PUMK - Pumpkins
	&lspr[NOLIGHT],				// SPR_HHPL - Dr Seuss Trees
	&lspr[NOLIGHT],				// SPR_SHRM - Mushroom
	&lspr[NOLIGHT],				// SPR_HHZM - Misc

	// Azure Temple Scenery
	&lspr[NOLIGHT],				// SPR_BGAR - ATZ Gargoyles
	&lspr[REDBALL_L],			// SPR_RCRY - ATZ Red Crystal (Target)
	&lspr[GREENBALL_L],			// SPR_CFLM - Green torch flame

	// Botanic Serenity Scenery
	&lspr[NOLIGHT],				// SPR_BSZ1 - Tall flowers
	&lspr[NOLIGHT],				// SPR_BSZ2 - Medium flowers
	&lspr[NOLIGHT],				// SPR_BSZ3 - Small flowers
	//&lspr[NOLIGHT],			// SPR_BSZ4 - -- Tulips
	&lspr[NOLIGHT],				// SPR_BST1 - Red tulip
	&lspr[NOLIGHT],				// SPR_BST2 - Purple tulip
	&lspr[NOLIGHT],				// SPR_BST3 - Blue tulip
	&lspr[NOLIGHT],				// SPR_BST4 - Cyan tulip
	&lspr[NOLIGHT],				// SPR_BST5 - Yellow tulip
	&lspr[NOLIGHT],				// SPR_BST6 - Orange tulip
	&lspr[NOLIGHT],				// SPR_BSZ5 - Cluster of Tulips
	&lspr[NOLIGHT],				// SPR_BSZ6 - Bush
	&lspr[NOLIGHT],				// SPR_BSZ7 - Vine
	&lspr[NOLIGHT],				// SPR_BSZ8 - Misc things

	// Misc Scenery
	&lspr[NOLIGHT],				// SPR_STLG - Stalagmites
	&lspr[STREETLIGHT_L],		// SPR_DBAL - Disco
	&lspr[NOLIGHT],				// SPR_GINE
	&lspr[NOLIGHT],				// SPR_PPAL

	// Powerup Indicators
	&lspr[REDSHIELD_L],			// SPR_ARMA - Armageddon Shield Orb
	&lspr[BLACKSHIELD_L],		// SPR_ARMF - Armageddon Shield Ring, Front
	&lspr[BLACKSHIELD_L],		// SPR_ARMB - Armageddon Shield Ring, Back
	&lspr[WHITESHIELD_L],		// SPR_WIND - Whirlwind Shield Orb
	&lspr[YELLOWSHIELD_L],		// SPR_MAGN - Attract Shield Orb
	&lspr[ORANGESHINE_L], // ORANGESHIELD_L // ORANGESHINE_L			// SPR_ELEM - Elemental Shield Orb
	&lspr[BLUESHIELD_L],		// SPR_FORC - Force Shield Orb
	&lspr[GREENSHIELD_L],		// SPR_PITY - Pity Shield Orb
	&lspr[ORANGESHINE_L], // ORANGESHIELD_L	// ORANGESHINE_L		// SPR_FIRS - Flame Shield Orb
	&lspr[LBLUESHINE_L],		// SPR_BUBS - Bubble Shield Orb
	&lspr[BLUESHIELD_L],		// SPR_ZAPS - Thunder Shield Orb
	&lspr[INVINCIBLE_L],		// SPR_IVSP - invincibility sparkles
	&lspr[SUPERSPARK_L],		// SPR_SSPK - Super Sonic Spark

	&lspr[NIGHTSLIGHT_L],		// SPR_GOAL - Special Stage goal (here because lol NiGHTS)

	// Flickies
	&lspr[NOLIGHT],				// SPR_FBUB - Flicky-sized bubble
	&lspr[NOLIGHT],				// SPR_FL01 - Bluebird
	&lspr[NOLIGHT],				// SPR_FL02 - Rabbit
	&lspr[NOLIGHT],				// SPR_FL03 - Chicken
	&lspr[NOLIGHT],				// SPR_FL04 - Seal
	&lspr[NOLIGHT],				// SPR_FL05 - Pig
	&lspr[NOLIGHT],				// SPR_FL06 - Chipmunk
	&lspr[NOLIGHT],				// SPR_FL07 - Penguin
	&lspr[NOLIGHT],				// SPR_FL08 - Fish
	&lspr[NOLIGHT],				// SPR_FL09 - Ram
	&lspr[NOLIGHT],				// SPR_FL10 - Puffin
	&lspr[NOLIGHT],				// SPR_FL11 - Cow
	&lspr[NOLIGHT],				// SPR_FL12 - Rat
	&lspr[NOLIGHT],				// SPR_FL13 - Bear
	&lspr[NOLIGHT],				// SPR_FL14 - Dove
	&lspr[NOLIGHT],				// SPR_FL15 - Cat
	&lspr[NOLIGHT],				// SPR_FL16 - Canary
	&lspr[NOLIGHT],				// SPR_FS01 - Spider
	&lspr[NOLIGHT],				// SPR_FS02 - Bat

	// Springs
	&lspr[NOLIGHT],				// SPR_FANS - Fan
	&lspr[NOLIGHT],				// SPR_STEM - Steam riser
	&lspr[NOLIGHT],				// SPR_BUMP - Bumpers
	&lspr[NOLIGHT],				// SPR_BLON - Balloons
	&lspr[NOLIGHT],				// SPR_SPRY - Yellow spring
	&lspr[NOLIGHT],				// SPR_SPRR - Red spring
	&lspr[NOLIGHT],				// SPR_SPRB - Blue spring
	&lspr[NOLIGHT],				// SPR_YSPR - Yellow Diagonal Spring
	&lspr[NOLIGHT],				// SPR_RSPR - Red Diagonal Spring
	&lspr[NOLIGHT],				// SPR_BSPR - Blue Diagonal Spring
	&lspr[NOLIGHT],				// SPR_SSWY - Yellow Side Spring
	&lspr[NOLIGHT],				// SPR_SSWR - Red Side Spring
	&lspr[NOLIGHT],				// SPR_SSWB - Blue Side Spring
	&lspr[NOLIGHT],				// SPR_BSTY - Yellow Booster
	&lspr[NOLIGHT],				// SPR_BSTR - Red Booster

	// Environmental Effects
	&lspr[GREYSHINE_L],			// SPR_RAIN - Rain
	&lspr[GREYSHINE_L],			// SPR_SNO1 - Snowflake
	&lspr[GREYSHINE_L],			// SPR_SPLH - Water Splish
	&lspr[REDBALL_L],			// SPR_LSPL - Lava Splish
	&lspr[GREYSHINE_L],			// SPR_SPLA - Water Splash
	&lspr[NOLIGHT],				// SPR_SMOK
	&lspr[NOLIGHT],				// SPR_BUBL- Bubble
	&lspr[RINGLIGHT_L],			// SPR_WZAP
	&lspr[NOLIGHT],				// SPR_DUST - Spindash dust
	&lspr[NOLIGHT],				// SPR_FPRT - Spindash dust (flame)
	&lspr[SUPERSPARK_L],		// SPR_TFOG - Teleport Fog
	&lspr[NIGHTSLIGHT_L],		// SPR_SEED - Sonic CD flower seed
	&lspr[NOLIGHT],				// SPR_PRTL - Particle (for fans, etc.)

	// Game Indicators
	&lspr[NOLIGHT], 			// SPR_SCOR - Score logo
	&lspr[NOLIGHT], 			// SPR_DRWN - Drowning Timer
	&lspr[NOLIGHT], 			// SPR_FLII - AI flight indicator
	&lspr[NOLIGHT], 			// SPR_LCKN - Target
	&lspr[NOLIGHT], 			// SPR_TTAG - Tag Sign
	&lspr[NOLIGHT], 			// SPR_GFLG - Got Flag sign
	&lspr[NOLIGHT], 			// SPR_FNSF - Finish flag

	&lspr[NOLIGHT],				// SPR_CORK
	&lspr[NOLIGHT],				// SPR_LHRT

	// Ring Weapons
	&lspr[REDSHINE_L], 			// SPR_RRNG - Red Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGB - Bounce Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGR - Rail Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGI - Infinity Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGA - Automatic Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGE - Explosion Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGS - Scatter Ring
	&lspr[RINGLIGHT_L], 		// SPR_RNGG - Grenade Ring

	&lspr[RINGLIGHT_L],			// SPR_PIKB - Bounce Ring Pickup
	&lspr[RINGLIGHT_L],			// SPR_PIKR - Rail Ring Pickup
	&lspr[RINGLIGHT_L],			// SPR_PIKA - Automatic Ring Pickup
	&lspr[RINGLIGHT_L],			// SPR_PIKE - Explosion Ring Pickup
	&lspr[RINGLIGHT_L],			// SPR_PIKS - Scatter Ring Pickup
	&lspr[RINGLIGHT_L],			// SPR_PIKG - Grenade Ring Pickup

	&lspr[RINGLIGHT_L],			// SPR_TAUT - Thrown Automatic Ring
	&lspr[RINGLIGHT_L],			// SPR_TGRE - Thrown Grenade Ring
	&lspr[RINGLIGHT_L],			// SPR_TSCR - Thrown Scatter Ring

	// Mario-specific stuff
	&lspr[RINGLIGHT_L],			// SPR_COIN
	&lspr[NOLIGHT],				// SPR_CPRK
	&lspr[NOLIGHT],				// SPR_GOOM
	&lspr[NOLIGHT],				// SPR_BGOM
	&lspr[REDBALL_L],			// SPR_FFWR
	&lspr[SMALLREDBALL_L],		// SPR_FBLL
	&lspr[NOLIGHT],				// SPR_SHLL
	&lspr[REDBALL_L],			// SPR_PUMA
	&lspr[NOLIGHT],				// SPR_HAMM
	&lspr[NOLIGHT],				// SPR_KOOP
	&lspr[REDBALL_L],			// SPR_BFLM
	&lspr[NOLIGHT],				// SPR_MAXE
	&lspr[NOLIGHT],				// SPR_MUS1
	&lspr[NOLIGHT],				// SPR_MUS2
	&lspr[NOLIGHT],				// SPR_TOAD

	// NiGHTS Stuff
	&lspr[NIGHTSLIGHT_L],		// SPR_NDRN - NiGHTS drone
	&lspr[NOLIGHT],	//NIGHTSLIGHT_L	// SPR_NSPK - NiGHTS sparkle
	&lspr[NOLIGHT],				// SPR_NBMP - NiGHTS Bumper
	&lspr[NOLIGHT],				// SPR_HOOP - NiGHTS hoop sprite
	&lspr[NOLIGHT],				// SPR_HSCR - NiGHTS score sprite
	&lspr[NOLIGHT],				// SPR_NPRU - Nights Powerups
	&lspr[NOLIGHT],				// SPR_CAPS - Capsule thingy for NiGHTS
	&lspr[INVINCIBLE_L],		// SPR_IDYA - Ideya
	&lspr[NOLIGHT],				// SPR_NTPN - Nightopian
	&lspr[NOLIGHT],				// SPR_SHLP - Shleep

	// Secret badniks and hazards, shhhh
	&lspr[NOLIGHT],				// SPR_PENG
	&lspr[NOLIGHT],				// SPR_POPH,
	&lspr[NOLIGHT],				// SPR_HIVE
	&lspr[NOLIGHT],				// SPR_BUMB,
	&lspr[NOLIGHT],				// SPR_BBUZ
	&lspr[NOLIGHT],				// SPR_FMCE,
	&lspr[NOLIGHT],				// SPR_HMCE,
	&lspr[NOLIGHT],				// SPR_CACO,
	&lspr[BLUEBALL_L],			// SPR_BAL2,
	&lspr[NOLIGHT],				// SPR_SBOB,
	&lspr[BLUEBALL_L],			// SPR_SBFL,
	&lspr[BLUEBALL_L],			// SPR_SBSK,
	&lspr[NOLIGHT],				// SPR_BATT,

	// Debris
	&lspr[RINGSPARK_L],			// SPR_SPRK - Sparkle
	&lspr[NOLIGHT],				// SPR_BOM1 - Robot Explosion
	&lspr[SUPERSPARK_L],		// SPR_BOM2 - Boss Explosion 1
	&lspr[SUPERSPARK_L],		// SPR_BOM3 - Boss Explosion 2
	&lspr[NOLIGHT],				// SPR_BOM4 - Underwater Explosion
	&lspr[REDBALL_L],			// SPR_BMNB - Mine Explosion

	// Crumbly rocks
	&lspr[NOLIGHT],				// SPR_ROIA
	&lspr[NOLIGHT],				// SPR_ROIB
	&lspr[NOLIGHT],				// SPR_ROIC
	&lspr[NOLIGHT],				// SPR_ROID
	&lspr[NOLIGHT],				// SPR_ROIE
	&lspr[NOLIGHT],				// SPR_ROIF
	&lspr[NOLIGHT],				// SPR_ROIG
	&lspr[NOLIGHT],				// SPR_ROIH
	&lspr[NOLIGHT],				// SPR_ROII
	&lspr[NOLIGHT],				// SPR_ROIJ
	&lspr[NOLIGHT],				// SPR_ROIK
	&lspr[NOLIGHT],				// SPR_ROIL
	&lspr[NOLIGHT],				// SPR_ROIM
	&lspr[NOLIGHT],				// SPR_ROIN
	&lspr[NOLIGHT],				// SPR_ROIO
	&lspr[NOLIGHT],				// SPR_ROIP

	// Level debris
	&lspr[NOLIGHT],				// SPR_GFZD - GFZ debris
	&lspr[NOLIGHT],				// SPR_BRIC - Bricks
	&lspr[NOLIGHT],				// SPR_WDDB - Wood Debris
	&lspr[NOLIGHT],				// SPR_BRIR - CEZ3 colored bricks
	&lspr[NOLIGHT],				// SPR_BRIB
	&lspr[NOLIGHT],				// SPR_BRIY

	// Gravity Well Objects
	&lspr[NOLIGHT],				// SPR_GWLG
	&lspr[NOLIGHT],				// SPR_GWLR

	// LJ Knuckles
	&lspr[NOLIGHT],				// SPR_OLDK,

	// Free slots
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],
	&lspr[NOLIGHT],

	[SPR_EEGG] = &lspr[GREENSHINE_L],	// SPR_EEGG - Easter Eggs
	&lspr[REDSHINE_L],					// SPR_TF2D - Dispenser goin' up
};
