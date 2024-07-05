// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  khu_stuff.c
/// \brief Heads up display

#include "khu_stuff.h"
#include "kk_hud.h"
#include "k_font.h"
#include "../../hu_stuff.h"
#include "../../r_patch.h"
#include "../../w_wad.h"
#include "../../z_zone.h"

//-------------------------------------------
//              misc vars
//-------------------------------------------

patch_t *missingpat;

#if 1
// STAR NOTE: meant to be in p_setup.c but needs to be in order for vanilla compatiblility :p //
UINT16 partadd_earliestfile = UINT16_MAX;
#endif

//======================================================================
//                          HEADS UP INIT
//======================================================================

// Initialise Heads up
// once at game startup.
//
void DRRR_HU_Init(void)
{
	font_t font;

	// only allocate if not present, to save us a lot of headache
	if (missingpat == NULL)
	{
		lumpnum_t missingnum = W_GetNumForName("MISSING");
		if (missingnum == LUMPERROR)
			I_Error("DRRR_HU_LoadGraphics: \"MISSING\" patch not present in resource files.");

		missingpat = W_CachePatchNum(missingnum, PU_STATIC);
	}

	/*
	Setup fonts
	*/

	if (!dedicated)
	{
#define  DIM( s, n ) ( font.start = s, font.size = n )
#define ADIM( name )        DIM (name ## _FONTSTART, name ## _FONTSIZE)
#define   PR( s )           strcpy(font.prefix, s)
#define  DIG( n )           ( font.digits = n )
#define  REG                Font_DumbRegister(&font)

		DIG  (3);

		ADIM (HU);
		PR   ("STCFN");
		REG;

		PR   ("MNUFN");
		REG;

		PR   ("TNYFN");
		REG;

		PR   ("MDFN");
		REG;

		PR   ("FILEF");
		REG;

		ADIM (LT);
		PR   ("LTFNT");
		REG;

		ADIM (CRED);
		PR   ("CRFNT");
		REG;

		DIG  (3);

		ADIM (LT);

		PR   ("GTOL");
		REG;

		PR   ("GTFN");
		REG;

		PR   ("4GTOL");
		REG;

		PR   ("4GTFN");
		REG;

		DIG  (1);

		DIM  (0, 10);

		PR   ("STTNUM");
		REG;

		PR   ("NGTNUM");
		REG;

		PR   ("PINGN");
		REG;

		PR   ("PRFN");
		REG;

		DIM  ('0', 10);
		DIG  (2);

		PR   ("ROLNUM");
		REG;

		PR   ("RO4NUM");
		REG;

		DIG  (3);

#if 0
		ADIM (KART);
		PR   ("MKFNT");
		REG;

		ADIM (NUM);
		PR   ("TMFNT");
		REG;

		PR   ("TMFNS");
		REG;
#endif

		ADIM (LT);
		PR   ("GAMEM");
		REG;

		ADIM (LT);
		PR   ("THIFN");
		REG;

		PR   ("TLWFN");
		REG;

#if 0
		ADIM (NUM);
		PR   ("OPPRF");
		REG;

		ADIM (NUM);
		PR   ("PINGF");
		REG;
#endif

#undef  REG
#undef  DIG
#undef  PR
#undef  ADMIN
#undef  DIM
	}
}

patch_t *HU_UpdateOrBlankPatch(patch_t **user, boolean required, const char *format, ...)
{
	va_list ap;
	char buffer[9];

	lumpnum_t lump = INT16_MAX;
	patch_t *patch;

	va_start (ap, format);
	vsnprintf(buffer, sizeof buffer, format, ap);
	va_end   (ap);

	if (user && partadd_earliestfile != UINT16_MAX)
	{
		UINT16 fileref = numwadfiles;
		lump = INT16_MAX;

		while ((lump == INT16_MAX) && ((--fileref) >= partadd_earliestfile))
		{
			lump = W_CheckNumForNamePwad(buffer, fileref, 0);
		}

		/* no update in this wad */
		if (fileref < partadd_earliestfile)
			return *user;

		lump |= (fileref << 16);
	}
	else
	{
		lump = W_CheckNumForName(buffer);

		if (lump == LUMPERROR)
		{
			if (required == true)
				*user = missingpat;

			return *user;
		}
	}

	patch = W_CachePatchNum(lump, PU_HUDGFX);

	if (user)
	{
		if (*user)
			Patch_Free(*user);

		*user = patch;
	}

	return patch;
}
