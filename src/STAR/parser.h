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

// VARIABLES //
extern INT32 star_line;

// FUNCTIONS //
boolean ValidTerm(const char *string, const char *word);

void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump);
void TSoURDt3rd_ParseScript(MYFILE *f);

#endif // __STAR_PARSER__
