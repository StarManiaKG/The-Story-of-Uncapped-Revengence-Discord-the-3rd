// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-ps_main.h
/// \brief Contains all the data needed to parse custom TSoURDt3rd scripts

#ifndef __SMKG_PS_MAIN__
#define __SMKG_PS_MAIN__

#include "../ss_main.h" // STAR_CONS_Printf() //

#include "../../m_tokenizer.h"
#include "../../w_wad.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_starparser_s
{
	wadfile_t   *wad; // the data of the wad we're looking at
	tokenizer_t *tokenizer; // script tokenizer
	const char  *tkn; // current token
	const char  *val; // current value of token, if needed
} tsourdt3rd_starparser_t;

typedef enum
{
	TSOURDT3RD_STARPARSER_ERROR_LUMP,
	TSOURDT3RD_STARPARSER_ERROR_LINE,
	TSOURDT3RD_STARPARSER_ERROR_FULL,
} tsourdt3rd_starparser_errortype_t;

typedef enum
{
	TSOURDT3RD_STARPARSER_NULL = -2,
	TSOURDT3RD_STARPARSER_BRAK_INVALID_CLOSE,
	TSOURDT3RD_STARPARSER_NO_BRAK,
	TSOURDT3RD_STARPARSER_BRAK_OPEN,
	TSOURDT3RD_STARPARSER_BRAK_CLOSE,
} tsourdt3rd_starparser_bracket_type_t;

extern INT32 tsourdt3rd_starparser_lump_loading;
extern INT32 tsourdt3rd_starparser_num_brackets;
extern INT32 tsourdt3rd_starparser_num_errors;
extern INT32 tsourdt3rd_starparser_num_errored_lumps;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_STARParser_Read(tsourdt3rd_starparser_t *script, char *text, size_t lumpLength, boolean (*parserfunc)(tsourdt3rd_starparser_t *));

boolean TSoURDt3rd_STARParser_JUKEDEF(tsourdt3rd_starparser_t *script);
boolean TSoURDt3rd_STARParser_EXMUSDEF(tsourdt3rd_starparser_t *script);

void TSoURDt3rd_STARParser_STRLCPY(char *p, const char *s, size_t n);
INT32 TSoURDt3rd_STARParser_CheckForBrackets(tsourdt3rd_starparser_t *script);
INT32 TSoURDt3rd_STARParser_ValidTableTerm(tsourdt3rd_starparser_t *script, const char *const *term_table, boolean enforce_brackets);
void TSoURDt3rd_STARParser_Error(const char *err, tsourdt3rd_starparser_t *script, tsourdt3rd_starparser_errortype_t verboseness);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_PS_MAIN__
