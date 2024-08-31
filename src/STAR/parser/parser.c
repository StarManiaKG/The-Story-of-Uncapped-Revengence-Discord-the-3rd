// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parser.c
/// \brief Contains STAR script parsing data

#include <time.h>

#include "smkg-script.h"
#include "../star_vars.h"
#include "../smkg-misc.h"

#include "../drrr/k_menu.h"

#include "../../w_wad.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_term_e
{
	star_jukeboxdef,
#if 0
	star_windowtitles,
#endif
};

static const char *const star_term_opt[] = {
	"JUKEBOXDEF",
#if 0
	"WINDOWTITLES",
#endif
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

// ============
// PARSER TOOLS
// ============

//
// boolean TSoURDt3rd_STARScript_CheckForCorrectEOL(tsourdt3rd_starscript_t *script, star_eol_type_t check_for_bracket_type)
// Finds the correct EOL to check for within a STAR script, based on the checking type given.
//
boolean TSoURDt3rd_STARScript_CheckForCorrectEOL(tsourdt3rd_starscript_t *script, star_eol_type_t check_for_bracket_type)
{
	if (!script->tkn)
		return false;

	switch (check_for_bracket_type)
	{
		case TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING:
			if (!strcmp(script->tkn, "{"))
			{
				script->num_brackets++;
				return true;
			}
			return false;

		case TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING:
		{
			if (strcmp(script->tkn, "}"))
				return false;

			if (!script->val || strcmp(script->val, ";"))
			{
				STAR_Script_Error("Missing semicolon directly after bracket!", script, STAR_SCRIPT_ERROR_FULL);
				return false;
			}

			script->num_brackets--;
			return true;
		}

		case TSOURDT3RD_STARSCRIPT_EOL_COMMA:
			return (!strcmp(script->tkn, ","));

		case TSOURDT3RD_STARSCRIPT_EOL_SEMICOLON:
			return (!strcmp(script->tkn, ";"));

		case TSOURDT3RD_STARSCRIPT_EOL_NEWLINE:
			return (!strcmp(script->tkn, "\n"));

		default:
			return false;
	}
}

//
// INT32 STAR_Script_TokenToTableTerm(tsourdt3rd_starscript_t *script, const char *const *term_table)
// Attempts to finds the token within the table given.
//
INT32 STAR_Script_TokenToTableTerm(tsourdt3rd_starscript_t *script, const char *const *term_table)
{
	INT32 i;

	if (!script->tkn)
		return -1;

	for (i = 0; term_table[i]; i++)
		if (!stricmp(script->tkn, term_table[i]))
			break;

	script->tkn = script->tokenizer->get(script->tokenizer, 0);
	return i;
}

//
// void STAR_Script_Error(const char *err, tsourdt3rd_starscript_t *script, star_script_errortype_t verboseness)
// Prints an error, related to the current STAR script, based on the given 'verboseness' type.
//
void STAR_Script_Error(const char *err, tsourdt3rd_starscript_t *script, star_script_errortype_t verboseness)
{
	script->errors++;

	switch (verboseness)
	{
		case STAR_SCRIPT_ERROR_FULL:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (in lump %s, line %d)\n", err, script->lump_name, script->tokenizer->line);
			break;
		case STAR_SCRIPT_ERROR_LUMP:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (in lump %s)\n", err, script->lump_name);
			break;
		case STAR_SCRIPT_ERROR_LINE:
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "%s (on line %d)\n", err, script->tokenizer->line);
			break;
		default:
			break;
	}
}

// ======
// PARSER
// ======

//
// static void TSoURDt3rd_Parse(tsourdt3rd_starscript_t *script, boolean (*parserfunc)(tsourdt3rd_starscript_t *))
// Parses given STAR script terms and their properties.
//
static void TSoURDt3rd_Parse(tsourdt3rd_starscript_t *script, boolean (*parserfunc)(tsourdt3rd_starscript_t *))
{
#if 1
	INT32 prev_brackets = script->num_brackets;
#else
	INT32 prev_brackets;
#endif

	// Check for the opening brackets first.
	if (!TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING))
	{
		STAR_Script_Error("Opening term bracket missing!", script, STAR_SCRIPT_ERROR_LINE);
		return;
	}
