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

#ifdef __cplusplus
extern "C" {
#endif

#define V_STRINGDANCE        0x00000002 // (strings/characters only) funny undertale

// draw a string using a font
char * V_ScaledWordWrap(
		fixed_t          w,
		fixed_t      scale,
		fixed_t spacescale,
		fixed_t    lfscale,
		INT32      flags,
		int        fontno,
		const char *s);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KV_VIDEO__
