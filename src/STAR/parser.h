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

#include "ss_main.h" // STAR_CONS_Printf() //
#include "../doomstat.h"
#include "../d_main.h" // srb2home //
#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../s_sound.h"
#include "../fastcmp.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_script_errortype {
	STAR_SCRIPT_ERROR_STANDARD,
	STAR_SCRIPT_ERROR_LUMP,
	STAR_SCRIPT_ERROR_FULL,
}

// ------------------------ //
//        Functions
// ------------------------ //

// ======
// PARSER
// ======

void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump);

void TSoURDt3rd_Parse(MYFILE *f, void (*parserfunc)(MYFILE *, const char *, const char *, ...));
void TSoURDt3rd_ParseScript(MYFILE *f);

// =======
// PARSING
// =======

void TSoURDt3rd_ParseJukeboxDef(MYFILE *f, const char *word, const char *value, ...);

#endif // __STAR_PARSER__
