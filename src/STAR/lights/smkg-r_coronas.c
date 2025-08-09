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
/// \file  smkg-r_coronas.c
/// \brief TSoURDt3rd's software corona rendering routines

#include "../smkg-defs.h"

#ifdef ALAM_LIGHTING

#include "smkg-coronas.h"
#include "../smkg-cvars.h"

#include "../../i_video.h"
#include "../../m_random.h"
#include "../../r_draw.h"
#include "../../r_main.h"
#include "../../v_video.h"
#include "../../w_wad.h"
#include "../../z_zone.h"

#ifdef HWRENDER
#include "../../hardware/hw_main.h"
#endif

// ==========================================================================
//																	CORONAS
// ==========================================================================

static patch_t *corona_patch = NULL;
static softwarepatch_t corona_sprlump;

// One or the other.
#ifdef ENABLE_DRAW_ALPHA
#else
#define ENABLE_COLORED_PATCH
// [WDJ] Wad patches usable as corona.
// It is easier to recolor during drawing than to pick one of each.
// Only use patches that are likely to be round in every instance (teleport fog is often not round).
// Corona alternatives list.
const char *corona_name[] = {
	"CORONAP", // patch version of corona, from legacy.wad
	"FLAMM0",
	"PLSEA0",
  	NULL
};
#endif

#ifdef ENABLE_COLORED_PATCH
static size_t corona_patch_size;

typedef struct
{
	RGBA_t corona_color;
	patch_t *colored_patch; // Z_Malloc
} corona_image_t;

static corona_image_t corona_image[NUMLIGHTS];
#endif

#ifdef ENABLE_COLORED_PATCH
// Also does release, after corona_patch_size is set.
static void init_corona_data(void)
{
	for (INT32 i = 0; i < NUMLIGHTS; i++ )
	{
		if (corona_patch_size)
		{
			if (corona_image[i].colored_patch)
				Z_Free(corona_image[i].colored_patch);
		}
		corona_image[i].corona_color.rgba = 0;
		corona_image[i].colored_patch = NULL;
	}
}
#endif

#ifdef ENABLE_COLORED_PATCH
static void setup_colored_corona(corona_image_t *ccp, RGBA_t corona_color)
{
	UINT8 colormap[256];
	patch_t	*pp;

	// when draw alpha is intense cannot have faint color in corona image
	INT32 alpha = (255 + corona_color.s.alpha) >> 1;
	INT32 za = (255 - alpha);
	INT32 c;
	INT32 r, g, b;

	// A temporary colormap; make a colormap that is mostly of the corona color
	for (c = 0; c < 256; c++)
	{
		RGBA_t rc = pLocalPalette[dc_colormap[c]];
		r = (corona_color.s.red * alpha + rc.s.red * za) >> 8;
		g = (corona_color.s.green * alpha + rc.s.green * za) >> 8;
		b = (corona_color.s.blue * alpha + rc.s.blue * za) >> 8;
		colormap[c] = NearestColor( r, g, b );
	}

	// Allocate a copy of the corona patch.
	ccp->corona_color = corona_color;
	if (ccp->colored_patch)
		Z_Free(ccp->colored_patch);

	pp = Z_Malloc(corona_patch_size, PU_STATIC, NULL);
	ccp->colored_patch = pp;
	memcpy(pp, corona_patch, corona_patch_size);

#if 0
	// Change the corona copy to the corona color.

#if 1
	for (c = 0; c < corona_patch->width; c++ )
	{
		column_t *cp = (column_t *)((UINT8 *)pp + pp->columnofs[c]);
		while (cp->topdelta != 0xff) // end of posts
		{
			UINT8 *s = (UINT8 *)cp + 3;
			INT32 count = cp->length;
			while (count--)
			{
				*s = colormap[*s];
				s++;
			}

			// next source post, adv by (length + 2 byte header + 2 extra bytes)
			cp = (column_t *)((UINT8 *)cp + cp->length + 4);
		}
	}
#else
	UINT8 *pdata;

	softwarepatch_t *realpatch = (softwarepatch_t *)corona_patch;
	UINT8 *colofs = (UINT8 *)realpatch->columnofs;

	for (x = 0; x < texture->width; x++)
	{
		doompost_t *col = (doompost_t *)((UINT8 *)realpatch + LONG(*(UINT32 *)&colofs[x<<2]));
		INT32 topdelta, prevdelta = -1, y = 0;
		while (col->topdelta != 0xff)
		{
#if 0
			topdelta = col->topdelta;
			if (topdelta <= prevdelta)
				topdelta += prevdelta;
			prevdelta = topdelta;
			if (topdelta > y)
				break;
			y = topdelta + col->length + 1;
			col = (doompost_t *)((UINT8 *)col + col->length + 4);
#else
			UINT8 *s = (UINT8 *)cp + 3;
			INT32 count = cp->length;
			while (count--)
			{
				*s = colormap[*s];
				s++;
			}

			// next source post, adv by (length + 2 byte header + 2 extra bytes)
			cp = (column_t *)((UINT8 *)cp + cp->length + 4);
			if (y < texture->height)
				texture->transparency = true; // this texture is HOLEy! D:
#endif
		}
	}
#endif
#endif
}

