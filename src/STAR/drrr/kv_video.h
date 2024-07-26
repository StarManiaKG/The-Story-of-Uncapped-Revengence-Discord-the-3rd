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
/// \file  kv_video.h
/// \brief Gamma correction LUT

#ifndef __KV_VIDEO__
#define __KV_VIDEO__

#include "kv_draw.h"
#include "../../v_video.h"

#ifdef __cplusplus
extern "C" {
#endif

#define V_STRINGDANCE        0x00000002 // (strings/characters only) funny undertale

/* Convenience macros for leagacy string function macros. */
#define V__DrawOneScaleString( x,y,scale,option,cm,font,string ) \
	DRRR_V_DrawStringScaled(x,y,scale,FRACUNIT,FRACUNIT,option,cm,font,string)
#define V__DrawDupxString( x,y,scale,option,cm,font,string )\
	V__DrawOneScaleString ((x)<<FRACBITS,(y)<<FRACBITS,scale,option,cm,font,string)

#define V__IntegerStringWidth( scale,option,font,string ) \
	(DRRR_V_StringScaledWidth(scale,FRACUNIT,FRACUNIT,option,font,string) / FRACUNIT)

INT32 V_DanceYOffset(INT32 counter);

// draw a string using a font
void DRRR_V_DrawStringScaled(
		fixed_t     x,
		fixed_t     y,
		fixed_t           scale,
		fixed_t     space_scale,
		fixed_t  linefeed_scale,
		INT32       flags,
		const UINT8 *colormap,
		int         font,
		const char *text);

fixed_t DRRR_V_StringScaledWidth(
		fixed_t      scale,
		fixed_t spacescale,
		fixed_t    lfscale,
		INT32      flags,
		int        fontno,
		const char *s);

char * DRRR_V_ScaledWordWrap(
		fixed_t          w,
		fixed_t      scale,
		fixed_t spacescale,
		fixed_t    lfscale,
		INT32      flags,
		int        fontno,
		const char *s);

// draw a string using the hu_font
#define DRRR_V_DrawString( x,y,option,string ) \
	V__DrawDupxString (x,y,FRACUNIT,option,NULL,HU_FONT,string)

// draw a string using the tny_font
#define DRRR_V_DrawThinString( x,y,option,string ) \
	V__DrawDupxString (x,y,FRACUNIT,option,NULL,TINY_FONT,string)

#define DRRR_V_ThinStringWidth( string,option ) \
	V__IntegerStringWidth ( FRACUNIT,option,TINY_FONT,string )

void DRRR_V_DrawCenteredThinString(INT32 x, INT32 y, INT32 option, const char *string);

#define DRRR_V_DrawMenuString( x,y,option,string ) \
	V__DrawDupxString (x,y,FRACUNIT,option,NULL,MENU_FONT,string)

#define V_MenuStringWidth( string,option ) \
	V__IntegerStringWidth ( FRACUNIT,option,MENU_FONT,string )

#define DRRR_V_DrawGamemodeString( x,y,option,cm,string ) \
	V__DrawDupxString (x,y,FRACUNIT,option,cm,GM_FONT,string)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KV_VIDEO__
