// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 2013-2016 by Matthew "Kaito Sinclaire" Walsh.
// Copyright (C) 1999-2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  f_wipe.c
/// \brief SRB2 2.1 custom fade mask "wipe" behavior.

#include "f_finale.h"
#include "i_video.h"
#include "v_video.h"

#include "r_main.h" // framecount
#include "r_state.h" // fadecolormap
#include "r_draw.h" // transtable
#include "p_pspr.h" // tr_transxxx
#include "p_local.h"
#include "st_stuff.h"
#include "w_wad.h"
#include "z_zone.h"

#include "i_system.h"
#include "m_menu.h"
#include "console.h"
#include "d_main.h"
#include "g_game.h"
#include "y_inter.h" // intertype
#include "m_misc.h" // movie mode

#include "doomstat.h"

#include "lua_hud.h" // level title

#ifdef HWRENDER
#include "hardware/hw_main.h"
#endif

typedef struct fademask_s {
	UINT8* mask;
	UINT16 width, height;
	size_t size;
	fixed_t xscale, yscale;
} fademask_t;

UINT8 wipedefs[NUMWIPEDEFS] = {
	99, // wipe_credits_intermediate (0)

	0,  // wipe_level_toblack
	UINT8_MAX,  // wipe_intermission_toblack
	0,  // wipe_continuing_toblack
	0,  // wipe_titlescreen_toblack
	0,  // wipe_timeattack_toblack
	99, // wipe_credits_toblack
	0,  // wipe_evaluation_toblack
	0,  // wipe_gameend_toblack
	99, // wipe_intro_toblack (hardcoded)
	0,  // wipe_ending_toblack
	99, // wipe_cutscene_toblack (hardcoded)

	0,  // wipe_specinter_toblack
	0,  // wipe_multinter_toblack
	0,  // wipe_speclevel_towhite

	0,  // wipe_level_final
	0,  // wipe_intermission_final
	0,  // wipe_continuing_final
	0,  // wipe_titlescreen_final
	0,  // wipe_timeattack_final
	99, // wipe_credits_final
	0,  // wipe_evaluation_final
	0,  // wipe_gameend_final
	99, // wipe_intro_final (hardcoded)
	0,  // wipe_ending_final
	99, // wipe_cutscene_final (hardcoded)

	0,  // wipe_specinter_final
	0   // wipe_multinter_final
};

//--------------------------------------------------------------------------
//                        SCREEN WIPE PACKAGE
//--------------------------------------------------------------------------

boolean WipeInAction = false;
boolean WipeRunPre = false;
boolean WipeRunPost = false;
boolean WipeDrawMenu = false;

UINT8 wipetype = 0;
UINT8 wipeframe = 0;

wipestyle_t wipestyle = WIPESTYLE_UNDEFINED;
wipestyleflags_t wipestyleflags = WSF_CROSSFADE;
specialwipe_t ranspecialwipe = SPECIALWIPE_NONE;

static INT32 wipedefindex = 0;

#ifndef NOWIPE
static UINT8 *wipe_scr_start; //screen 3
static UINT8 *wipe_scr_end; //screen 4
static UINT8 *wipe_scr; //screen 0 (main drawing)
static fixed_t paldiv = 0;

/** Create fademask_t from lump
  *
  * \param	lump	Lump name to get data from
  * \return	fademask_t for lump
  */