static patch_t *get_colored_corona(int sprite_light_num)
{
	corona_image_t *cc = &corona_image[sprite_light_num];
	RGBA_t corona_color = V_GetColor(lspr[sprite_light_num].corona_color);

	if (cc->corona_color.rgba != corona_color.rgba || cc->colored_patch == NULL)
		setup_colored_corona(cc, corona_color);

	return cc->colored_patch;
}
#endif

// Called by SCR_SetMode
void TSoURDt3rd_R_Load_Corona(void)
{
#ifdef ENABLE_COLORED_PATCH
	lumpnum_t lumpid = LUMPERROR;
#endif

#ifdef HWRENDER
	if (rendermode != render_soft)
		return;
#endif

#ifdef ENABLE_DRAW_ALPHA
	if (!corona_patch)
	{
		pic_t *corona_pic = (pic_t *)W_CachePicName("CORONA", PU_STATIC);
		if (corona_pic)
		{
			// Z_Malloc; The corona pic is INTENSITY_ALPHA, bytepp=2, blank=0
			corona_patch = (patch_t *)R_Create_Patch(corona_pic->width, corona_pic->height,
				/*SRC*/    TM_row_image, & corona_pic->data[0], 2, 1, 0,
				/*DEST*/   TM_patch, CPO_blank_trim, NULL);

			Z_ChangeTag(corona_patch, PU_STATIC);
			corona_patch->leftoffset += corona_pic->width/2;
			corona_patch->topoffset += corona_pic->height/2;

			// Don't need the corona pic_t anymore
			Z_Free(corona_pic);
			goto setup_corona;
		}
	}
#endif

#ifdef ENABLE_COLORED_PATCH
	init_corona_data(); // must call at least once, before setting corona_patch_size

	if (!corona_patch)
	{
		// Find first valid patch in corona_name list
		//const char **namep = &corona_name[0];
		INT32 i = 0;
		const char *namep = corona_name[i];
		while (namep)
		{
			lumpid = W_CheckNumForName(namep);
			if (lumpid != LUMPERROR)
				goto setup_corona;
			//namep++;
			namep = corona_name[++i];
		}
	}
#endif

	setup_corona:
	{
#ifdef ENABLE_COLORED_PATCH
		// Setup the corona support
		corona_patch_size = W_LumpLength(lumpid);
		corona_patch = W_CachePatchNum(lumpid, PU_STATIC);
#endif

		// The patch endian conversion is already done.
		corona_sprlump.width = corona_patch->width << FRACBITS;
		corona_sprlump.height = corona_patch->height << FRACBITS;
		corona_sprlump.leftoffset = corona_patch->leftoffset << FRACBITS;
		corona_sprlump.topoffset = corona_patch->topoffset << FRACBITS;
	}
}

void TSoURDt3rd_R_Release_Coronas(void)
{
#ifdef ENABLE_COLORED_PATCH
	init_corona_data(); // does release too
#endif
	if (corona_patch)
	{
		Z_Free(corona_patch);
		corona_patch = NULL;
	}
}

