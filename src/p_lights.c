// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  p_lights.c
/// \brief Sector lighting effects
///        Fire flicker, light flash, strobe flash, lightning flash, glow, and fade.

#include "doomdef.h"
#include "p_local.h"
#include "r_state.h"
#include "z_zone.h"
#include "m_random.h"
#include "d_netcmd.h"

/** Removes any active lighting effects in a sector.
  *
  * \param sector The sector to remove effects from.
  */
void P_RemoveLighting(sector_t *sector)
{
	if (sector->lightingdata)
	{
		// The thinker is the first member in all the lighting action structs,
		// so just let the thinker get freed, and that will free the whole
		// structure.
		P_RemoveThinker(&((thinkerdata_t *)sector->lightingdata)->thinker);
		sector->lightingdata = NULL;
	}
}

// =========================================================================
//                           FIRELIGHT FLICKER
// =========================================================================

/** Thinker function for fire flicker.
  *
  * \param flick Action structure for this effect.
  * \sa P_SpawnAdjustableFireFlicker
  */
void T_FireFlicker(fireflicker_t *flick)
{
	INT16 amount;

	if (--flick->count)
		return;

	amount = (INT16)((UINT8)(P_RandomByte() & 3) * 16);

	if (flick->sector->lightlevel - amount < flick->minlight)
		flick->sector->lightlevel = flick->minlight;
	else
		flick->sector->lightlevel = flick->maxlight - amount;

	flick->count = flick->resetcount;
}

/** Spawns an adjustable fire flicker effect in a sector.
  *
  * \param sector    Target sector for the effect.
  * \param lighta    One of the two light levels to move between.
  * \param lightb    The other light level.
  * \param length    Four times the number of tics between flickers.
  * \sa T_FireFlicker
  */
fireflicker_t *P_SpawnAdjustableFireFlicker(sector_t *sector, INT16 lighta, INT16 lightb, INT32 length)
{
	fireflicker_t *flick;

	P_RemoveLighting(sector); // out with the old, in with the new
	flick = Z_Calloc(sizeof (*flick), PU_LEVSPEC, NULL);

	P_AddThinker(THINK_MAIN, &flick->thinker);

	flick->thinker.function.acp1 = (actionf_p1)T_FireFlicker;
	flick->sector = sector;
	flick->maxlight = max(lighta, lightb);
	flick->minlight = min(lighta, lightb);
	flick->count = flick->resetcount = length/4;
	sector->lightingdata = flick;

	// input bounds checking and stuff
	if (!flick->resetcount)
		flick->resetcount = 1;
	if (flick->minlight == flick->maxlight)
	{
		if (flick->minlight > 0)
			flick->minlight--;
		if (flick->maxlight < 255)
			flick->maxlight++;
	}

	// Make sure the starting light level is in range.
	sector->lightlevel = max(flick->minlight, min(flick->maxlight, sector->lightlevel));

	return flick;
}

//
// LIGHTNING FLASH EFFECT
//

/** Thinker function for a lightning flash storm effect.
  *
  * \param flash The effect being considered.
  * \sa P_SpawnLightningFlash
  */
void T_LightningFlash(lightflash_t *flash)
{
	flash->sector->lightlevel -= 4;

	if (flash->sector->lightlevel <= flash->minlight)
	{
		flash->sector->lightlevel = (INT16)flash->minlight;
		P_RemoveLighting(flash->sector);
	}
}

/** Spawns a one-time lightning flash.
  *
  * \param sector Sector to light up.
  * \sa T_LightningFlash
  */
void P_SpawnLightningFlash(sector_t *sector)
{
	INT32 minlight;
	lightflash_t *flash;

	minlight = sector->lightlevel;

	if (sector->lightingdata)
	{
		if (((lightflash_t *)sector->lightingdata)->thinker.function.acp1
			== (actionf_p1)T_LightningFlash)
		{
			// lightning was already flashing in this sector
			// save the original light level value
			minlight = ((lightflash_t *)sector->lightingdata)->minlight;
		}

		P_RemoveThinker(&((thinkerdata_t *)sector->lightingdata)->thinker);
	}

	sector->lightingdata = NULL;

	flash = Z_Calloc(sizeof (*flash), PU_LEVSPEC, NULL);

	P_AddThinker(THINK_MAIN, &flash->thinker);

	flash->thinker.function.acp1 = (actionf_p1)T_LightningFlash;
	flash->sector = sector;
	flash->maxlight = 255;
	flash->minlight = minlight;
	sector->lightlevel = (INT16)flash->maxlight;

	sector->lightingdata = flash;
}

//
// STROBE LIGHT FLASHING
//

/** Thinker function for strobe light flashing.
  *
  * \param flash The effect under consideration.
  * \sa P_SpawnAdjustableStrobeFlash
  */
void T_StrobeFlash(strobe_t *flash)
{
	if (--flash->count)
		return;

	if (flash->sector->lightlevel == flash->minlight)
	{
		flash->sector->lightlevel = flash->maxlight;
		flash->count = flash->brighttime;
	}
	else
	{
		flash->sector->lightlevel = flash->minlight;
		flash->count = flash->darktime;
	}
}

/** Spawns an adjustable strobe light effect in a sector.
  *
  * \param sector     Target sector for the effect.
  * \param lighta     One of the two light levels to move between.
  * \param lightb     The other light level.
  * \param darktime   Time in tics for the light to be dark.
  * \param brighttime Time in tics for the light to be bright.
  * \param inSync     If true, the effect will be kept in sync
  *                   with other effects of this type, provided
  *                   they use the same time values, and
  *                   provided this function is called at level
  *                   load. Otherwise, the starting state of
  *                   the strobe flash is random.
  * \sa T_StrobeFlash
  */
strobe_t *P_SpawnAdjustableStrobeFlash(sector_t *sector, INT16 lighta, INT16 lightb, INT32 darktime, INT32 brighttime, boolean inSync)
{
	strobe_t *flash;

	P_RemoveLighting(sector); // out with the old, in with the new
	flash = Z_Calloc(sizeof (*flash), PU_LEVSPEC, NULL);

	P_AddThinker(THINK_MAIN, &flash->thinker);

	flash->sector = sector;
	flash->darktime = darktime;
	flash->brighttime = brighttime;
	flash->thinker.function.acp1 = (actionf_p1)T_StrobeFlash;
	flash->maxlight = max(lighta, lightb);
	flash->minlight = min(lighta, lightb);

	if (flash->minlight == flash->maxlight)
		flash->minlight = 0;

	if (!inSync)
		flash->count = (P_RandomByte() & 7) + 1;
	else
		flash->count = 1;

	// Make sure the starting light level is in range.
	sector->lightlevel = max(flash->minlight, min(flash->maxlight, sector->lightlevel));

	sector->lightingdata = flash;
	return flash;
}