#if 0
	prev_brackets = script->num_brackets;
#endif

	// Now, parse the individual data of our terms.
	while (script->tkn != NULL)
	{
#if 1
		script->tkn = script->tokenizer->get(script->tokenizer, 0);
		script->val = script->tokenizer->get(script->tokenizer, 1);
		CONS_Printf("before - param + val = %s:%s\n", script->tkn, script->val);

		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);

#if 0
		if (!parserfunc)
			continue;
#endif

#if 0
		while (!parserfunc(script))
		{
			// If the function returns true, then we
			// have an error, and can't continue.
			// Otherwise, just keep on chugging.
			continue;
		}
#else
#if 0
		parserfunc(script);
#else
#if 1
		if (parserfunc && parserfunc(script))
#else
		if (parserfunc(script))
#endif
			break;
#endif
#endif
#else
		CONS_Printf("pre-before - param + val = %s:%s\n", script->tkn, script->val);

		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);

		if (parserfunc)
		{
			script->tkn = script->tokenizer->get(script->tokenizer, 0);
			script->val = script->tokenizer->get(script->tokenizer, 1);
			CONS_Printf("before - param + val = %s:%s\n", script->tkn, script->val);
			parserfunc(script);
		}
		else
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "No parser function for term %s found!\n", script->tkn);
			TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
			TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);
			return;
		}
#endif

#if 1
		CONS_Printf("current brackets = %d:%d\n", script->num_brackets, prev_brackets);
		CONS_Printf("at brackets - param + val = %s:%s\n", script->tkn, script->val);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);

#if 0
		script->tkn = script->tokenizer->get(script->tokenizer, 0);
		//script->val = script->tokenizer->get(script->tokenizer, 1);
		CONS_Printf("after - param + val = %s:%s\n", script->tkn, script->val);
#endif

		if (script->num_brackets <= prev_brackets)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Successfully parsed a block!\n");
			break;
		}
#if 0
		else if (script->errors)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Encountered errors while parsing a block.\n");
			break;
		}
#endif
#endif

#if 1
		script->tkn = script->tokenizer->get(script->tokenizer, 0);
		script->val = script->tokenizer->get(script->tokenizer, 1);
		CONS_Printf("after - param + val = %s:%s\n", script->tkn, script->val);
#endif

		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);
#if 0
		if (script->num_brackets < prev_brackets)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Successfully parsed a block!\n");
			break;
		}
#endif
	}
}

//
// static void TSoURDt3rd_ParseScript(tsourdt3rd_starscript_t *script)
// Parses the given STAR script, checking for any parserable terms.
//
static void TSoURDt3rd_ParseScript(tsourdt3rd_starscript_t *script)
{
	// Check for valid fields through all of the script, and the sort.
	while (script->tkn != NULL)
	{
		const char *block_term = script->tkn;

		switch (STAR_Script_TokenToTableTerm(script, star_term_opt))
		{
			case star_jukeboxdef:
				TSoURDt3rd_Parse(script, TSoURDt3rd_ParseJukeboxDef);
				break;
#if 0
			case star_windowtitles:
				TSoURDt3rd_Parse(script, NULL);
				break;
#endif
			default:
				STAR_Script_Error(va("Unknown field '%s'.", block_term), script, STAR_SCRIPT_ERROR_FULL);
				break;
		}

		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING);
		TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING);

		if (script->errors)
			break;
	}

	// Check for errors now.
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "\n\tSTRING: %s\n\tBRACKETS: %d\n", script->tkn, script->num_brackets);
	if (script->num_brackets > 1 || script->errors)
	{
		// Oooops! You have to put the brackets and terms, in the correct place.
		if (script->num_brackets > 0)
			STAR_Script_Error("Some brackets are not properly enclosed!", script, STAR_SCRIPT_ERROR_LUMP);
		else if (script->num_brackets < 0)
			STAR_Script_Error("Bracket enclosure '}' has been misplaced somewhere!", script, STAR_SCRIPT_ERROR_LUMP);

		if (script->errors)
		{
			DRRR_M_StartMessage(
				wadfiles[numwadfiles-1]->filename,
				va("Stumbled upon\n%d parser errors\nwithin this lump!\n", script->errors),
				NULL,
				MM_NOTHING,
				NULL,
				NULL
			);
			S_StartSound(NULL, sfx_skid);
		}
		script->errors = 0;
	}
}

