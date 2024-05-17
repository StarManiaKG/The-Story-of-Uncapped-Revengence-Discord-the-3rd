// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parser.h
/// \brief parser functions, used to parse TSoURDt3rd scripts

#ifndef __STAR_PARSER__
#define __STAR_PARSER__

#include "parsed.h"
#include "../doomstat.h"

// ------------------------ //
//        Variables
// ------------------------ //

extern INT32 star_line;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump);

void TSoURDt3rd_Parse(MYFILE *f, void (*parserfunc)(MYFILE *, const char *, const char *));
void TSoURDt3rd_ParseScript(MYFILE *f);

#endif // __STAR_PARSER__