static fademask_t *F_GetFadeMask(UINT8 masknum, UINT8 scrnnum) {
	static char lumpname[10] = "FADEmmss";
	static fademask_t fm = {NULL,0,0,0,0,0};
	lumpnum_t lumpnum;
	UINT8 *lump, *mask;
	size_t lsize;
	RGBA_t *pcolor;

	if (masknum > 99 || scrnnum > 99)
		goto freemask;

	sprintf(&lumpname[4], "%.2hu%.2hu", (UINT16)masknum, (UINT16)scrnnum);

	lumpnum = W_CheckNumForName(lumpname);
	if (lumpnum == LUMPERROR)
		goto freemask;

	lump = W_CacheLumpNum(lumpnum, PU_CACHE);
	lsize = W_LumpLength(lumpnum);
	switch (lsize)
	{
		case 256000: // 640x400
			fm.width = 640;
			fm.height = 400;
			break;
		case 64000: // 320x200
			fm.width = 320;
			fm.height = 200;
			break;
		case 16000: // 160x100
			fm.width = 160;
			fm.height = 100;
			break;
		case 4000: // 80x50 (minimum)
			fm.width = 80;
			fm.height = 50;
			break;

		default: // bad lump
			CONS_Alert(CONS_WARNING, "Fade mask lump %s of incorrect size, ignored\n", lumpname);
		case 0: // end marker (not bad!, but still need clearing)
			goto freemask;
	}
	if (lsize != fm.size)
		fm.mask = Z_Realloc(fm.mask, lsize, PU_STATIC, NULL);
	fm.size = lsize;

	mask = fm.mask;

	while (lsize--)
	{
		// Determine pixel to use from fademask
		pcolor = &pMasterPalette[*lump++];
		if (wipestyle == WIPESTYLE_COLORMAP)
			*mask++ = pcolor->s.red / FADECOLORMAPDIV;
		else
			*mask++ = FixedDiv((pcolor->s.red+1)<<FRACBITS, paldiv)>>FRACBITS;
	}

	fm.xscale = FixedDiv(vid.width<<FRACBITS, fm.width<<FRACBITS);
	fm.yscale = FixedDiv(vid.height<<FRACBITS, fm.height<<FRACBITS);
	return &fm;

	// Landing point for freeing data -- do this instead of just returning NULL
	// this ensures the fade data isn't remaining in memory, unused
	// (could be up to 256,000 bytes if it's a HQ fade!)
	freemask:
	if (fm.mask)
	{
		Z_Free(fm.mask);
		fm.mask = NULL;
		fm.size = 0;
	}

	return NULL;
}

/**	Wipe ticker
  *
  * \param	fademask	pixels to change
  */
