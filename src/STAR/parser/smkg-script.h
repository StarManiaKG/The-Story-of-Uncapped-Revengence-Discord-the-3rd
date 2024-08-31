// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-script.h
/// \brief Contains all the data needed to parse custom TSoURDt3rd scripts

#ifndef __SMKG_SCRIPT__
#define __SMKG_SCRIPT__

#include "../ss_main.h" // STAR_CONS_Printf() //

#include "../../m_tokenizer.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_starscript_s
{
	tokenizer_t *tokenizer; // script tokenizer

	const char  *tkn; // current token
	const char  *val; // current value of token, if needed

	char        *lump_name; // the name of the lump we're looking at
	INT32        lump_loading; // are we loading lumps?

	INT32        num_brackets; // are we checking for our proper brackets?
	INT32        eol_type; // the EOL type of our line
	INT32        errors; // have we stumbled upon a parser error?
} tsourdt3rd_starscript_t;

typedef enum
{
	STAR_SCRIPT_ERROR_LUMP,
	STAR_SCRIPT_ERROR_LINE,
	STAR_SCRIPT_ERROR_FULL,
} star_script_errortype_t;

typedef enum star_eol_type
{
	TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING,
	TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING,
	TSOURDT3RD_STARSCRIPT_EOL_COMMA,
	TSOURDT3RD_STARSCRIPT_EOL_SEMICOLON,
	TSOURDT3RD_STARSCRIPT_EOL_NEWLINE
} star_eol_type_t;

// ------------------------ //
//        Functions
// ------------------------ //

// PARSING
void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump);

boolean TSoURDt3rd_STARScript_CheckForCorrectEOL(tsourdt3rd_starscript_t *script, star_eol_type_t check_for_bracket_type);
INT32 STAR_Script_TokenToTableTerm(tsourdt3rd_starscript_t *script, const char *const *term_table);

void STAR_Script_Error(const char *err, tsourdt3rd_starscript_t *script, star_script_errortype_t verboseness);

// PARSER
boolean TSoURDt3rd_ParseJukeboxDef(tsourdt3rd_starscript_t *script);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_SCRIPT__
