// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-ps_main.c
/// \brief A unique parser for TSoURDt3rd related lumps.

#include "smkg-ps_main.h"
#include "../star_vars.h"
#include "../misc/smkg-m_misc.h"
#include "../menus/smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

INT32 tsourdt3rd_starparser_lump_loading = 0; // are we loading lumps?
INT32 tsourdt3rd_starparser_num_brackets = 0; // are we checking for our proper brackets?
INT32 tsourdt3rd_starparser_num_errors = 0; // have we stumbled upon a parser error?
INT32 tsourdt3rd_starparser_num_errored_lumps = 0; // how many lumps, that we've loaded so far, have come across errors?

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_STARParser_Read(tsourdt3rd_starparser_t *script, char *text, size_t lumpLength, boolean (*parserfunc)(tsourdt3rd_starparser_t *))
// Parses the given script, using the STARParser, checking for any parserable terms and the sort.
//
void TSoURDt3rd_STARParser_Read(tsourdt3rd_starparser_t *script, char *text, size_t lumpLength, boolean (*parserfunc)(tsourdt3rd_starparser_t *))
{
	script->tokenizer = Tokenizer_Open(text, lumpLength, 2);

	if (!script->tokenizer)
		return;

	script->tkn = script->tokenizer->get(script->tokenizer, 0);
	script->val = script->tokenizer->get(script->tokenizer, 1);

	if (stricmp(script->tkn, "TSoURDt3rd"))
	{
		TSoURDt3rd_STARParser_Error("No script namespace provided!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		return;
	}
	if (TSoURDt3rd_STARParser_CheckForBrackets(script) != TSOURDT3RD_STARPARSER_BRAK_OPEN)
	{
		TSoURDt3rd_STARParser_Error("No script initializing bracket!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		return;
	}

	while (script->tkn != NULL)
	{
		TSoURDt3rd_STARParser_CheckForBrackets(script);
		if (tsourdt3rd_starparser_num_errors)
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Oops! Error(s?) found!\n");
			break;
		}
		else if (tsourdt3rd_starparser_num_brackets <= 0)
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Successfully parsed the lump! Exiting...\n");
			break;
		}

		boolean failed = parserfunc(script);
		if (!failed)
		{
			script->tkn = script->tokenizer->get(script->tokenizer, 0);
			script->val = script->tokenizer->get(script->tokenizer, 1);
			TSoURDt3rd_STARParser_CheckForBrackets(script);
		}

		if (tsourdt3rd_starparser_num_errors)
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Oops! Error(s?) found!\n");
			break;
		}
		else if (tsourdt3rd_starparser_num_brackets <= 0)
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Successfully parsed the lump! Exiting...\n");
			break;
		}

		if (failed)
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Failed to parse lump! Exiting...\n");
			break;
		}
	}
}
