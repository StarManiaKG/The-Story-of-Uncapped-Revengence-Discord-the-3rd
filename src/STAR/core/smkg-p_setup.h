// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_setup.h
/// \brief Globalizes TSoURDt3rd's unique WAD I/O and map setup routines

#ifndef __SMKG_S_PSETUP__
#define __SMKG_S_PSETUP__

#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_P_LoadAddon(INT32 wadnum, INT32 numlumps);
void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_S_PSETUP__