// --------------------------------------------------------------------------
// coronas lighting with the sprite
// --------------------------------------------------------------------------

// corona state
fixed_t		corona_x0, corona_x1, corona_x2;
fixed_t		corona_xscale, corona_yscale;
float		corona_size;
UINT8		corona_alpha;
UINT8		corona_bright; // used by software draw to brighten active light sources
UINT8		corona_index; // t_lspr index
UINT8		corona_draw = 0; // 1 = before sprite, 2 = after sprite

UINT8 spec_dist[ 16 ] = {
	10,  // SPLT_unk
	35,  // SPLT_rocket
	20,  // SPLT_lamp
	45,  // SPLT_fire
	0, 0, 0, 0, 0, 0, 0, 0,
	60,  // SPLT_light
	30,  // SPLT_firefly
	80,  // SPLT_random
	80,  // SPLT_pulse
};

typedef enum
{
   FADE_FAR = 0x01,
   FADE_NEAR = 0x02
} sprite_corona_fade_e;

#define NUM_FIRE_PATTERN 64
static UINT8 fire_pattern[NUM_FIRE_PATTERN];
static UINT8 fire_pattern_tic[NUM_FIRE_PATTERN];

#define NUM_RAND_PATTERN 32
static UINT8 rand_pattern_cnt[NUM_RAND_PATTERN];
static UINT8 rand_pattern_state[NUM_RAND_PATTERN];
static UINT8 rand_pattern_tic[NUM_RAND_PATTERN];

//
//  sprnum : sprite number
//
//  Return: corona_index
//  Return NULL when no draw.
//
light_t *Sprite_Corona_Light_lsp(INT32 sprnum)
{
	light_t *p_lspr = t_lspr[sprnum];
	UINT8 li;

	if (p_lspr == NULL)
		return NULL;
	li = p_lspr->type;

	corona_index = li;
	if (li == NOLIGHT)
		return NULL;

	return p_lspr;
}

//
//  p_lspr : sprite light
//  cz : distance to corona
//
//  Return: corona_alpha, corona_size
//  Return 0 when no draw.
//
UINT8 Sprite_Corona_Light_fade(light_t *p_lspr, float cz, mobj_t *mobj)
{
	float relsize;
	UINT16 type, cflags;
	UINT8 fade;
	UINT32 index, v;

	if (!cv_tsourdt3rd_video_lighting_coronas.value)
	{
		// Lighting has been disabled.
		goto no_corona;
	}

	// Objects which emit light.
	type = (p_lspr->impl_flags & TYPE_FIELD_SPR); // working type setting
	cflags = p_lspr->type;
	if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(mobj, NULL, &corona_alpha, false))
		corona_alpha = V_GetColor(p_lspr->corona_color).s.alpha;
	corona_bright = 0;

#ifndef STAR_LIGHTING
#ifdef HWRENDER
	if (cv_glcoronas.changed)
	{
		// Force light setup, without another test.
		for (int i = 0; i < NUMLIGHTS; i++)
			t_lspr[i]->impl_flags |= SLI_changed;
	}
