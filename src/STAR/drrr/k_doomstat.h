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
/// \file  k_doomstat.h
/// \brief All the global variables that store the internal state.
///
///        Theoretically speaking, the internal state of the engine
///        should be found by looking at the variables collected
///        here, and every relevant module will have to include
///        this header file. In practice... things are a bit messy.

#ifndef __K_DOOMSTAT__
#define __K_DOOMSTAT__

// We need globally shared data structures, for defining the global state variables.
#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

// =========================
// Status flags for refresh.
// =========================
//

extern INT32 window_x;
extern INT32 window_y;

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__K_DOOMSTAT__