/** Thinker function for glowing light.
  *
  * \param g Action structure for this effect.
  * \sa P_SpawnAdjustableGlowingLight
  */
void T_Glow(glow_t *g)
{
	switch (g->direction)
	{
		case -1:
			// DOWN
			g->sector->lightlevel -= g->speed;
			if (g->sector->lightlevel <= g->minlight)
			{
				g->sector->lightlevel += g->speed;
				g->direction = 1;
			}
			break;

		case 1:
			// UP
			g->sector->lightlevel += g->speed;
			if (g->sector->lightlevel >= g->maxlight)
			{
				g->sector->lightlevel -= g->speed;
				g->direction = -1;
			}
			break;
	}
}

/** Spawns an adjustable glowing light effect in a sector.
  *
  * \param sector    Target sector for the effect.
  * \param lighta    One of the two light levels to move between.
  * \param lightb    The other light level.
  * \param length    The speed of the effect.
  * \sa T_Glow
  */
glow_t *P_SpawnAdjustableGlowingLight(sector_t *sector, INT16 lighta, INT16 lightb, INT32 length)
{
	glow_t *g;

	P_RemoveLighting(sector); // out with the old, in with the new
	g = Z_Calloc(sizeof (*g), PU_LEVSPEC, NULL);

	P_AddThinker(THINK_MAIN, &g->thinker);

	g->sector = sector;
	g->minlight = min(lighta, lightb);
	g->maxlight = max(lighta, lightb);
	g->thinker.function.acp1 = (actionf_p1)T_Glow;
	g->direction = 1;
	g->speed = (INT16)(length/4);
	if (g->speed > (g->maxlight - g->minlight)/2) // don't make it ridiculous speed
		g->speed = (g->maxlight - g->minlight)/2;

	while (g->speed < 1)
	{
		if (g->minlight > 0)
			g->minlight--;
		if (g->maxlight < 255)
			g->maxlight++;

		g->speed = (g->maxlight - g->minlight)/2;
	}

	// Make sure the starting light level is in range.
	sector->lightlevel = max(g->minlight, min(g->maxlight, sector->lightlevel));

	sector->lightingdata = g;

	return g;
}

/** Fades all the lights in specified sector to a new
  * value.
  *
  * \param sector    Target sector
  * \param destvalue The final light value in these sectors.
  * \param speed     If tic-based: total duration of effect.
  *                  If speed-based: Speed of the fade; the change to the ligh
  *                  level in each sector per tic.
  * \param ticbased  Use a specific duration for the fade, defined by speed
  * \sa T_LightFade
  */
void P_FadeLightBySector(sector_t *sector, INT32 destvalue, INT32 speed, boolean ticbased)
{
	lightlevel_t *ll;

	P_RemoveLighting(sector); // remove the old lighting effect first

	if ((ticbased && !speed) || sector->lightlevel == destvalue) // set immediately
	{
		sector->lightlevel = destvalue;
		return;
	}

	ll = Z_Calloc(sizeof (*ll), PU_LEVSPEC, NULL);
	ll->thinker.function.acp1 = (actionf_p1)T_LightFade;
	sector->lightingdata = ll; // set it to the lightlevel_t

	P_AddThinker(THINK_MAIN, &ll->thinker); // add thinker

	ll->sector = sector;
	ll->sourcelevel = sector->lightlevel;
	ll->destlevel = destvalue;

	ll->fixedcurlevel = sector->lightlevel<<FRACBITS;

	if (ticbased)
	{
		// Speed means duration.
		ll->timer = abs(speed);
		ll->fixedpertic = FixedDiv((destvalue<<FRACBITS) - ll->fixedcurlevel, speed<<FRACBITS);
	}
	else
	{
		// Speed means increment per tic (literally speed).
		ll->timer = abs(FixedDiv((destvalue<<FRACBITS) - ll->fixedcurlevel, speed<<FRACBITS)>>FRACBITS);
		ll->fixedpertic = ll->destlevel < ll->sourcelevel ? -speed<<FRACBITS : speed<<FRACBITS;
	}
}

void P_FadeLight(INT16 tag, INT32 destvalue, INT32 speed, boolean ticbased, boolean force, boolean relative)
{
	INT32 i;
	INT32 realdestvalue;

	// search all sectors for ones with tag
	TAG_ITER_SECTORS(tag, i)
	{
		if (!force && ticbased // always let speed fader execute
			&& sectors[i].lightingdata
			&& ((lightlevel_t*)sectors[i].lightingdata)->thinker.function.acp1 == (actionf_p1)T_LightFade)
			// && ((lightlevel_t*)sectors[i].lightingdata)->timer > 2)
		{
			CONS_Debug(DBG_GAMELOGIC, "Line type 420 Executor: Fade light thinker already exists, timer: %d\n", ((lightlevel_t*)sectors[i].lightingdata)->timer);
			continue;
		}

		realdestvalue = relative ? max(0, min(255, sectors[i].lightlevel + destvalue)) : destvalue;
		P_FadeLightBySector(&sectors[i], realdestvalue, speed, ticbased);
	}
}

/** Fades the light level in a sector to a new value.
  *
  * \param ll The fade effect under consideration.
  * \sa P_FadeLight
  */
void T_LightFade(lightlevel_t *ll)
{
	if (--ll->timer <= 0)
	{
		ll->sector->lightlevel = ll->destlevel; // set to dest lightlevel
		P_RemoveLighting(ll->sector); // clear lightingdata, remove thinker
		return;
	}

	ll->fixedcurlevel = ll->fixedcurlevel + ll->fixedpertic;
	ll->sector->lightlevel = (ll->fixedcurlevel)>>FRACBITS;
}

#ifdef ALAM_LIGHTING
// ---------------------------------------- //
//        			Coronas
// ---------------------------------------- //

