// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
// Copyright (C) 2024-2025 by Kart Krew.
// Copyright (C) 2020-2025 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-r_main.h
/// \brief TSoURDt3rd's additional rendering variables, consvars, defines

#ifndef __SMKG_R_MAIN__
#define __SMKG_R_MAIN__

#include "../../r_main.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// POV related.
//

// Same as rendertimefrac but not suspended when the game is paused
// Ported from Dr.Robotnik's Ring Racers!
extern fixed_t rendertimefrac_unpaused;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_R_MAIN__