#endif
#endif

	// Update flagged by corona setting change, and fragglescript settings.
	if (p_lspr->impl_flags & SLI_changed)
	{
		p_lspr->impl_flags &= ~SLI_changed;

		// [WDJ] Fixes must be determined here because Phobiata and other wads,
		// do not set all the dependent fields at one time.
		// They never set some fields, like type, at all.
		type = cflags & TYPE_FIELD_SPR; // table or fragglescript setting

#ifdef STAR_LIGHTING
		// Old
		if (cv_tsourdt3rd_video_lighting_coronas.value == 6)
		{
			// Revert the new tables to use
			// only that flags that existed in Old.
			cflags &= (CORONA_SPR|DYNLIGHT_SPR);
			if (type != SPLT_rocket)
			{
			   if (cflags & DYNLIGHT_SPR)
				  type = SPLT_lamp;  // closest to old SPLGT_light
			   else
				  type = SPLT_unk;  // corona only
			}
		}
		else
#endif

		// We have no way of determining the intended version compatibility.  This limits
		// the characteristics that we can check.
		// Some older wads just used the existing corona without setting the type.
		// The default type of some of the existing corona have changed to use the new
		// corona types for ordinary wads, version 1.47.3.
		if ((p_lspr->impl_flags & SLI_corona_set)  // set by fragglescript
			&& (!(p_lspr->impl_flags & SLI_type_set) || (type == SPLT_unk)))
		{
			// Correct corona settings made by older wads, such as Phobiata, and newmaps.
			// Has the old default type, or type was never set.
#if 0
			// In the original code, the alpha from the corona color was ignored,
			// even though it was set in the tables.  Instead the draw code used 0xff.
			if (corona_alpha == 0)
			{
				// previous default
				if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(mobj, NULL, &corona_alpha, false))
					corona_alpha = V_GetColor(p_lspr->corona_color).s.alpha = 0xff;
				else
					corona_alpha = 0xff;
			}
#endif

			// Refine some of the old wad settings, to use new capabilities correctly.
			// Check for Phobiata and newmaps problems.
			if (corona_alpha > 0xDF)
			{
				// Default radius is 20 to 120.
				// Phobiata flies have a radius of 7
				if (p_lspr->corona_radius < 10.0f)
				{
					// newmaps and phobiata firefly
					type = SPLT_light;
				}
				else if (p_lspr->corona_radius < 80.0f)
				{
					// torches
					type = SPLT_lamp;
				}
			}
		}
		// update the working type
		p_lspr->impl_flags = (p_lspr->impl_flags & ~TYPE_FIELD_SPR) | type;
	}

#ifdef STAR_LIGHTING
	switch (cv_tsourdt3rd_video_lighting_coronas.value)
	{
		case 6: // Old
			corona_alpha = 0xff; // alpha settings were ignored
			break;
		case 5: // Bright
			corona_bright = 20; // brighten the default cases
			//corona_alpha = (((UINT8)corona_alpha * 3) + 255) >> 2; // +25%
			corona_alpha = ((UINT8)corona_alpha * 3) >> 2; // -25%
			break;
		case 3: // Dim
			//corona_alpha = ((UINT8)corona_alpha * 3) >> 2; // -25%
			corona_alpha = (((UINT8)corona_alpha * 3) + 255) >> 2; // +25%
			break;
		default: // Special, Most
			if (cv_tsourdt3rd_video_lighting_coronas.value <= 2)
			{
				INT32 spec = spec_dist[type>>4];

				if (p_lspr->impl_flags & SLI_corona_set) // set by wad
					spec <<= 2;

				if (cv_tsourdt3rd_video_lighting_coronas.value == 2) // Most
				{
					// Must do this before any flicker modifications, or else they blink.
					// ignore the dim corona
					if (corona_alpha < 40)
						goto no_corona;

					// not close enough
					if (corona_alpha + spec + CORONA_Z1 < cz)
						goto no_corona;
				}
				else
				{
					// not special enough
					if ((spec < 33) && (cz > (CORONA_Z1 + CORONA_Z2)/2))
						goto no_corona;

					// ignore the dim corona
					if (corona_alpha < 20)
						goto no_corona;
				}
			}
			break;
	}
