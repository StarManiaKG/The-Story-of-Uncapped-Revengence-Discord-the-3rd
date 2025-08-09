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
//        Variables
// ------------------------ //

typedef enum
{
	TSOURDT3RD_WORLD_SCENARIO_NORMAL,
	TSOURDT3RD_WORLD_SCENARIO_BOSS,
	TSOURDT3RD_WORLD_SCENARIO_NUM_TYPES
} tsourdt3rd_world_scenarios_t;

typedef enum
{
	TSOURDT3RD_WORLD_SCENARIO_TYPES_NONE          		= 0,
	// -- Bosses
	TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH           = 1<<1,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS         	= 1<<2,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSSPINCH		= 1<<3,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS		= 1<<4,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSSPINCH	= 1<<5,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_RACE				= 1<<6,
	TSOURDT3RD_WORLD_SCENARIO_TYPES_POSTBOSS			= 1<<7
} tsourdt3rd_world_scenarios_types_t;

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_P_LoadAddon(INT32 wadnum, INT32 numlumps);
void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_S_PSETUP__
