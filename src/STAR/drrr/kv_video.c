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
/// \file  kv_video.c
/// \brief Gamma correction LUT stuff
///        Functions to draw patches (by post) directly to screen.
///        Functions to blit a block to the screen.

#include "../../doomdef.h"
#include "../../v_video.h"
#include "kv_video.h"
#include "kv_draw.h"
#include "k_font.h"
#include "../../z_zone.h"

static inline fixed_t FixedCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)scale;
	(void)hchw;
	(void)dupx;
	(*cwp) = chw;
	return 0;
}

static inline fixed_t VariableCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul ((*cwp) << FRACBITS, scale);
	return 0;
}

static inline fixed_t CenteredCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	INT32 cxoff;
	/*
	For example, center a 4 wide patch to 8 width:
	4/2   = 2
	8/2   = 4
	4 - 2 = 2 (our offset)
	2 + 4 = 6 = 8 - 2 (equal space on either side)
	*/
	cxoff  = hchw -((*cwp) >> 1 );
	(*cwp) = chw;
	return FixedMul (( cxoff * dupx )<< FRACBITS, scale);
}

static inline fixed_t BunchedCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul((INT32)max(1, (*cwp) - 1) << FRACBITS, scale);
	return 0;
}

static inline fixed_t MenuCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul((INT32)max(1, (*cwp) - 2) << FRACBITS, scale);
	return 0;
}

static inline fixed_t GamemodeCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul((INT32)max(1, (*cwp) - 2) << FRACBITS, scale);
	return 0;
}

static inline fixed_t FileCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul((INT32)max(1, (*cwp) - 3) << FRACBITS, scale);
	return 0;
}

static inline fixed_t LSTitleCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul((INT32)max(1, (*cwp) - 4) << FRACBITS, scale);
	return 0;
}

typedef struct
{
	fixed_t    chw;
	fixed_t spacew;
	fixed_t    lfh;
	fixed_t (*dim_fn)(fixed_t,fixed_t,INT32,INT32,fixed_t *);
	UINT8 button_yofs;
} fontspec_t;

static void V_GetFontSpecification(int fontno, INT32 flags, fontspec_t *result)
{
	/*
	Hardcoded until a better system can be implemented
	for determining how fonts space.
	*/

	// All other properties are guaranteed to be set
	result->chw = 0;
	result->button_yofs = 0;

	const INT32 spacing = ( flags & V_SPACINGMASK );

	switch (fontno)
	{
		default:
		case HU_FONT:
		case MENU_FONT:
			result->spacew = 4;
			switch (spacing)
			{
				case V_MONOSPACE:
					result->spacew = 8;
					/* FALLTHRU */
				case V_OLDSPACING:
					result->chw    = 8;
					break;
				case V_6WIDTHSPACE:
					result->spacew = 6;
					break;
			}
			break;
		case TINY_FONT:
		case TINYTIMER_FONT:
			result->spacew = 2;
			switch (spacing)
			{
				case V_MONOSPACE:
					result->spacew = 5;
					/* FALLTHRU */
				case V_OLDSPACING:
					result->chw    = 5;
					break;
				case V_6WIDTHSPACE:
					result->spacew = 3;
					break;
			}
			break;
		case MED_FONT:
			result->chw    = 6;
			result->spacew = 6;
			break;
		case LT_FONT:
			result->spacew = 12;
			break;
		case CRED_FONT:
			result->spacew = 16;
			break;
		case KART_FONT:
			result->spacew = 3;
			switch (spacing)
			{
				case V_MONOSPACE:
					result->spacew = 12;
					/* FALLTHRU */
				case V_OLDSPACING:
					result->chw    = 12;
					break;
				case V_6WIDTHSPACE:
					result->spacew = 6;
			}
			break;
		case GM_FONT:
			result->spacew = 6;
			break;
		case FILE_FONT:
			result->spacew = 0;
			break;
		case LSHI_FONT:
		case LSLOW_FONT:
			result->spacew = 10;
			break;
		case OPPRF_FONT:
			result->spacew = 5;
			break;
		case PINGF_FONT:
			result->spacew = 3;
			break;
		case ROLNUM_FONT:
			result->spacew = 17;
			break;
		case RO4NUM_FONT:
			result->spacew = 9;
			break;
	}

	switch (fontno)
	{
		default:
		case HU_FONT:
		case MENU_FONT:
		case TINY_FONT:
		case TINYTIMER_FONT:
		case KART_FONT:
		case MED_FONT:
			result->lfh = 12;
			break;
		case LT_FONT:
		case CRED_FONT:
		case FILE_FONT:
			result->lfh    = 12;
			break;
		case GM_FONT:
			result->lfh    = 32;
			break;
		case LSHI_FONT:
			result->lfh    = 56;
			break;
		case LSLOW_FONT:
			result->lfh    = 38;
			break;
		case OPPRF_FONT:
		case PINGF_FONT:
			result->lfh = 10;
			break;
		case ROLNUM_FONT:
			result->lfh = 33;
			break;
		case RO4NUM_FONT:
			result->lfh = 15;
			break;
	}

	switch (fontno)
	{
		default:
			if (result->chw)
				result->dim_fn = CenteredCharacterDim;
			else
				result->dim_fn = VariableCharacterDim;
			break;
		case HU_FONT:
			if (result->chw)
				result->dim_fn = CenteredCharacterDim;
			else
				result->dim_fn = BunchedCharacterDim;
			break;
		case MENU_FONT:
			if (result->chw)
				result->dim_fn = CenteredCharacterDim;
			else
				result->dim_fn = MenuCharacterDim;
			break;
		case KART_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = BunchedCharacterDim;
			break;
		case TINY_FONT:
		case TINYTIMER_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = BunchedCharacterDim;
			break;
		case MED_FONT:
			result->dim_fn = FixedCharacterDim;
			break;
		case GM_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = GamemodeCharacterDim;
			break;
		case FILE_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = FileCharacterDim;
			break;
		case LSHI_FONT:
		case LSLOW_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = LSTitleCharacterDim;
			break;
		case OPPRF_FONT:
		case PINGF_FONT:
		case ROLNUM_FONT:
		case RO4NUM_FONT:
			if (result->chw)
				result->dim_fn = FixedCharacterDim;
			else
				result->dim_fn = BunchedCharacterDim;
			break;
	}

	switch (fontno)
	{
	case HU_FONT:
		result->button_yofs = 2;
		break;
	case MENU_FONT:
		result->button_yofs = 1;
		break;
	}
}