static void F_DoWipe(fademask_t *fademask)
{
	// Software mask wipe -- optimized; though it might not look like it!
	// Okay, to save you wondering *how* this is more optimized than the simpler
	// version that came before it...
	// ---
	// The previous code did two FixedMul calls for every single pixel on the
	// screen, of which there are hundreds of thousands -- if not millions -- of.
	// This worked fine for smaller screen sizes, but with excessively large
	// (1920x1200) screens that meant 4 million+ calls out to FixedMul, and that
	// would take /just/ long enough that fades would start to noticably lag.
	// ---
	// This code iterates over the fade mask's pixels instead of the screen's,
	// and deals with drawing over each rectangular area before it moves on to
	// the next pixel in the fade mask.  As a result, it's more complex (and might
	// look a little messy; sorry!) but it simultaneously runs at twice the speed.
	// In addition, we precalculate all the X and Y positions that we need to draw
	// from and to, so it uses a little extra memory, but again, helps it run faster.
	{
		// wipe screen, start, end
		UINT8       *w = wipe_scr;
		const UINT8 *s = wipe_scr_start;
		const UINT8 *e = wipe_scr_end;

		// first pixel for each screen
		UINT8       *w_base = w;
		const UINT8 *s_base = s;
		const UINT8 *e_base = e;

		// mask data, end
		UINT8       *transtbl;
		const UINT8 *mask    = fademask->mask;
		const UINT8 *maskend = mask + fademask->size;

		// rectangle draw hints
		UINT32 draw_linestart, draw_rowstart;
		UINT32 draw_lineend,   draw_rowend;
		UINT32 draw_linestogo, draw_rowstogo;

		// rectangle coordinates, etc.
		UINT16* scrxpos = (UINT16*)malloc((fademask->width + 1)  * sizeof(UINT16));
		UINT16* scrypos = (UINT16*)malloc((fademask->height + 1) * sizeof(UINT16));
		UINT16 maskx, masky;
		UINT32 relativepos;

		// ---
		// Screw it, we do the fixed point math ourselves up front.
		scrxpos[0] = 0;
		for (relativepos = 0, maskx = 1; maskx < fademask->width; ++maskx)
			scrxpos[maskx] = (relativepos += fademask->xscale)>>FRACBITS;
		scrxpos[fademask->width] = vid.width;

		scrypos[0] = 0;
		for (relativepos = 0, masky = 1; masky < fademask->height; ++masky)
			scrypos[masky] = (relativepos += fademask->yscale)>>FRACBITS;
		scrypos[fademask->height] = vid.height;
		// ---

		maskx = masky = 0;
		do
		{
			draw_rowstart = scrxpos[maskx];
			draw_rowend   = scrxpos[maskx + 1];
			draw_linestart = scrypos[masky];
			draw_lineend   = scrypos[masky + 1];

			relativepos = (draw_linestart * vid.width) + draw_rowstart;
			draw_linestogo = draw_lineend - draw_linestart;

			if (*mask == 0)
			{
				// shortcut - memcpy source to work
				while (draw_linestogo--)
				{
					M_Memcpy(w_base+relativepos, s_base+relativepos, draw_rowend-draw_rowstart);
					relativepos += vid.width;
				}
			}
			else if (*mask >= 10)
			{
				// shortcut - memcpy target to work
				while (draw_linestogo--)
				{
					M_Memcpy(w_base+relativepos, e_base+relativepos, draw_rowend-draw_rowstart);
					relativepos += vid.width;
				}
			}
			else
			{
				// pointer to transtable that this mask would use
				transtbl = transtables + ((9 - *mask)<<FF_TRANSSHIFT);

				// DRAWING LOOP
				while (draw_linestogo--)
				{
					w = w_base + relativepos;
					s = s_base + relativepos;
					e = e_base + relativepos;
					draw_rowstogo = draw_rowend - draw_rowstart;

					while (draw_rowstogo--)
						*w++ = transtbl[ ( *e++ << 8 ) + *s++ ];

					relativepos += vid.width;
				}
				// END DRAWING LOOP
			}

			if (++maskx >= fademask->width)
				++masky, maskx = 0;
		} while (++mask < maskend);

		free(scrxpos);
		free(scrypos);
	}
}

