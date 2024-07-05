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
/// \file  khu_stuff.h
/// \brief Heads up display

#ifndef __KHU_STUFF_H__
#define __KHU_STUFF_H__

#include "../../r_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// init heads up data at game startup.
void DRRR_HU_Init(void);

// Load a HUDGFX patch or NULL/missingpat (dependent on required boolean).
patch_t *HU_UpdateOrBlankPatch(patch_t **user, boolean required, const char *format, ...);
//#define HU_CachePatch(...) HU_UpdateOrBlankPatch(NULL, false, __VA_ARGS__) -- not sure how to default the missingpat here plus not currently used
#define HU_UpdatePatch(user, ...) HU_UpdateOrBlankPatch(user, true, __VA_ARGS__)

#ifdef __cplusplus
} // extern "C"
#endif

/// STAR NOTE: earliest wadfile added ///
extern UINT16 partadd_earliestfile;

#endif // __KHU_STUFF_H__