#endif

	relsize = 1.0f;
	fade = FADE_FAR | FADE_NEAR;

	// Each of these types has a corona.
	switch (type)
	{
		case SPLT_unk: // corona only
			// object corona
			relsize = ((cz+60.0f)/100.0f);
			break;
		case SPLT_rocket: // flicker
			// svary the alpha
			relsize = ((cz+60.0f)/100.0f);
			corona_alpha = (UINT8)((M_RandomByte()>>1)&0xff);
			corona_bright = 128;
			break;
		case SPLT_lamp:  // lamp with a corona
			// lamp corona
			relsize = ((cz+120.0f)/950.0f);
			corona_bright = 40;
			break;
		case SPLT_fire: // slow flicker, torch
			// torches
			relsize = ((cz+120.0f)/950.0f);
			index = (mobj->type & (NUM_FIRE_PATTERN - 1)); // obj dependent
			if (fire_pattern_tic[index] != gametic)
			{
				fire_pattern_tic[index] = gametic;
				if (P_RandomByte() > 35)
				{
					INT32 r = P_RandomByte();
					r = ((r - 128) >> 3) + fire_pattern[index];
					if (r > 50)
						r = 40;
					else if (r < -50)
						r = -40;
					fire_pattern[index] = r;
				}
			}
			v = (UINT32)(corona_alpha + fire_pattern[index]);
			if (v > 255)
				v = 255;
			if (v < 4)
				v = 4;
			corona_alpha = v;
			corona_bright = 45;
			break;
		case SPLT_light: // no corona fade
			// newmaps and phobiata firefly
			relsize = ((cz + 120.0f) / 950.0f); // dimming with distance
#if 0
			if ((cz < CORONA_Z1) & ((p_lspr->type & SPLGT_source) == 0))
			{
				// Fade corona partial to 0 when get too close
				corona_alpha = (UINT8)((atof(corona_alpha) * corona_alpha + (255 - corona_alpha) * (corona_alpha * cz / CORONA_Z1)) / 255.0f);
			}
#endif
			// Version 1.42 had corona_alpha = 0xff
			corona_bright = 132;
			fade = FADE_FAR;
			break;
		case SPLT_firefly: // firefly blink, un-synch
			// lower 6 bits gives a repeat rate of 1.78 seconds
			if (((gametic + mobj->type) & 0x003F) < 0x20) // obj dependent phase
				goto no_corona; // blink off
			fade = FADE_FAR;
			break;
		case SPLT_random: // random LED, un-synch
			index = (mobj->type & (NUM_RAND_PATTERN-1)); // obj dependent counter
			if (rand_pattern_tic[index] != gametic)
			{
				rand_pattern_tic[index] = gametic;
				if (rand_pattern_cnt[index] == 0)
				{
					rand_pattern_cnt[index] = P_RandomByte();
					rand_pattern_state[index]++;
				}
				rand_pattern_cnt[index]--;
			}
			if ((rand_pattern_state[index] & 1) == 0)
				goto no_corona; // off
			corona_bright = 128;
			fade = 0;
			break;
		case SPLT_pulse: // slow pulsation, un-synch
			index = ((gametic + mobj->type) & 0xFF); // obj dependent phase
			index -= 128; // -128 to +127
			// Make a positive parabola pulse, min does not quite reach 0.
			float f = 1.0f - ((index*index) * 0.000055f);
			relsize = f;
			corona_alpha = corona_alpha * f;
			corona_bright = 80;
			fade = 0;
			break;
		default:
			//CONS_Debug(DBG_RENDER, "Draw_Sprite_Corona_Light: unknown light type %x\n", type);
			CONS_Alert(CONS_WARNING, "Draw_Sprite_Corona_Light: unknown light type %x\n", type);
			goto no_corona;
	}

	if (cz > CORONA_Z1 && (fade & FADE_FAR))
	{
		// Proportional fade from Z1 to Z2
		corona_alpha = (int)(corona_alpha * (CORONA_Z2 - cz) / (CORONA_Z2 - CORONA_Z1));
	}
	else if (fade & FADE_NEAR)
	{
		// Fade to 0 when get too close
		corona_alpha = (int)(corona_alpha * cz / CORONA_Z1);
	}

	if (relsize > 1.0)
		relsize = 1.0;

	corona_size = (p_lspr->corona_radius * relsize);
#ifndef STAR_LIGHTING
#ifdef HWRENDER
	corona_size *= FIXED_TO_FLOAT(cv_glcoronasize.value);
#endif
#else
	corona_size *= FIXED_TO_FLOAT(cv_tsourdt3rd_video_lighting_coronas_size.value);
#endif
	return corona_alpha;

no_corona:
{
	corona_alpha = 0;
	return corona_alpha;
}

}