static void F_DoColormapWipe(fademask_t *fademask, UINT8 *colormap)
{
	// Lactozilla: F_DoWipe for WIPESTYLE_COLORMAP
	{
		// wipe screen, start, end
		UINT8       *w = wipe_scr;
		const UINT8 *s = wipe_scr_start;
		const UINT8 *e = wipe_scr_end;

		// first pixel for each screen
		UINT8       *w_base = w;
		const UINT8 *s_base = s;
		const UINT8 *e_base = e;

		// mask data, end
		UINT8       *transtbl;
		const UINT8 *mask    = fademask->mask;
		const UINT8 *maskend = mask + fademask->size;

		// rectangle draw hints
		UINT32 draw_linestart, draw_rowstart;
		UINT32 draw_lineend,   draw_rowend;
		UINT32 draw_linestogo, draw_rowstogo;

		// rectangle coordinates, etc.
		UINT16* scrxpos = (UINT16*)malloc((fademask->width + 1)  * sizeof(UINT16));
		UINT16* scrypos = (UINT16*)malloc((fademask->height + 1) * sizeof(UINT16));
		UINT16 maskx, masky;
		UINT32 relativepos;

		// ---
		// Screw it, we do the fixed point math ourselves up front.
		scrxpos[0] = 0;
		for (relativepos = 0, maskx = 1; maskx < fademask->width; ++maskx)
			scrxpos[maskx] = (relativepos += fademask->xscale)>>FRACBITS;
		scrxpos[fademask->width] = vid.width;

		scrypos[0] = 0;
		for (relativepos = 0, masky = 1; masky < fademask->height; ++masky)
			scrypos[masky] = (relativepos += fademask->yscale)>>FRACBITS;
		scrypos[fademask->height] = vid.height;
		// ---

		maskx = masky = 0;
		do
		{
			draw_rowstart = scrxpos[maskx];
			draw_rowend   = scrxpos[maskx + 1];
			draw_linestart = scrypos[masky];
			draw_lineend   = scrypos[masky + 1];

			relativepos = (draw_linestart * vid.width) + draw_rowstart;
			draw_linestogo = draw_lineend - draw_linestart;

			if (*mask == 0)
			{
				// shortcut - memcpy source to work
				while (draw_linestogo--)
				{
					M_Memcpy(w_base+relativepos, s_base+relativepos, draw_rowend-draw_rowstart);
					relativepos += vid.width;
				}
			}
			else if (*mask >= FADECOLORMAPROWS)
			{
				// shortcut - memcpy target to work
				while (draw_linestogo--)
				{
					M_Memcpy(w_base+relativepos, e_base+relativepos, draw_rowend-draw_rowstart);
					relativepos += vid.width;
				}
			}
			else
			{
				int nmask = *mask;
				if (wipestyleflags & WSF_FADEIN)
					nmask = (FADECOLORMAPROWS-1) - nmask;

				transtbl = colormap + (nmask * 256);

				// DRAWING LOOP
				while (draw_linestogo--)
				{
					w = w_base + relativepos;
					s = s_base + relativepos;
					e = e_base + relativepos;
					draw_rowstogo = draw_rowend - draw_rowstart;

					while (draw_rowstogo--)
						*w++ = transtbl[*e++];

					relativepos += vid.width;
				}
				// END DRAWING LOOP
			}

			if (++maskx >= fademask->width)
				++masky, maskx = 0;
		} while (++mask < maskend);

		free(scrxpos);
		free(scrypos);
	}
}
#endif

/** Save the "before" screen of a wipe.
  */
void F_WipeStartScreen(void)
{
#ifndef NOWIPE
#ifdef HWRENDER
	if(rendermode != render_soft)
	{
		HWR_StartScreenWipe();
		return;
	}
#endif
	wipe_scr_start = screens[3];
	I_ReadScreen(wipe_scr_start);
#endif
}

/** Save the "after" screen of a wipe.
  */
void F_WipeEndScreen(void)
{
#ifndef NOWIPE
#ifdef HWRENDER
	if(rendermode != render_soft)
	{
		HWR_EndScreenWipe();
		return;
	}
#endif
	wipe_scr_end = screens[4];
	I_ReadScreen(wipe_scr_end);
#endif
}

/** Verifies every condition for a colormapped fade.
  */
boolean F_ShouldColormapFade(void)
{
#ifndef NOWIPE
	if ((wipestyleflags & (WSF_FADEIN|WSF_FADEOUT)) // only if one of those wipestyleflags are actually set
	&& !(wipestyleflags & WSF_CROSSFADE)) // and if not crossfading
	{
		// World
		return (gamestate == GS_LEVEL
		|| gamestate == GS_TITLESCREEN
		// Finales
		|| gamestate == GS_CONTINUING
		|| gamestate == GS_CREDITS
		|| gamestate == GS_EVALUATION
		|| gamestate == GS_INTRO
		|| gamestate == GS_ENDING
		// Menus
		|| gamestate == GS_TIMEATTACK);
	}
#endif
	return false;
}

/** Decides what wipe style to use.
  */
void F_DecideWipeStyle(void)
{
	// Set default wipe style
	wipestyle = WIPESTYLE_NORMAL;

	// Check for colormap wipe style
	if (F_ShouldColormapFade())
		wipestyle = WIPESTYLE_COLORMAP;
}

/** Attempt to run a colormap fade,
    provided all the conditions were properly met.
    Returns true if so.
    Call F_StartWipe after this function.
  */
boolean F_TryColormapFade(UINT8 wipecolor)
{
#ifndef NOWIPE
	if (F_ShouldColormapFade())
	{
#ifdef HWRENDER
		if (rendermode == render_opengl)
			F_WipeColorFill(wipecolor);
#endif
		return true;
	}
	else
#endif
	{
		F_WipeColorFill(wipecolor);
		return false;
	}
}

