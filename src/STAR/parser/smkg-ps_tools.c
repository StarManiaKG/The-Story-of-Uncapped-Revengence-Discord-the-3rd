// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-ps_tools.c
/// \brief Contains helpful STARScript parsing routines

#include "smkg-ps_main.h"
#include "../misc/smkg-m_misc.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_STARParser_STRLCPY(char *p, const char *s, size_t n)
// Copies a given string, parses underscores as spaces, and returns the new string when done.
//
void TSoURDt3rd_STARParser_STRLCPY(char *p, const char *s, size_t n)
{
	strlcpy(p, s, n);
	while ((p = strchr(p, '_')))
		*p++ = ' '; // turn _ into spaces.
}

//
// INT32 TSoURDt3rd_STARParser_CheckForBrackets(tsourdt3rd_starparser_t *script)
// Checks for any brackets within STARParser tokens.
//
// Returns a zero if a bracket wasn't found,
//	a value lower than zero if the wrong EOL for a bracket was found,
//	or a non-zero value if everything is correct otherwise, depending on the bracket.
//
INT32 TSoURDt3rd_STARParser_CheckForBrackets(tsourdt3rd_starparser_t *script)
{
	const char *string_to_check[] = {
		script->tkn,
		script->val,
		NULL
	};
	INT32 string_iterator;

	if (!script)
		return TSOURDT3RD_STARPARSER_NULL;

	for (string_iterator = 0; string_to_check[string_iterator]; string_iterator++)
	{
		const char *string = string_to_check[string_iterator];
		const char *opposite_string = string_to_check[!(string_iterator % 1)];

		if (string == NULL || *string == '\0')
			continue;

		if (!strcmp(string, "{"))
		{
			tsourdt3rd_starparser_num_brackets++;
			return TSOURDT3RD_STARPARSER_BRAK_OPEN;
		}

		if (opposite_string == NULL || *opposite_string == '\0')
			continue;

		if (!strcmp(string, "}"))
		{
			if (strcmp(opposite_string, ";"))
			{
				TSoURDt3rd_STARParser_Error("Missing semicolon directly after bracket!", script, TSOURDT3RD_STARPARSER_ERROR_FULL);
				return TSOURDT3RD_STARPARSER_BRAK_INVALID_CLOSE;
			}
			tsourdt3rd_starparser_num_brackets--;
			return TSOURDT3RD_STARPARSER_BRAK_CLOSE;
		}
	}
	return TSOURDT3RD_STARPARSER_NO_BRAK;
}

//
// INT32 TSoURDt3rd_STARParser_ValidTableTerm(tsourdt3rd_starparser_t *script, const char *const *term_table, boolean enforce_brackets)
// Attempts to finds a STARParser token within the table given.
//
INT32 TSoURDt3rd_STARParser_ValidTableTerm(tsourdt3rd_starparser_t *script, const char *const *term_table, boolean enforce_brackets)
{
	INT32 table_index;

	if (!script)
		return -1;

	script->tkn = script->tokenizer->get(script->tokenizer, 0);
	script->val = script->tokenizer->get(script->tokenizer, 1);

	if (!script->tkn || !script->val)
		return -1;

	table_index = TSoURDt3rd_M_FindWordInTermTable(term_table, script->tkn, TSOURDT3RD_TERMTABLESEARCH_NORM);

	if (enforce_brackets)
	{
		if (TSoURDt3rd_STARParser_CheckForBrackets(script) != TSOURDT3RD_STARPARSER_BRAK_OPEN)
			return -1;

		script->tkn = script->tokenizer->get(script->tokenizer, 0);
		script->val = script->tokenizer->get(script->tokenizer, 1);
	}
	return table_index;
}

//
// void TSoURDt3rd_STARParser_Error(const char *err, tsourdt3rd_starparser_t *script, tsourdt3rd_starparser_errortype_t verboseness)
// Prints a STARParser error, based on the given 'verboseness' type.
//
void TSoURDt3rd_STARParser_Error(const char *err, tsourdt3rd_starparser_t *script, tsourdt3rd_starparser_errortype_t verboseness)
{
	tsourdt3rd_starparser_num_errors++;
	switch (verboseness)
	{
		case TSOURDT3RD_STARPARSER_ERROR_FULL:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (in lump %s, line %d)\n", err, script->wad->filename, script->tokenizer->line);
			break;
		case TSOURDT3RD_STARPARSER_ERROR_LUMP:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (in lump %s)\n", err, script->wad->filename);
			break;
		default:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (near terms (%s,%s))\n", err, script->tkn, script->val);
			break;
	}
}