// ============
// FILE LOADING
// ============

//
// static boolean CheckForScriptNamespace(tsourdt3rd_starscript_t *script)
// Looks for the correct TSoURDt3rd namespace initializer at the beginning of a STAR Script.
//
static boolean CheckForScriptNamespace(tsourdt3rd_starscript_t *script)
{
	if (!script->tokenizer)
		return false;

	script->tkn = script->tokenizer->get(script->tokenizer, 0);
	if (stricmp(script->tkn, "TSoURDt3rd"))
	{
		STAR_Script_Error("No script namespace provided!", script, STAR_SCRIPT_ERROR_LUMP);
		return false;
	}
	script->tkn = script->tokenizer->get(script->tokenizer, 0);

	if (!TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING))
	{
		STAR_Script_Error("No script initializing bracket!", script, STAR_SCRIPT_ERROR_LUMP);
		return false;
	}
	script->tkn = script->tokenizer->get(script->tokenizer, 0);

	return true;
}

//
// static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
// Loads a script from the given 'MYFILE'.
//
static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
{
	tsourdt3rd_starscript_t *script = Z_Malloc(sizeof(tsourdt3rd_starscript_t), PU_STATIC, NULL);

	if (!script)
		return;

	script->tokenizer = Tokenizer_Open(f->data, f->size, 2);
	if (script->lump_loading <= 0)
	{
		script->lump_loading = 0;
		script->num_brackets = 0;
		script->eol_type = 0;
		script->errors = 0;
	}

	if (!name)
		name = wadfiles[f->wad]->filename;
	script->lump_name = name;

	if (CheckForScriptNamespace(script))
	{
		script->lump_loading++; // turn on loading flag
		TSoURDt3rd_ParseScript(script);
		script->lump_loading--; // turn off loading flag
	}

	Tokenizer_Close(script->tokenizer);
	if (script->lump_loading <= 0)
	{
		Z_Free(script);
		script = NULL;
	}
}

//
// void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump)
// Loads the given STAR script lump.
//
void TSoURDt3rd_LoadLump(UINT16 wad, UINT16 lump)
{
	MYFILE f;
	char *name;
	size_t len;

	f.wad = wad;
	f.size = W_LumpLengthPwad(wad, lump);
	f.data = Z_Malloc(f.size, PU_STATIC, NULL);
	W_ReadLumpPwad(wad, lump, f.data);
	f.curpos = f.data;

	len = strlen(wadfiles[wad]->filename); // length of file name

	if (wadfiles[wad]->type == RET_STAR)
	{
		name = malloc(len+1);
		strcpy(name, wadfiles[wad]->filename);
	}
	else // If it's not a .star file, copy the lump name in too.
	{
		lumpinfo_t *lump_p = &wadfiles[wad]->lumpinfo[lump];
		len += 1 + strlen(lump_p->fullname); // length of file name, '|', and lump name
		name = malloc(len+1);
		sprintf(name, "%s|%s", wadfiles[wad]->filename, lump_p->fullname);
		name[len] = '\0';
	}

	TSoURDt3rd_LoadFile(&f, name); // actually load file!

	free(name);
	Z_Free(f.data);
}
