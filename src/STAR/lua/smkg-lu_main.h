// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-lu_main.h
/// \brief TSoURDt3rd's extended BLua library

#ifndef __SMKG_LU_MAIN__
#define __SMKG_LU_MAIN__

#include "../../lua_script.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Functions
// ------------------------ //

INT32 TSoURDt3rd_LUA_PushGlobalVariables(lua_State *L, const char *word);
boolean TSoURDt3rd_LUA_SetGlobalVariables(lua_State *L, const char *word);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_LU_MAIN__