static void Sprite_Corona_Light_setup(vissprite_t *vis)
{
	fixed_t		tz;
	float		cz, size;
	light_t 	*p_lspr;

	// Objects which emit light.
	p_lspr = Sprite_Corona_Light_lsp(vis->mobj->sprite);
	if (p_lspr == NULL)
		goto no_corona;

	if (!(p_lspr->type & (CORONA_SPR|TYPE_FIELD_SPR)))
		goto no_corona;

	tz = FixedDiv(projectiony, vis->scale);
	cz = FIXED_TO_FLOAT(tz);

	// more realistique corona !
	if (cz >= CORONA_Z2)
		goto no_corona;

	// mobj dependent id
	if (!(Sprite_Corona_Light_fade(p_lspr, cz, vis->mobj)))
		goto no_corona;

	// brighten the corona for software draw
	if (corona_bright)
		corona_alpha = ((((UINT8)corona_alpha * (255 - corona_bright)) + (255 * (UINT8)corona_bright)) >> 8);

	size = corona_size / FIXED_TO_FLOAT(corona_sprlump.width);
	corona_xscale = FLOAT_TO_FIXED(((double)vis->xscale * size));
	corona_yscale = FLOAT_TO_FIXED(((double)vis->scale * size));

	// Corona specific.
	// Corona offsets are from center of drawn sprite.
	// no flip on corona

	// Position of the corona
#if 1
	fixed_t midx = (vis->x1 + vis->x2) << (FRACBITS-1);  // screen
#else
	// same as spr, but not stored in vissprite so must recalculate it
	//fixed_t tr_x = vis->mobj->x - viewx;
	//fixed_t tr_y = vis->mobj->y - viewy;
	fixed_t tr_x = vis->mobj_x - viewx;
	fixed_t tr_y = vis->mobj_y - viewy;
	fixed_t tx = (FixedMul(tr_x,viewsin) - FixedMul(tr_y,viewcos));
	fixed_t midx = centerxfrac + FixedMul(tx, vis->xscale);
#endif
	corona_x0 = corona_x1 = (midx - FixedMul(corona_sprlump.leftoffset, corona_xscale)) >>FRACBITS;
	corona_x2 = ((midx + FixedMul(corona_sprlump.width - corona_sprlump.leftoffset, corona_xscale)) >>FRACBITS) - 1;

	// off the right side
	if (corona_x1 < 0)
		corona_x1 = 0;
	if (corona_x1 > viewwidth)
		goto no_corona;

	// off the left side
	if (corona_x2 >= viewwidth)
		corona_x2 = viewwidth - 1;
	if (corona_x2 < 0)
		goto no_corona;

	corona_draw = 2;

	no_corona:
	{
		corona_draw = 0;
		return;
	}
}