boolean LCR_SuperSonicLight(mobj_t *mobj)
{
	if (!mobj || mobj->sprite != SPR_PLAY || (!mobj->player || !mobj->player->powers[pw_super]))
		return false;
	return true;
}

// Force light setup, without another test.
static void CV_corona_OnChange(void)
{
    for (int i = 0; i < NUMLIGHTS; i++)
        t_lspr[i]->impl_flags |= SLI_changed;
}

static CV_PossibleValue_t corona_cons_t[] = {{0, "Off"}, {1, "Special"}, {2, "Most"}, {14, "Dim"}, {15, "All"}, {16, "Bright"}, {20, "Old"}, {0, NULL}};
consvar_t cv_corona = CVAR_INIT ("corona", "All", CV_SAVE|CV_CALL, corona_cons_t, CV_corona_OnChange);
consvar_t cv_coronasize = CVAR_INIT ("coronasize", "1", CV_SAVE|CV_FLOAT, NULL, NULL);

static CV_PossibleValue_t corona_draw_mode_cons_t[] = {{0, "Blend"}, {1, "Blend_BG"}, {2, "Additive"}, {3, "Additive_BG"}, {4, "Add_Limit"}, {0, NULL}};
consvar_t cv_corona_draw_mode = CVAR_INIT ("corona_draw_mode", "2", CV_SAVE, corona_draw_mode_cons_t, NULL);

//Hurdler: now we can change those values via FS :)
// RGBA( r, g, b, a )
// Indexed by lightspritenum_s
light_t lspr[NUMLIGHTS] =
{	// type       		offset x,   y  		coronas color,	c_radius,	light color,	l_radius, 	sqr radius computed at init		coronaroutine
	// default
	{UNDEFINED_SPR,		0.0f,		0.0f, 	0x00000000,		60.0f,		0x00000000,		100.0f,		0.0f, 0,						NULL},

	// weapons
	// RINGSPARK_L
	{LIGHT_SPR,      	0.0f,   	0.0f, 	0x0000e0ff,  	16.0f, 		0x0000e0ff,  	 32.0f, 	0.0f, 0,						NULL}, // Tails 09-08-2002
	// SUPERSONIC_L
	{/*DYNLIGHT_SPR*/LIGHT_SPR,   	0.0f,   	0.0f, 	0xff00e0ff,  	32.0f, 		0xff00e0ff, 	128.0f, 	0.0f, 0,			LCR_SuperSonicLight}, // Tails 09-08-2002
	// SUPERSPARK_L
	{LIGHT_SPR,      	0.0f,   	0.0f, 	0xe0ffffff,   	 8.0f, 		0xe0ffffff,  	 64.0f, 	0.0f, 0,						NULL},
	// INVINCIBLE_L
	{DYNLIGHT_SPR,   	0.0f,   	0.0f, 	0x10ffaaaa,  	16.0f, 		0x10ffaaaa, 	128.0f, 	0.0f, 0,						NULL},
	// GREENSHIELD_L
	{DYNLIGHT_SPR,		0.0f,		0.0f,	0x602b7337,		32.0f,		0x602b7337,		128.0f,		0.0f, 0,						NULL},
	// BLUESHIELD_L
	{DYNLIGHT_SPR,		0.0f,		0.0f,	0x60cb0000,		32.0f,		0x60cb0000,		128.0f,		0.0f, 0,						NULL},

	// tall lights
	// YELLOWSHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x601f7baf,  32.0f, 0x601f7baf, 128.0f, 0.0f, 0,						NULL},

	// REDSHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x600000cb,  32.0f, 0x600000cb, 128.0f, 0.0f, 0,						NULL},

	// BLACKSHIELD_L // Black light? lol
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60010101,  32.0f, 0x60ff00ff, 128.0f, 0.0f, 0,						NULL},

	// WHITESHIELD_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60ffffff,  32.0f, 0x60ffffff, 128.0f, 0.0f, 0,						NULL},

	// SMALLREDBALL_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x606060f0,   0.0f, 0x302070ff,  32.0f, 0.0f, 0,						NULL},

	// small lights
	// RINGLIGHT_L
	{DYNLIGHT_SPR,   0.0f,   0.0f, 0x60b0f0f0,   0.0f, 0x30b0f0f0, 100.0f, 0.0f, 0,						NULL},
	// GREENSMALL_L
	{    LIGHT_SPR,   0.0f,  14.0f, 0x6070ff70,  60.0f, 0x4070ff70, 100.0f, 0.0f, 0,						NULL},
	// REDSMALL_L
	{    LIGHT_SPR,   0.0f,  14.0f, 0x705070ff,  60.0f, 0x405070ff, 100.0f, 0.0f, 0,						NULL},

	// type       offset x,   y  coronas color, c_size,light color,l_radius, sqr radius computed at init
	// GREENSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff00ff00,  64.0f, 0xff00ff00, 256.0f, 0.0f, 0,						NULL},
	// ORANGESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff0080ff,  64.0f, 0xff0080ff, 256.0f, 0.0f, 0,						NULL},
	// PINKSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffe080ff,  64.0f, 0xffe080ff, 256.0f, 0.0f, 0,						NULL},
	// BLUESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffff0000,  64.0f, 0xffff0000, 256.0f, 0.0f, 0,						NULL},
	// REDSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xff0000ff,  64.0f, 0xff0000ff, 256.0f, 0.0f, 0,						NULL},
	// LBLUESHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffff8080,  64.0f, 0xffff8080, 256.0f, 0.0f, 0,						NULL},
	// GREYSHINE_L
	{    LIGHT_SPR,   0.0f,   0.0f, 0xffe0e0e0,  64.0f, 0xffe0e0e0, 256.0f, 0.0f, 0,						NULL},

	// monsters
	// REDBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f, 0x606060ff,   0.0f, 0x606060ff, 100.0f, 0.0f, 0,						NULL},
	// GREENBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f,	0x6060ff60, 120.0f, 0x6060ff60, 100.0f, 0.0f, 0,						NULL},
	// BLUEBALL_L
	{DYNLIGHT_SPR,	0.0f,	0.0f,	0x60ff6060, 120.0f, 0x60ff6060, 100.0f, 0.0f, 0,						NULL},

	// misc.
	// NIGHTSLIGHT_L
	{    LIGHT_SPR,   0.0f,   6.0f, 0x60ffffff,  16.0f, 0x30ffffff,  32.0f, 0.0f, 0,						NULL},

	// JETLIGHT_L
	{DYNLIGHT_SPR,   0.0f,   6.0f, 0x60ffaaaa,  16.0f, 0x30ffaaaa,  64.0f, 0.0f, 0,						NULL},

	// GOOPLIGHT_L
	{DYNLIGHT_SPR,   0.0f,   6.0f, 0x60ff00ff,  16.0f, 0x30ff00ff,  32.0f, 0.0f, 0,						NULL},

	// STREETLIGHT_L
	{ LIGHT_SPR,      0.0f,   0.0f, 0xe0ffffff,  64.0f, 0xe0ffffff, 384.0f, 0.0f, 0,						NULL},


	// weapons
    // LT_PLASMA