/** After setting up the screens you want to wipe,
  * calling this will do a 'typical' wipe.
  */
void F_StartWipe(UINT8 type, boolean drawMenu)
{
#ifdef NOWIPE
	(void)type;
	(void)drawMenu;
#else
	if (!paldiv)
		paldiv = FixedDiv(257<<FRACBITS, 11<<FRACBITS);

	// Init the wipe
	if (wipestyle == WIPESTYLE_UNDEFINED)
		F_DecideWipeStyle();

	WipeInAction = true;
	WipeDrawMenu = (!drawMenu);

	wipetype = type;
	wipeframe = 0;
#endif
}

/** Runs the current wipe.
  */
void F_RunWipe(void)
{
#ifndef NOWIPE
	fademask_t *fmask = F_GetFadeMask(wipetype, wipeframe);
	if (!fmask)
	{
		F_StopWipe();
		return;
	}

	wipeframe++;
#else
	F_StopWipe();
#endif
}

/** Stops running the current wipe.
  */
void F_StopWipe(void)
{
	WipeInAction = false;
	WipeDrawMenu = false;

	if (titlecard.wipe)
		titlecard.wipe = 0;

	if (wipestyleflags & WSF_SPECIALSTAGE)
	{
#ifndef NOWIPE
		tic_t starttime = I_GetTime(), lasttime = starttime;
		tic_t endtime = starttime + (3*TICRATE)/2;
		tic_t nowtime;

		// Hold on white for extra effect.
		while (nowtime < endtime)
		{
			while (!((nowtime = I_GetTime()) - lasttime))
				I_Sleep();
			lasttime = nowtime;

			I_OsPolling();
			if (moviemode) // make sure we save frames for the white hold too
				M_SaveFrame();
		}
#endif
		wipestyleflags &= ~WSF_SPECIALSTAGE;
	}

	if (wipestyleflags & WSF_LEVELLOADING)
	{
		G_DoLoadLevel();
		wipestyleflags &= ~WSF_LEVELLOADING;
	}

	wipestyle = WIPESTYLE_UNDEFINED;
}

#ifndef NOWIPE
/** Renders the current wipe into wipe_scr.
  */
static void F_RenderWipe(fademask_t *fmask)
{
	switch (wipestyle)
	{
		case WIPESTYLE_COLORMAP:
#ifdef HWRENDER
			if (rendermode == render_opengl)
			{
				// send in the wipe type and wipe frame because we need to cache the graphic
				HWR_DoTintedWipe(wipetype, wipeframe-1);
			}
			else
#endif
			{
				UINT8 *colormap = fadecolormap;
				if (wipestyleflags & WSF_TOWHITE)
					colormap += (FADECOLORMAPROWS * 256);
				F_DoColormapWipe(fmask, colormap);
			}
			break;
		case WIPESTYLE_NORMAL:
#ifdef HWRENDER
			if (rendermode == render_opengl)
			{
				// send in the wipe type and wipe frame because we need to cache the graphic
				HWR_DoWipe(wipetype, wipeframe-1);
			}
			else
#endif
				F_DoWipe(fmask);
			break;
		default:
			break;
	}
}
#endif

/** Displays the current wipe.
  */
void F_DisplayWipe(void)
{
#ifndef NOWIPE
	fademask_t *fmask;
	wipe_scr = screens[0];

	// get fademask first so we can tell if it exists or not
	fmask = F_GetFadeMask(wipetype, wipeframe);
	if (!fmask)
	{
		// Save screen for post-wipe
		if (WipeRunPre)
		{
			fmask = F_GetFadeMask(wipetype, wipeframe-1);
			WipeRunPre = false; // Disable post-wipe flag
			if (!fmask)
				return;
			else
			{
				F_RenderWipe(fmask);
				F_WipeStartScreen();
			}
		}
		return;
	}

	F_RenderWipe(fmask);
#endif
}

