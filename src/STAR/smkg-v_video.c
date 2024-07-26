// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-v_video.c
/// \brief TSoURDt3rd's exclusive video routines

#include "../v_video.h"
#include "../hu_stuff.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_V_DrawLevelTitleAtFixed(fixed_t x, fixed_t y, INT32 option, const char *string)
// Draws a level-title-like string at the fixed_t location given.
//
void TSoURDt3rd_V_DrawLevelTitleAtFixed(fixed_t x, fixed_t y, INT32 option, const char *string)
{
	fixed_t cx = x, cy = y;
	INT32 w, c, dupx, dupy, scrwidth, left = 0;
	const char *ch = string;
	INT32 charflags = (option & V_CHARCOLORMASK);
	const UINT8 *colormap = NULL;

	option &= ~V_FLIP; // which is also shared with V_ALLOWLOWERCASE...

	if (option & V_NOSCALESTART)
	{
		dupx = vid.dupx;
		dupy = vid.dupy;
		scrwidth = vid.width;
	}
	else
	{
		dupx = dupy = 1;
		scrwidth = vid.width/vid.dupx;
		left = (scrwidth - BASEVIDWIDTH)/2;
		scrwidth -= left;
	}

	if (option & V_NOSCALEPATCH)
		scrwidth *= vid.dupx;

	for (;;ch++)
	{
		if (!*ch)
			break;
		if (*ch & 0x80) //color parsing -x 2.16.09
		{
			// manually set flags override color codes
			if (!(option & V_CHARCOLORMASK))
				charflags = ((*ch & 0x7f) << V_CHARCOLORSHIFT) & V_CHARCOLORMASK;
			continue;
		}
		if (*ch == '\n')
		{
			cx = x;
			cy += (12*dupy)<<FRACBITS;
			continue;
		}

		c = *ch - LT_FONTSTART;

		// character does not exist or is a space
		if (c < 0 || c >= LT_FONTSIZE || !lt_font[c])
		{
			cx += (4*dupx)<<FRACBITS;
			continue;
		}

		w = lt_font[c]->width * dupx;

		if ((cx>>FRACBITS) > scrwidth)
			continue;
		if ((cx>>FRACBITS)+left + w < 0) //left boundary check
		{
			cx += w<<FRACBITS;
			continue;
		}

		colormap = V_GetStringColormap(charflags);
		V_DrawFixedPatch(cx, cy, FRACUNIT, option, lt_font[c], colormap);

		cx += w<<FRACBITS;
	}
}
