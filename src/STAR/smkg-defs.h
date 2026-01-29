// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-defs.h
/// \brief BIOS definition data for TSoURDt3rd

#ifndef __SMKG_DEFS__
#define __SMKG_DEFS__

#include "../doomdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_APP                      "TSoURDt3rd"
#define TSOURDT3RD_APP_FULL                 "The Story of Uncapped-Revengence Discord the 3rd"

#define TSOURDT3RD_SRB2_APP_SHORT	        SRB2APPLICATION " - " TSOURDT3RD_APP
#define TSOURDT3RD_SRB2_APP_SHORT_COMBINED	SRB2APPLICATION "" TSOURDT3RD_APP
#define TSOURDT3RD_SRB2_APP_FULL	        SRB2APPLICATION_FULLNAME " - " TSOURDT3RD_APP


//
// TSoURDt3rd game data!
// The following is meant to be saved in 'tsourdt3rd_gamedata_file'
//
typedef struct
{
	boolean loaded; 	// -- WHENEVER OR NOT WE'RE READY TO SAVE

	//
	// Events
	//

	struct {
		UINT32 found_eggs;
		UINT32 current_eggs;  // -- How many Easter Eggs does the user have?
		UINT32 total_eggs;	  // -- How many Easter Eggs can be found?
	} easter;
} tsourdt3rd_gamedata_t;
// Events //
// Easter
extern INT32 TOTALEGGS;
extern INT32 foundeggs;
extern INT32 collectedmapeggs;
extern INT32 currenteggs;
extern INT32 numMapEggs;


// Converts duration in tics to milliseconds
#define TICS_TO_MS(tics) ((INT32)(tics * (1000.0f/TICRATE)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_DEFS__