static void Draw_Sprite_Corona_Light(vissprite_t * vis)
{
	int texturecolumn;

	// Sprite has a corona, and coronas are enabled.
	long dr_alpha = (((UINT8)corona_alpha * 7) + (2 * (16-7))) >> 4; // compensate for different HWR alpha

#ifdef ENABLE_DRAW_ALPHA
	colfunc = alpha_colfunc;  // R_DrawAlphaColumn
	patch_t *corona_cc_patch = corona_patch;

	dr_color = t_lspr[vis->mobj->sprite]->corona_color;

#ifndef ENABLE_DRAW8_USING_12
	if (vid.drawmode == DRAW8PAL)
		dr_color8 = NearestColor(dr_color.s.red, dr_color.s.green, dr_color.s.blue);
#endif

#ifdef STAR_LIGHTING
	dr_alpha_mode = cv_tsourdt3rd_video_lighting_coronas_drawingmode.value;
	// alpha to dim the background through the corona
	dr_alpha_background = (cv_tsourdt3rd_video_lighting_coronas_drawingmode.value == 1 ? (255 - dr_alpha) : 240);
#else
	dr_alpha_mode = 2;
	// alpha to dim the background through the corona
	dr_alpha_background = 240;
#endif
#else
	colfunc = colfuncs[COLDRAWFUNC_TRANS]; // R_DrawTranslucentColumn; translate certain pixels to white

	// Get the corona patch specifically colored for this light.
	patch_t *corona_cc_patch = get_colored_corona(corona_index);

	// dc_colormap = & reg_colormaps[0];
	transtables = 0;  // translucent dr_alpha
	dc_transmap = R_GetTranslucencyTable(dr_alpha >> 4); // for draw8
#endif

	fixed_t light_yoffset = (fixed_t)(t_lspr[vis->mobj->sprite]->light_yoffset * FRACUNIT); // float to fixed

#if 1
	// [WDJ] This is the one that puts the center closest to where OpenGL puts it.
	fixed_t g_midy = (vis->gz + vis->gzt)>>1; // mid of sprite
#else
	// Too high
#if 0
	fixed_t g_midy = vis->mobj->z + ((vis->gzt - vis->gz)>>1);
#else
	fixed_t g_midy = (vis->mobj->z + vis->gzt)>>1;  // mid of sprite
#endif
#endif

	fixed_t g_cp = g_midy + light_yoffset - viewz;  // corona center point in vissprite scale
	fixed_t tp_cp = FixedMul(g_cp, vis->scale) + FixedMul(corona_sprlump.topoffset, corona_yscale);
	sprtopscreen = centeryfrac - tp_cp;
	dc_texturemid = FixedDiv(tp_cp, corona_yscale);
	spryscale = corona_yscale;
	dc_iscale = FixedDiv(FRACUNIT, dc_yh); // y texture step
	dc_texheight = 0; // no wrap repeat
//	dc_texheight = corona_patch->height;

// not flipped so
//  tex_x0 = 0
//  tex_x_iscale = iscale
//  fixed_t tex_x_iscale = (int)((double)vis->iscale*size);

	fixed_t tex_x_iscale = FixedDiv(FRACUNIT, corona_xscale);
	fixed_t texcol_frac = 0; // tex_x0, not flipped

	if ((corona_x1 - corona_x0) > 0) // it was clipped
		texcol_frac = tex_x_iscale * (corona_x1 - corona_x0);

	for (dc_x = corona_x1; dc_x <= corona_x2; dc_x++, texcol_frac += tex_x_iscale)
	{
		texturecolumn = texcol_frac>>FRACBITS;

#ifdef RANGECHECK
		// [WDJ] Give msg and don't draw it
		if (texturecolumn < 0 || texturecolumn >= corona_patch->width)
		{
			CONS_Debug(DBG_RENDER, "Sprite_Corona: bad texturecolumn\n");
			CONS_Alert(CONS_WARNING, "Sprite_Corona: bad texturecolumn\n");
			return;
		}
#endif

		column_t *col_data = &corona_cc_patch->columns[texturecolumn];
		R_DrawMaskedColumn(col_data, corona_cc_patch->height);
	}

	colfunc = colfuncs[BASEDRAWFUNC];
	colfunc();
}

void TSoURDt3rd_R_RenderSoftwareCoronas(vissprite_t *spr, INT32 x1, INT32 x2)
{
	corona_draw = 0;

#if 0
	// Exclude Split sprites that are cut on the bottom, so there
	// is only one corona per object.
	// Their position would be off too.
#ifdef STAR_LIGHTING
	if (cv_tsourdt3rd_video_lighting_coronas.value && (!(spr->cut & SC_BOTTOM)))
#else
	if (cv_glcoronas.value && (!(spr->cut & SC_BOTTOM)))
#endif
#else
	// setup corona state
#ifdef STAR_LIGHTING
	if (cv_tsourdt3rd_video_lighting_coronas.value)
#else
#ifdef HWRENDER
	if (cv_glcoronas.value)
#else
	return;
#endif
#endif
#endif
	{
		Sprite_Corona_Light_setup(spr); // set corona_draw
		if (corona_draw) // Expand clipping to include corona draw
		{
			if (corona_x1 < x1)
				x1 = corona_x1;
			if (corona_x2 > x2)
				x2 = corona_x2;
		}
	}
}

void TSoURDt3rd_R_DrawSoftwareCoronas(vissprite_t *spr)
{
#if 1
	// Draw corona before sprite, occlude
	if (corona_draw == 1)
		Draw_Sprite_Corona_Light(spr);
#endif

#if 0
#if 1
	// draw sprite, restricted x range
	R_DrawVisSprite(vis, ((dbx1 > vis->x1) ? dbx1 : vis->x1),
					((dbx2 < vis->x2) ? dbx2 : vis->x2));
#else
	R_DrawVisSprite(vis, cx1, cx2);
#endif
#endif

	if (corona_draw == 2)
		Draw_Sprite_Corona_Light(spr);
}

#endif // ALAM_LIGHTING