//    { DYNLIGHT_SPR,  0.0f,   0.0f, 0x60ff7750,  24.0f, 0x20f77760,  80.0f },
    /*{ DYNLIGHT_SPR,  0, 0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x20)},  80.0f },
    // LT_PLASMAEXP
//    { DYNLIGHT_SPR,  0.0f,   0.0f, 0x60ff7750,  24.0f, 0x40f77760, 120.0f },
    { DYNLIGHT_SPR,  0, 0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x40)}, 120.0f },
    // LT_ROCKET
//    { ROCKET_SPR,   0,   0, 0x606060f0,  20, 0x4020f7f7, 120 },
    { ROCKET_SPR,   0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x40)}, 120.0f },
    // LT_ROCKETEXP
//    { DYNLIGHT_SPR,  0,   0, 0x606060f0,  20, 0x6020f7f7, 200 },
    { DYNLIGHT_SPR,  0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x60)}, 200.0f },
    // LT_BFG
//    { DYNLIGHT_SPR,  0,   0, 0x6077f777, 120, 0x8060f060, 200 },
    { DYNLIGHT_SPR,  0, 0, {RGBA(0x77,0xf7,0x77,0x60)}, 120.0f, {RGBA(0x60,0xf0,0x60,0x80)}, 200.0f },
    // LT_BFGEXP
//    { DYNLIGHT_SPR,  0,   0, 0x6077f777, 120, 0x6060f060, 400 },
    { DYNLIGHT_SPR,  0, 0, {RGBA(0x77,0xf7,0x77,0x60)}, 120.0f, {RGBA(0x60,0xf0,0x60,0x60)}, 400.0f },

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
    { DYNLIGHT_SPR,   0, 0, {RGBA(0xf0,0x60,0x60,0x60)},   0.0f, {RGBA(0xff,0x70,0x20,0x30)}, 100.0f },
    // LT_GREENBALL
//    { DYNLIGHT_SPR,   0,   0, 0x6077f777, 120, 0x3060f060, 100 },
    { DYNLIGHT_SPR,   0, 0, {RGBA(0x77,0xf7,0x77,0x60)}, 120, {RGBA(0x60,0xf0,0x60,0x30)}, 100.0f },
    // LT_ROCKET2
//    { DYNLIGHT_SPR,   0,   0, 0x606060f0,  20, 0x4020f7f7, 120 },
    { DYNLIGHT_SPR,   0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x40)}, 120.0f },

    // weapons
    // LT_FX03
//    { DYNLIGHT_SPR,   0,   0, 0x6077ff50,  24, 0x2077f760,  80 },
    { DYNLIGHT_SPR,   0, 0, {RGBA(0x50,0xff,0x77,0x60)},  24.0f, {RGBA(0x60,0xf7,0x77,0x20)},  80.0f },
    // LT_FX17
//    { DYNLIGHT_SPR,   0,   0, 0x60ff7750,  24, 0x40f77760,  80 },
    { DYNLIGHT_SPR,   0, 0, {RGBA(0x50,0x77,0xff,0x60)},  24.0f, {RGBA(0x60,0x77,0xf7,0x40)},  80.0f },
    // LT_FX00
//    { DYNLIGHT_SPR,   0,   0, 0x602020ff,  24, 0x302020f7,  80 },
    { DYNLIGHT_SPR,   0, 0, {RGBA(0xff,0x20,0x20,0x60)},  24.0f, {RGBA(0xf7,0x20,0x20,0x30)},  80.0f },
    // LT_FX08
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x4020c0f7, 120 },
    { ROCKET_SPR,    0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xc0,0x20,0x40)}, 120.0f },
    // LT_FX04
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x2020c0f7, 120 },
    { ROCKET_SPR,    0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xc0,0x20,0x20)}, 120.0f },
    // LT_FX02
//    { ROCKET_SPR,    0,   0, 0x606060f0,  20, 0x1720f7f7, 120 },
    { ROCKET_SPR,    0, 0, {RGBA(0xf0,0x60,0x60,0x60)},  20.0f, {RGBA(0xf7,0xf7,0x20,0x17)}, 120.0f },

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
	&lspr[NOLIGHT],     // SPR_NULL
	&lspr[NOLIGHT],     // SPR_UNKN

	&lspr[/*NOLIGHT*/STREETLIGHT_L],     // SPR_THOK
	&lspr[SUPERSONIC_L],// SPR_PLAY

	// Enemies
	&lspr[NOLIGHT],     // SPR_POSS
	&lspr[NOLIGHT],     // SPR_SPOS
	&lspr[NOLIGHT],     // SPR_FISH
	&lspr[NOLIGHT],     // SPR_BUZZ
	&lspr[NOLIGHT],     // SPR_RBUZ
	&lspr[NOLIGHT],     // SPR_JETB
	&lspr[NOLIGHT],     // SPR_JETG
	&lspr[NOLIGHT],     // SPR_CCOM
	&lspr[NOLIGHT],     // SPR_DETN
	&lspr[NOLIGHT],     // SPR_SKIM
	&lspr[NOLIGHT],     // SPR_TRET
	&lspr[NOLIGHT],     // SPR_TURR
	&lspr[NOLIGHT],     // SPR_SHRP
	&lspr[NOLIGHT],     // SPR_CRAB
	&lspr[NOLIGHT],     // SPR_CR2B
	&lspr[NOLIGHT],     // SPR_CSPR
	&lspr[NOLIGHT],     // SPR_JJAW
	&lspr[NOLIGHT],     // SPR_SNLR
	&lspr[NOLIGHT],     // SPR_VLTR
	&lspr[NOLIGHT],     // SPR_PNTY
	&lspr[NOLIGHT],     // SPR_ARCH
	&lspr[NOLIGHT],     // SPR_CBFS
	&lspr[/*JETLIGHT_L*/STREETLIGHT_L],  // SPR_STAB
	&lspr[NOLIGHT],     // SPR_SPSH
	&lspr[NOLIGHT],     // SPR_ESHI
	&lspr[NOLIGHT],     // SPR_GSNP
	&lspr[NOLIGHT],     // SPR_GSNL
	&lspr[NOLIGHT],     // SPR_GSNH
	&lspr[NOLIGHT],     // SPR_MNUS
	&lspr[NOLIGHT],     // SPR_MNUD
	&lspr[NOLIGHT],     // SPR_SSHL
	&lspr[NOLIGHT],     // SPR_UNID
	&lspr[NOLIGHT],     // SPR_CANA
	&lspr[NOLIGHT],     // SPR_CANG
	&lspr[NOLIGHT],     // SPR_PYRE
	&lspr[NOLIGHT],     // SPR_PTER
	&lspr[NOLIGHT],     // SPR_DRAB

	// Generic Boss Items
	&lspr[/*JETLIGHT_L*/STREETLIGHT_L],     // SPR_JETF // Boss jet fumes

	// Boss 1, (Greenflower)
	&lspr[NOLIGHT],     // SPR_EGGM
	&lspr[NOLIGHT],     // SPR_EGLZ

	// Boss 2, (Techno Hill)
	&lspr[NOLIGHT],     // SPR_EGGN
	&lspr[NOLIGHT],     // SPR_TANK
	&lspr[NOLIGHT],     // SPR_GOOP

	// Boss 3 (Deep Sea)
	&lspr[NOLIGHT],     // SPR_EGGO
	&lspr[NOLIGHT],     // SPR_SEBH
	&lspr[NOLIGHT],     // SPR_FAKE
	&lspr[LBLUESHINE_L],// SPR_SHCK

	// Boss 4 (Castle Eggman)
	&lspr[NOLIGHT],     // SPR_EGGP
	&lspr[REDBALL_L],   // SPR_EFIR
	&lspr[NOLIGHT],     // SPR_EGR1

	// Boss 5 (Arid Canyon)
	&lspr[NOLIGHT],      // SPR_FANG // replaces EGGQ
	&lspr[NOLIGHT],      // SPR_BRKN
	&lspr[NOLIGHT],      // SPR_WHAT
	&lspr[INVINCIBLE_L], // SPR_VWRE
	&lspr[INVINCIBLE_L], // SPR_PROJ
	&lspr[NOLIGHT],      // SPR_FBOM
	&lspr[NOLIGHT],      // SPR_FSGN
	&lspr[/*REDBALL_L*/REDSHINE_L],    // SPR_BARX // bomb explosion (also used by barrel)
	&lspr[NOLIGHT],      // SPR_BARD // bomb dust (also used by barrel)

	// Boss 6 (Red Volcano)
	&lspr[NOLIGHT],     // SPR_EEGR

	// Boss 7 (Dark City)
	&lspr[NOLIGHT],     // SPR_BRAK
	&lspr[NOLIGHT],     // SPR_BGOO
	&lspr[NOLIGHT],     // SPR_BMSL

	// Boss 8 (Egg Rock)
	&lspr[NOLIGHT],     // SPR_EGGT

	&lspr[NOLIGHT], //SPR_RCKT
	&lspr[NOLIGHT], //SPR_ELEC
	&lspr[NOLIGHT], //SPR_TARG
	&lspr[NOLIGHT], //SPR_NPLM
	&lspr[NOLIGHT], //SPR_MNPL

	// Metal Sonic
	&lspr[/*NOLIGHT*/BLUESHINE_L],     // SPR_METL
	&lspr[/*NOLIGHT*//*GOOPLIGHT_L*/PINKSHINE_L],     // SPR_MSCF
	&lspr[/*NOLIGHT*//*GOOPLIGHT_L*/PINKSHINE_L],     // SPR_MSCB

	// Collectible Items
	&lspr[/*NOLIGHT*/RINGSPARK_L],     // SPR_RING
	&lspr[NOLIGHT],     // SPR_TRNG
	&lspr[ORANGESHINE_L],// SPR_TOKE
	&lspr[REDBALL_L],   // SPR_RFLG
	&lspr[BLUEBALL_L],  // SPR_BFLG
	&lspr[NOLIGHT],     // SPR_SPHR
	&lspr[NOLIGHT],     // SPR_NCHP
	&lspr[NOLIGHT],     // SPR_NSTR
	&lspr[/*NOLIGHT*/LIGHT_SPR],     // SPR_EMBM
	&lspr[BLUESHINE_L], // SPR_CEMG
	&lspr[GREENSHINE_L],// SPR_SHRD

	// Interactive Objects
	&lspr[NOLIGHT],     // SPR_BBLS
	&lspr[NOLIGHT],     // SPR_SIGN
	&lspr[NOLIGHT],     // SPR_SPIK
	&lspr[NOLIGHT],     // SPR_SFLM
	&lspr[NOLIGHT],     // SPR_TFLM
	&lspr[NOLIGHT],     // SPR_USPK
	&lspr[NOLIGHT],     // SPR_WSPK
	&lspr[NOLIGHT],     // SPR_WSPB
	&lspr[NOLIGHT],     // SPR_STPT
	&lspr[NOLIGHT],     // SPR_BMNE
	&lspr[NOLIGHT],     // SPR_PUMI

	// Monitor Boxes
	&lspr[NOLIGHT],     // SPR_MSTV
	&lspr[NOLIGHT],     // SPR_XLTV

	&lspr[NOLIGHT],     // SPR_TRRI
	&lspr[NOLIGHT],     // SPR_TBRI

	&lspr[NOLIGHT],     // SPR_TVRI
	&lspr[NOLIGHT],     // SPR_TVPI
	&lspr[NOLIGHT],     // SPR_TVAT
	&lspr[NOLIGHT],     // SPR_TVFO
	&lspr[NOLIGHT],     // SPR_TVAR
	&lspr[NOLIGHT],     // SPR_TVWW
	&lspr[NOLIGHT],     // SPR_TVEL
	&lspr[NOLIGHT],     // SPR_TVSS
	&lspr[NOLIGHT],     // SPR_TVIV
	&lspr[NOLIGHT],     // SPR_TV1U
	&lspr[NOLIGHT],     // SPR_TV1P
	&lspr[NOLIGHT],     // SPR_TVEG
	&lspr[NOLIGHT],     // SPR_TVMX
	&lspr[NOLIGHT],     // SPR_TVMY
	&lspr[NOLIGHT],     // SPR_TVGV
	&lspr[NOLIGHT],     // SPR_TVRC
	&lspr[NOLIGHT],     // SPR_TV1K
	&lspr[NOLIGHT],     // SPR_TVTK
	&lspr[NOLIGHT],     // SPR_TVFL
	&lspr[NOLIGHT],     // SPR_TVBB
	&lspr[NOLIGHT],     // SPR_TVZP

	// Projectiles
	&lspr[NOLIGHT],     // SPR_MISL
	&lspr[SMALLREDBALL_L], // SPR_LASR
	&lspr[REDSHINE_L],  // SPR_LASF
	&lspr[NOLIGHT],     // SPR_TORP
	&lspr[NOLIGHT],     // SPR_ENRG
	&lspr[NOLIGHT],     // SPR_MINE
	&lspr[NOLIGHT],     // SPR_JBUL
	&lspr[SMALLREDBALL_L], // SPR_TRLS
	&lspr[NOLIGHT],     // SPR_CBLL
	&lspr[NOLIGHT],     // SPR_AROW
	&lspr[NOLIGHT],     // SPR_CFIR

	// Greenflower Scenery
	&lspr[NOLIGHT],     // SPR_FWR1
	&lspr[NOLIGHT],     // SPR_FWR2
	&lspr[NOLIGHT],     // SPR_FWR3
	&lspr[NOLIGHT],     // SPR_FWR4
	&lspr[NOLIGHT],     // SPR_BUS1
	&lspr[NOLIGHT],     // SPR_BUS2
	&lspr[NOLIGHT],     // SPR_BUS3
	// Trees (both GFZ and misc)
	&lspr[NOLIGHT],     // SPR_TRE1
	&lspr[NOLIGHT],     // SPR_TRE2
	&lspr[NOLIGHT],     // SPR_TRE3
	&lspr[NOLIGHT],     // SPR_TRE4
	&lspr[NOLIGHT],     // SPR_TRE5
	&lspr[NOLIGHT],     // SPR_TRE6

	// Techno Hill Scenery
	&lspr[NOLIGHT],     // SPR_THZP
	&lspr[NOLIGHT],     // SPR_FWR5
	&lspr[REDBALL_L],   // SPR_ALRM

	// Deep Sea Scenery
	&lspr[NOLIGHT],     // SPR_GARG
	&lspr[NOLIGHT],     // SPR_SEWE
	&lspr[NOLIGHT],     // SPR_DRIP
	&lspr[NOLIGHT],     // SPR_CRL1
	&lspr[NOLIGHT],     // SPR_CRL2
	&lspr[NOLIGHT],     // SPR_CRL3
	&lspr[NOLIGHT],     // SPR_BCRY

	// Castle Eggman Scenery
	&lspr[NOLIGHT],     // SPR_CHAN
	&lspr[REDBALL_L],   // SPR_FLAM
	&lspr[NOLIGHT],     // SPR_ESTA
	&lspr[NOLIGHT],     // SPR_SMCH
	&lspr[NOLIGHT],     // SPR_BMCH
	&lspr[NOLIGHT],     // SPR_SMCE
	&lspr[NOLIGHT],     // SPR_BMCE
	&lspr[NOLIGHT],     // SPR_YSPB
	&lspr[NOLIGHT],     // SPR_RSPB
	&lspr[REDBALL_L],   // SPR_SFBR
	&lspr[REDBALL_L],   // SPR_BFBR
	&lspr[NOLIGHT],     // SPR_BANR
	&lspr[NOLIGHT],     // SPR_PINE
	&lspr[NOLIGHT],     // SPR_CEZB
	&lspr[REDBALL_L],   // SPR_CNDL
	&lspr[NOLIGHT],     // SPR_FLMH
	&lspr[REDBALL_L],   // SPR_CTRC
	&lspr[NOLIGHT],     // SPR_CFLG
	&lspr[NOLIGHT],     // SPR_CSTA
	&lspr[NOLIGHT],     // SPR_CBBS

	// Arid Canyon Scenery
	&lspr[NOLIGHT],     // SPR_BTBL
	&lspr[NOLIGHT],     // SPR_STBL
	&lspr[NOLIGHT],     // SPR_CACT
	&lspr[NOLIGHT],     // SPR_WWSG
	&lspr[NOLIGHT],     // SPR_WWS2
	&lspr[NOLIGHT],     // SPR_WWS3
	&lspr[NOLIGHT],     // SPR_OILL
	&lspr[NOLIGHT],     // SPR_OILF
	&lspr[NOLIGHT],     // SPR_BARR
	&lspr[NOLIGHT],     // SPR_REMT
	&lspr[NOLIGHT],     // SPR_TAZD
	&lspr[NOLIGHT],     // SPR_ADST
	&lspr[NOLIGHT],     // SPR_MCRT
	&lspr[NOLIGHT],     // SPR_MCSP
	&lspr[NOLIGHT],     // SPR_SALD
	&lspr[NOLIGHT],     // SPR_TRAE
	&lspr[NOLIGHT],     // SPR_TRAI
	&lspr[NOLIGHT],     // SPR_STEA

	// Red Volcano Scenery
	&lspr[REDBALL_L],   // SPR_FLME
	&lspr[REDBALL_L],   // SPR_DFLM
	&lspr[NOLIGHT],     // SPR_LFAL
	&lspr[NOLIGHT],     // SPR_JPLA
	&lspr[NOLIGHT],     // SPR_TFLO
	&lspr[NOLIGHT],     // SPR_WVIN

	// Dark City Scenery

	// Egg Rock Scenery

	// Christmas Scenery
	&lspr[NOLIGHT],     // SPR_XMS1
	&lspr[NOLIGHT],     // SPR_XMS2
	&lspr[NOLIGHT],     // SPR_XMS3
	&lspr[NOLIGHT],     // SPR_XMS4
	&lspr[NOLIGHT],     // SPR_XMS5
	&lspr[NOLIGHT],     // SPR_XMS6
	&lspr[NOLIGHT],     // SPR_FHZI
	&lspr[NOLIGHT],     // SPR_ROSY

	// Halloween Scenery
	&lspr[RINGLIGHT_L], // SPR_PUMK
	&lspr[NOLIGHT],     // SPR_HHPL
	&lspr[NOLIGHT],     // SPR_SHRM
	&lspr[NOLIGHT],     // SPR_HHZM

	// Azure Temple Scenery
	&lspr[NOLIGHT],     // SPR_BGAR
	&lspr[NOLIGHT],     // SPR_RCRY
	&lspr[GREENBALL_L], // SPR_CFLM

	// Botanic Serenity Scenery
	&lspr[NOLIGHT],     // SPR_BSZ1
	&lspr[NOLIGHT],     // SPR_BSZ2
	&lspr[NOLIGHT],     // SPR_BSZ3
	//&lspr[NOLIGHT],     // SPR_BSZ4
	&lspr[NOLIGHT],     // SPR_BST1
	&lspr[NOLIGHT],     // SPR_BST2
	&lspr[NOLIGHT],     // SPR_BST3
	&lspr[NOLIGHT],     // SPR_BST4
	&lspr[NOLIGHT],     // SPR_BST5
	&lspr[NOLIGHT],     // SPR_BST6
	&lspr[NOLIGHT],     // SPR_BSZ5
	&lspr[NOLIGHT],     // SPR_BSZ6
	&lspr[NOLIGHT],     // SPR_BSZ7
	&lspr[NOLIGHT],     // SPR_BSZ8

	// Misc Scenery
	&lspr[NOLIGHT],     // SPR_STLG
	&lspr[NOLIGHT],     // SPR_DBAL

	// Powerup Indicators
	&lspr[NOLIGHT],     // SPR_ARMA
	&lspr[NOLIGHT],     // SPR_ARMF
	&lspr[NOLIGHT],     // SPR_ARMB
	&lspr[NOLIGHT],     // SPR_WIND
	&lspr[NOLIGHT],     // SPR_MAGN
	&lspr[NOLIGHT],     // SPR_ELEM
	&lspr[NOLIGHT],     // SPR_FORC
	&lspr[NOLIGHT],     // SPR_PITY
	&lspr[NOLIGHT],     // SPR_FIRS
	&lspr[NOLIGHT],     // SPR_BUBS
	&lspr[NOLIGHT],     // SPR_ZAPS
	&lspr[INVINCIBLE_L], // SPR_IVSP
	&lspr[SUPERSPARK_L], // SPR_SSPK

	&lspr[/*NOLIGHT*/STREETLIGHT_L],     // SPR_GOAL

	// Flickies
	&lspr[NOLIGHT],     // SPR_FBUB
	&lspr[NOLIGHT],     // SPR_FL01
	&lspr[NOLIGHT],     // SPR_FL02
	&lspr[NOLIGHT],     // SPR_FL03
	&lspr[NOLIGHT],     // SPR_FL04
	&lspr[NOLIGHT],     // SPR_FL05
	&lspr[NOLIGHT],     // SPR_FL06
	&lspr[NOLIGHT],     // SPR_FL07
	&lspr[NOLIGHT],     // SPR_FL08
	&lspr[NOLIGHT],     // SPR_FL09
	&lspr[NOLIGHT],     // SPR_FL10
	&lspr[NOLIGHT],     // SPR_FL11
	&lspr[NOLIGHT],     // SPR_FL12
	&lspr[NOLIGHT],     // SPR_FL13
	&lspr[NOLIGHT],     // SPR_FL14
	&lspr[NOLIGHT],     // SPR_FL15
	&lspr[NOLIGHT],     // SPR_FL16
	&lspr[NOLIGHT],     // SPR_FS01
	&lspr[NOLIGHT],     // SPR_FS02

	// Springs
	&lspr[NOLIGHT],     // SPR_FANS
	&lspr[NOLIGHT],     // SPR_STEM
	&lspr[NOLIGHT],     // SPR_BUMP
	&lspr[NOLIGHT],     // SPR_BLON
	&lspr[NOLIGHT],     // SPR_SPRY
	&lspr[NOLIGHT],     // SPR_SPRR
	&lspr[NOLIGHT],     // SPR_SPRB
	&lspr[NOLIGHT],     // SPR_YSPR
	&lspr[NOLIGHT],     // SPR_RSPR
	&lspr[NOLIGHT],     // SPR_BSPR
	&lspr[NOLIGHT],     // SPR_SSWY
	&lspr[NOLIGHT],     // SPR_SSWR
	&lspr[NOLIGHT],     // SPR_SSWB
	&lspr[NOLIGHT],     // SPR_BSTY
	&lspr[NOLIGHT],     // SPR_BSTR

	// Environmental Effects
	&lspr[NOLIGHT],     // SPR_RAIN
	&lspr[NOLIGHT],     // SPR_SNO1
	&lspr[NOLIGHT],     // SPR_SPLH
	&lspr[NOLIGHT],     // SPR_LSPL
	&lspr[NOLIGHT],     // SPR_SPLA
	&lspr[NOLIGHT],     // SPR_SMOK
	&lspr[NOLIGHT],     // SPR_BUBL
	&lspr[RINGLIGHT_L], // SPR_WZAP
	&lspr[NOLIGHT],     // SPR_DUST
	&lspr[NOLIGHT],     // SPR_FPRT
	&lspr[SUPERSPARK_L], // SPR_TFOG
	&lspr[NIGHTSLIGHT_L], // SPR_SEED
	&lspr[NOLIGHT],     // SPR_PRTL

	// Game Indicators
	&lspr[NOLIGHT],     // SPR_SCOR
	&lspr[NOLIGHT],     // SPR_DRWN
	&lspr[NOLIGHT],     // SPR_FLII
	&lspr[NOLIGHT],     // SPR_LCKN
	&lspr[NOLIGHT],     // SPR_TTAG
	&lspr[NOLIGHT],     // SPR_GFLG
	&lspr[NOLIGHT],     // SPR_FNSF

	&lspr[NOLIGHT],     // SPR_CORK
	&lspr[NOLIGHT],     // SPR_LHRT

	// Ring Weapons
	&lspr[RINGLIGHT_L],     // SPR_RRNG
	&lspr[RINGLIGHT_L],     // SPR_RNGB
	&lspr[RINGLIGHT_L],     // SPR_RNGR
	&lspr[RINGLIGHT_L],     // SPR_RNGI
	&lspr[RINGLIGHT_L],     // SPR_RNGA
	&lspr[RINGLIGHT_L],     // SPR_RNGE
	&lspr[RINGLIGHT_L],     // SPR_RNGS
	&lspr[RINGLIGHT_L],     // SPR_RNGG

	&lspr[RINGLIGHT_L],     // SPR_PIKB
	&lspr[RINGLIGHT_L],     // SPR_PIKR
	&lspr[RINGLIGHT_L],     // SPR_PIKA
	&lspr[RINGLIGHT_L],     // SPR_PIKE
	&lspr[RINGLIGHT_L],     // SPR_PIKS
	&lspr[RINGLIGHT_L],     // SPR_PIKG

	&lspr[RINGLIGHT_L],     // SPR_TAUT
	&lspr[RINGLIGHT_L],     // SPR_TGRE
	&lspr[RINGLIGHT_L],     // SPR_TSCR

	// Mario-specific stuff
	&lspr[NOLIGHT],     // SPR_COIN
	&lspr[NOLIGHT],     // SPR_CPRK
	&lspr[NOLIGHT],     // SPR_GOOM
	&lspr[NOLIGHT],     // SPR_BGOM
	&lspr[REDBALL_L],     // SPR_FFWR
	&lspr[SMALLREDBALL_L], // SPR_FBLL
	&lspr[NOLIGHT],     // SPR_SHLL
	&lspr[REDBALL_L],   // SPR_PUMA
	&lspr[NOLIGHT],     // SPR_HAMM
	&lspr[NOLIGHT],     // SPR_KOOP
	&lspr[REDBALL_L],   // SPR_BFLM
	&lspr[NOLIGHT],     // SPR_MAXE
	&lspr[NOLIGHT],     // SPR_MUS1
	&lspr[NOLIGHT],     // SPR_MUS2
	&lspr[NOLIGHT],     // SPR_TOAD

	// NiGHTS Stuff
	&lspr[SUPERSONIC_L], // SPR_NDRN // NiGHTS drone
	&lspr[NOLIGHT],     // SPR_NSPK
	&lspr[NOLIGHT],     // SPR_NBMP
	&lspr[NOLIGHT],     // SPR_HOOP
	&lspr[NOLIGHT],     // SPR_HSCR
	&lspr[NOLIGHT],     // SPR_NPRU
	&lspr[NOLIGHT],     // SPR_CAPS
	&lspr[INVINCIBLE_L], // SPR_IDYA
	&lspr[NOLIGHT],     // SPR_NTPN
	&lspr[NOLIGHT],     // SPR_SHLP

	// Secret badniks and hazards, shhhh
	&lspr[NOLIGHT],     // SPR_PENG
	&lspr[NOLIGHT],     // SPR_POPH,
	&lspr[NOLIGHT],     // SPR_HIVE
	&lspr[NOLIGHT],     // SPR_BUMB,
	&lspr[NOLIGHT],     // SPR_BBUZ
	&lspr[NOLIGHT],     // SPR_FMCE,
	&lspr[NOLIGHT],     // SPR_HMCE,
	&lspr[NOLIGHT],     // SPR_CACO,
	&lspr[BLUEBALL_L],  // SPR_BAL2,
	&lspr[NOLIGHT],     // SPR_SBOB,
	&lspr[BLUEBALL_L],  // SPR_SBFL,
	&lspr[BLUEBALL_L],  // SPR_SBSK,
	&lspr[NOLIGHT],     // SPR_BATT,

	// Debris
	&lspr[RINGSPARK_L],  // SPR_SPRK
	&lspr[NOLIGHT],      // SPR_BOM1
	&lspr[SUPERSPARK_L], // SPR_BOM2
	&lspr[SUPERSPARK_L], // SPR_BOM3
	&lspr[NOLIGHT],      // SPR_BOM4
	&lspr[REDBALL_L],    // SPR_BMNB

	// Crumbly rocks
	&lspr[NOLIGHT],     // SPR_ROIA
	&lspr[NOLIGHT],     // SPR_ROIB
	&lspr[NOLIGHT],     // SPR_ROIC
	&lspr[NOLIGHT],     // SPR_ROID
	&lspr[NOLIGHT],     // SPR_ROIE
	&lspr[NOLIGHT],     // SPR_ROIF
	&lspr[NOLIGHT],     // SPR_ROIG
	&lspr[NOLIGHT],     // SPR_ROIH
	&lspr[NOLIGHT],     // SPR_ROII
	&lspr[NOLIGHT],     // SPR_ROIJ
	&lspr[NOLIGHT],     // SPR_ROIK
	&lspr[NOLIGHT],     // SPR_ROIL
	&lspr[NOLIGHT],     // SPR_ROIM
	&lspr[NOLIGHT],     // SPR_ROIN
	&lspr[NOLIGHT],     // SPR_ROIO
	&lspr[NOLIGHT],     // SPR_ROIP

	// Level debris
	&lspr[NOLIGHT], // SPR_GFZD
	&lspr[NOLIGHT], // SPR_BRIC
	&lspr[NOLIGHT], // SPR_WDDB
	&lspr[NOLIGHT], // SPR_BRIR
	&lspr[NOLIGHT], // SPR_BRIB
	&lspr[NOLIGHT], // SPR_BRIY

	// Gravity Well Objects
	&lspr[NOLIGHT],     // SPR_GWLG
	&lspr[NOLIGHT],     // SPR_GWLR

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

	// STAR STUFF //
	[SPR_EEGG] = &lspr[GREENSHINE_L],
	&lspr[SUPERSONIC_L], // SPR_TF2D
	// LIGHT TABLE SET! //
};
#endif
