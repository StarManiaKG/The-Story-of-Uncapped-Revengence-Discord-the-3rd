// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-d_main.h
/// \brief TSoURDt3rd's main program data

#ifndef __SMKG_D_MAIN__
#define __SMKG_D_MAIN__

#include "../../w_wad.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_AUTOLOAD_CONFIG_FILENAME "autoload.cfg"
#define TSOURDT3RD_DEFAULT_SAVEGAMEFOLDER   "saves"

#define TSOURDT3RD_MAX_DIRPATH              MAX_WADPATH*2

extern char tsourdt3rd_home_dir[TSOURDT3RD_MAX_DIRPATH];

extern char tsourdt3rd_gamedata_file[MAX_WADPATH];
extern char tsourdt3rd_savedata_prefix[MAX_WADPATH];

extern char tsourdt3rd_save_dir[MAX_WADPATH];
extern char tsourdt3rd_savefile_dir[TSOURDT3RD_MAX_DIRPATH];

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_D_AutoLoadAddons(void);

void TSoURDt3rd_D_Init(void);
void TSoURDt3rd_D_Loop(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_D_MAIN__
