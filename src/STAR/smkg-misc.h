// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-misc.h
/// \brief Commonly used manipulation routines, globally initalized

#ifndef __SMKG_MISC__
#define __SMKG_MISC__

#include "../doomstat.h"

// ------------------------ //
//        Functions
// ------------------------ //

char *STAR_M_RemoveStringChars(char *string, const char *c);

const char *TSoURDt3rd_FOL_ReturnHomepath(void);
boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory);
boolean TSoURDt3rd_FOL_CreateDirectory(const char *cpath);

FILE *TSoURDt3rd_FIL_CreateFile(const char *directory, const char *filename, const char *mode);
boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename);

#endif // __SMKG_MISC__
