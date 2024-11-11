// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-ps_main.c
/// \brief A unique parser for TSoURDt3rd related lumps.

#include "smkg-ps_main.h"
#include "../star_vars.h"
#include "../smkg-misc.h"

#include "../menus/smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

INT32 tsourdt3rd_starparser_lump_loading = 0; // are we loading lumps?
INT32 tsourdt3rd_starparser_num_brackets = 0; // are we checking for our proper brackets?
INT32 tsourdt3rd_starparser_num_errors = 0; // have we stumbled upon a parser error?

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
		goto close_lump;
	tsourdt3rd_starparser_lump_loading++; // turn on loading flag

	script->tkn = script->tokenizer->get(script->tokenizer, 0);
	script->val = script->tokenizer->get(script->tokenizer, 1);

	if (stricmp(script->tkn, "TSoURDt3rd"))
	{
		TSoURDt3rd_STARParser_Error("No script namespace provided!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		goto close_lump;
	}
	if (TSoURDt3rd_STARParser_CheckForBrackets(script) != TSOURDT3RD_STARPARSER_BRAK_OPEN)
	{
		TSoURDt3rd_STARParser_Error("No script initializing bracket!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		goto close_lump;
	}

	while (script->tkn != NULL)
	{
		TSoURDt3rd_STARParser_CheckForBrackets(script);
		if (tsourdt3rd_starparser_num_errors)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Oops! Error(s?) found!\n");
			break;
		}
		else if (tsourdt3rd_starparser_num_brackets <= 0)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Successfully parsed the lump! Exiting...\n");
			break;
		}

		if (parserfunc)
			parserfunc(script);

		script->tkn = script->tokenizer->get(script->tokenizer, 0);
		script->val = script->tokenizer->get(script->tokenizer, 1);

		TSoURDt3rd_STARParser_CheckForBrackets(script);
		if (tsourdt3rd_starparser_num_errors)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Oops! Error(s?) found!\n");
			break;
		}
		else if (tsourdt3rd_starparser_num_brackets <= 0)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Successfully parsed the lump! Exiting...\n");
			break;
		}
	}

	// Oooops! You have to put the brackets and terms, in the correct place.
	if (tsourdt3rd_starparser_num_brackets > 0)
		TSoURDt3rd_STARParser_Error("Some brackets are not properly enclosed!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
	else if (tsourdt3rd_starparser_num_brackets < 0)
		TSoURDt3rd_STARParser_Error("Bracket enclosure '}' has been misplaced somewhere!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);

	// Uh-oh! Error!
	if (tsourdt3rd_starparser_num_errors > 0)
	{
		TSoURDt3rd_M_StartMessage(
			wadfiles[numwadfiles-1]->filename,
			va("Stumbled upon\n%d parser error(s)\nwithin this lump!\n", tsourdt3rd_starparser_num_errors),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		S_StartSound(NULL, sfx_skid);
	}

	goto close_lump;

	close_lump:
	{
		if (script->tokenizer)
			Tokenizer_Close(script->tokenizer);
		tsourdt3rd_starparser_num_errors = 0;
		tsourdt3rd_starparser_lump_loading--; // turn off loading flag
	}
}
