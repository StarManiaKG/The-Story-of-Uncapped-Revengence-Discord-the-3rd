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

#include "ss_main.h" // STAR_CONS_Printf() & STAR_M_StartMessage() //

#include "../doomstat.h"
#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../s_sound.h"
#include "../fastcmp.h"

#include "../d_main.h" // srb2home //

// ------------------------ //
//        Variables
// ------------------------ //

typedef enum
{
	STAR_SCRIPT_ERROR_STANDARD,
	STAR_SCRIPT_ERROR_LUMP,
	STAR_SCRIPT_ERROR_LINE,
	STAR_SCRIPT_ERROR_FULL,
} star_script_errortype_t;

// ------------------------ //
//        Functions
// ------------------------ //

// ======
// PARSER
// ======

void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump);

void STAR_Script_Error(const char *err, star_script_errortype_t verboseness);

const char *STAR_Script_ReadNewLine(UINT32 i);
boolean STAR_Script_ValidTerm(const char *string, const char *value, const char *cmpstring);

void TSoURDt3rd_Parse(MYFILE *f, boolean (*parserfunc)(const char *, const char *));
void TSoURDt3rd_ParseScript(MYFILE *f);

// =======
// PARSING
// =======

boolean TSoURDt3rd_ParseJukeboxDef(const char *word, const char *value);

#endif // __STAR_PARSER__