/** Starts the "pre" type of a wipe.
  */
void F_StartWipePre(void)
{
	// set for all later
	wipedefindex = gamestate; // wipe_xxx_toblack
	if (gamestate == GS_INTERMISSION)
	{
		if (intertype == int_spec) // Special Stage
			wipedefindex = wipe_specinter_toblack;
		else if (intertype != int_coop) // Multiplayer
			wipedefindex = wipe_multinter_toblack;
	}

	if (wipetypepre < 0 || !F_WipeExists(wipetypepre))
		wipetypepre = wipedefs[wipedefindex];

	if (rendermode != render_none)
	{
		WipeRunPre = true;
		// Fade to black first
		if ((wipegamestate == (gamestate_t)FORCEWIPE ||
				(wipegamestate != (gamestate_t)FORCEWIPEOFF
					&& !(gamestate == GS_LEVEL || (gamestate == GS_TITLESCREEN && titlemapinaction)))
				) // fades to black on its own timing, always
		 && wipetypepre != UINT8_MAX)
		{
			F_WipeStartScreen();

			// Check for Mega Genesis fade
			wipestyleflags = WSF_FADEOUT;
			if (wipegamestate == (gamestate_t)FORCEWIPE)
				F_WipeColorFill(31);
			else if (F_TryColormapFade(31))
				wipetypepost = DEFAULTWIPE; // Don't run the fade below this one

			F_WipeEndScreen();

			F_StartWipe(wipetypepre, gamestate != GS_TIMEATTACK && gamestate != GS_TITLESCREEN);
			WipeRunPost = true;
		}
	}

	wipetypepre = DEFAULTWIPE;
	wipegamestate = gamestate;
}

/** Starts the "post" type of a wipe.
  */
void F_StartWipePost(void)
{
	wipedefindex += WIPEFINALSHIFT;

	if (wipetypepost == DEFAULTWIPE || !F_WipeExists(wipetypepost))
		wipetypepost = wipedefs[wipedefindex];

	if (rendermode != render_none)
	{
		F_WipeEndScreen();

		// Check for Mega Genesis fade
		if (F_ShouldColormapFade())
		{
			wipestyleflags |= WSF_FADEIN;
			wipestyleflags &= ~(WSF_FADEOUT|WSF_LEVELLOADING|WSF_SPECIALSTAGE);
		}

		F_StartWipe(wipetypepost, gamestate != GS_TIMEATTACK && gamestate != GS_TITLESCREEN);
	}

	// reset counters so timedemo doesn't count the wipe duration
	if (timingdemo)
	{
		framecount = 0;
		demostarttime = I_GetTime();
	}

	wipetypepost = DEFAULTWIPE;
	WipeRunPost = false;
}

/** Returns tic length of wipe
  * One lump equals one tic
  */
tic_t F_GetWipeLength(UINT8 type)
{
#ifdef NOWIPE
	(void)type;
	return 0;
#else
	static char lumpname[10] = "FADEmmss";
	lumpnum_t lumpnum;
	UINT8 wipeframe;

	if (type > 99)
		return 0;

	for (wipeframe = 0; wipeframe < 100; wipeframe++)
	{
		sprintf(&lumpname[4], "%.2hu%.2hu", (UINT16)type, (UINT16)wipeframe);

		lumpnum = W_CheckNumForName(lumpname);
		if (lumpnum == LUMPERROR)
			return --wipeframe;
	}
	return --wipeframe;
#endif
}

/** Does the specified wipe exist?
  */
boolean F_WipeExists(UINT8 type)
{
#ifdef NOWIPE
	(void)type;
	return false;
#else
	static char lumpname[10] = "FADEmm00";
	lumpnum_t lumpnum;

	if (type > 99)
		return false;

	sprintf(&lumpname[4], "%.2hu00", (UINT16)type);

	lumpnum = W_CheckNumForName(lumpname);
	return !(lumpnum == LUMPERROR);
#endif
}