static UINT8 V_GetButtonCodeWidth(UINT8 c)
{
	UINT8 x = 0;

	switch (c & 0x0F)
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
		// arrows
		x = 12;
		break;

	case 0x07:
	case 0x08:
	case 0x09:
		// shoulders, start
		x = 14;
		break;

	case 0x04:
		// dpad
		x = 14;
		break;

	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F:
		// faces
		x = 10;
		break;
	}

	return x;
}

static boolean V_CharacterValid(font_t *font, int c)
{
	return (c >= 0 && c < font->size && font->font[c] != NULL);
}

// Modify a string to wordwrap at any given width.
char * V_ScaledWordWrap(
		fixed_t          w,
		fixed_t      scale,
		fixed_t spacescale,
		fixed_t    lfscale,
		INT32      flags,
		int        fontno,
		const char *s)
{
	INT32     hchw;/* half-width for centering */

	INT32     dupx;

	font_t   *font;

	boolean uppercase;

	fixed_t cx;
	fixed_t right;

	fixed_t cw;

	int c;

	uppercase  = ((flags & V_ALLOWLOWERCASE) != V_ALLOWLOWERCASE);
	flags	&= ~(V_FLIP);/* These two (V_ALLOWLOWERCASE) share a bit. */

	font       = &fontv[fontno];

	fontspec_t fontspec;

	V_GetFontSpecification(fontno, flags, &fontspec);

	hchw     = fontspec.chw >> 1;

	fontspec.chw    <<= FRACBITS;
	fontspec.spacew <<= FRACBITS;

#define Mul( id, scale ) ( id = FixedMul (scale, id) )
	Mul    (fontspec.chw,      scale);
	Mul (fontspec.spacew,      scale);
	Mul    (fontspec.lfh,      scale);

	Mul (fontspec.spacew, spacescale);
	Mul    (fontspec.lfh,    lfscale);
#undef  Mul

	if (( flags & V_NOSCALESTART ))
	{
		dupx      = vid.dupx;

		hchw     *=     dupx;

		fontspec.chw      *=     dupx;
		fontspec.spacew   *=     dupx;
		fontspec.lfh      *= vid.dupy;
	}
	else
	{
		dupx      = 1;
	}

	cx = 0;
	right = 0;

	size_t reader = 0, writer = 0, startwriter = 0;
	fixed_t cxatstart = 0;

	size_t len = strlen(s) + 1;
	size_t potentialnewlines = 8;
	size_t sparenewlines = potentialnewlines;

	char *newstring = (char *)Z_Malloc(len + sparenewlines, PU_STATIC, NULL);

	for (; ( c = s[reader] ); ++reader, ++writer)
	{
		newstring[writer] = s[reader];

		right = 0;

		switch (c)
		{
			case '\n':
				cx  =   0;
				cxatstart = 0;
				startwriter = 0;
				break;
			default:
				if (( c & 0xF0 ) == 0x80 || c == V_STRINGDANCE)
					;
				else if (( c & 0xB0 ) & 0x80) // button prompts
				{
					cw = V_GetButtonCodeWidth(c) * dupx;
					cx += cw * scale;
					right = cx;
				}
				else
				{
					if (uppercase)
					{
						c = toupper(c);
					}
					else if (V_CharacterValid(font, c - font->start) == false)
					{
						// Try the other case if it doesn't exist
						if (c >= 'A' && c <= 'Z')
						{
							c = tolower(c);
						}
						else if (c >= 'a' && c <= 'z')
						{
							c = toupper(c);
						}
					}

					c -= font->start;
					if (V_CharacterValid(font, c) == true)
					{
						cw = SHORT (font->font[c]->width) * dupx;

						// How bunched dims work is by incrementing cx slightly less than a full character width.
						// This causes the next character to be drawn overlapping the previous.
						// We need to count the full width to get the rightmost edge of the string though.
						right = cx + (cw * scale);

						(*fontspec.dim_fn)(scale, fontspec.chw, hchw, dupx, &cw);
						cx += cw;
					}
					else
					{
						cx += fontspec.spacew;
						cxatstart = cx;
						startwriter = writer;
					}
				}
		}

		// Start trying to wrap if presumed length exceeds the space we have on-screen.
		if (right && right > w)
		{
			if (startwriter != 0)
			{
				newstring[startwriter] = '\n';
				cx -= cxatstart;
				cxatstart = 0;
				startwriter = 0;
			}
			else
			{
				if (sparenewlines == 0)
				{
					sparenewlines = (potentialnewlines *= 2);
					newstring = (char *)Z_Realloc(newstring, len + sparenewlines, PU_STATIC, NULL);
				}

				sparenewlines--;
				len++;

				newstring[writer++] = '\n'; // Over-write previous
				cx = cw; // Valid value in the only case right is currently set
				newstring[writer] = s[reader]; // Re-add
			}
		}
	}

	newstring[writer] = '\0';

	return newstring;
}
