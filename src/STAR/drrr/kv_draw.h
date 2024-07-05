// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by James Robert Roman
// Copyright (C) 2024 by Kart Krew
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------

#ifndef __KV_DRAW__
#define __KV_DRAW__

typedef enum
{
	TINY_FONT, // Font::kThin
	GM_FONT, // Font::kGamemode
	HU_FONT, // Font::kConsole
	KART_FONT, // Font::kFreeplay
	OPPRF_FONT, // Font::kZVote
	PINGF_FONT, // Font::kPing
	TIMER_FONT, // Font::kTimer
	TINYTIMER_FONT, // Font::kThinTimer
	MENU_FONT, // Font::kMenu
	MED_FONT, // Font::kMedium
	ROLNUM_FONT, // Font::kRollingNum
	RO4NUM_FONT, // Font::kRollingNum4P

	LSHI_FONT,
	LSLOW_FONT,

	LT_FONT,
	CRED_FONT,
	FILE_FONT,
} font_e;

#endif // __KV_DRAW__
