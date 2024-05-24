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

#include "parser.h"

// ------------------------ //
//        Variables
// ------------------------ //

INT32 star_lumploading = 0; // are we loading our lump(s)? //
const char *star_lump = NULL; // what is the current lump's name? //

INT32 star_line = -1; // what line are we on? //

INT32 star_brackets = 0; // are we checking for our proper brackets? //

INT32 star_parseerror = 0; // have we stumbled upon a parser error? //

enum star_term_e
{
	star_jukeboxdef,
	star_windowtitles,
};

static const char *const star_term_opt[] = {
	"JUKEBOXDEF",
	"WINDOWTITLES",
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

// ============
// FILE LOADING
// ============

//
// static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
// Loads a script from the given 'MYFILE'.
//
static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
{
	star_lumploading++; // turn on loading flag

	if (!name)
		name = wadfiles[f->wad]->filename;
	star_lump = name;

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Loading STAR script from %s\n", star_lump);
    TSoURDt3rd_ParseScript(f);

	star_lumploading--; // turn off loading flag
	star_line = -1; // clear lines checked

	star_brackets = 0; // clear bracket amounts

	star_parseerror = 0; // clear parser errors
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

// ============
// PARSER TOOLS
// ============

#if 0
static void MusicDefStrcpy (char *p, const char *s, size_t n)
{
	strlcpy(p, s, n);
	while (( p = strchr(p, '_') ))
		*p++ = ' '; // turn _ into spaces.
}
#endif

//
// const char *STAR_Script_ReadNewLine(UINT32 i)
// Returns the read text in the (already) given file, based on position 'i'.
//
const char *STAR_Script_ReadNewLine(UINT32 i)
{
	FILE *af;
	const char *path;

	path = va("%s"PATHSEP"%s", srb2home, "STAR_starfilecontents.star");
	af = fopen(path, "a+");

	if (!i)
		star_line++;
	const char *txt = M_TokenizerRead(i);

	fputs(va("%s\n", txt), af);
	fclose(af);

	return txt;
}

//
// static boolean STAR_Script_CheckForEnclosedBrackets(const char *string)
// Checks for brackets within the STAR script and whether or not they're enclosed.
//
// Return values:
//	0 - No bracket found.
//	1 - Enclosed bracket found!
//	2 - Bracket, but not enclosed properly...
//
static boolean STAR_Script_CheckForEnclosedBrackets(const char *string)
{
	M_TokenizerSetEndPos(M_TokenizerGetEndPos()); // set the end position for this text junk right quick please

	if (fastcmp(string, "}"))
	{
#if 0
		char junk[64];
		strcpy(junk, string);
		myfgets(junk, sizeof junk, f);

		string = STAR_Script_ReadNewLine(0);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "STAR_Script_CheckForEnclosedBrackets_AFTER: '%s, %s'!\n", STAR_Script_ReadNewLine(0), STAR_Script_ReadNewLine(1));	

		if (!fastcmp(string, ";"))
		{
			STAR_Script_Error("Missing semicolon directly after bracket!", STAR_SCRIPT_ERROR_FULL);
			return 2;
		}
#endif

		star_brackets--;
		return 1;
	}
	else if (fastcmp(string, "{"))
	{
		star_brackets++;
		return 1;
	}

	return 0;
}

//
// boolean STAR_Script_ValidTerm(const char *string, const char *value, const char *cmpstring)
//
// Compares 'string' against the 'cmpstring', in order to ensure that 'string' is a valid STAR script term.
//
// Afterwards, if successful, checks for proper brackets,
//	sets the given 'string' (and 'value' if it exists), to new lines in the STAR script, and returns.
//
boolean STAR_Script_ValidTerm(const char *string, const char *value, const char *cmpstring)
{
	boolean bracketFound;

	if (!fasticmp(string, cmpstring))
		return false;

	if (value)
		string = value;
	else
		string = STAR_Script_ReadNewLine(0);
	bracketFound = STAR_Script_CheckForEnclosedBrackets(value);

	if (bracketFound == 2)
	{
		star_parseerror++;
		return false;
	}
	else if (!bracketFound && !fastcmp(cmpstring, ","))
	{
		star_parseerror++;
		return false;
	}

	string = STAR_Script_ReadNewLine(0);
	if (value)
		value = STAR_Script_ReadNewLine(1);

	return true;
}

//
// void STAR_Script_Error(const char *err, star_script_errortype_t verboseness)
// Prints an error, 'err', related to the current STAR script, based on the given 'verboseness'.
//
void STAR_Script_Error(const char *err, star_script_errortype_t verboseness)
{
	switch (verboseness)
	{
		case STAR_SCRIPT_ERROR_FULL:
			err = va("%s (in lump %s, line %d)", err, star_lump, star_line);
			break;

		case STAR_SCRIPT_ERROR_LUMP:
			err = va("%s (in lump %s)", err, star_lump);
			break;

		case STAR_SCRIPT_ERROR_LINE:
			err = va("%s (on line %d)", err, star_line);
			break;

		default:
			break;
	}
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, va("%s\n", err));
}

// ======
// PARSER
// ======

//
// void TSoURDt3rd_Parse(MYFILE *f, boolean (*parserfunc)(const char *, const char *))
// Parses the (already) given STAR script term and its properties, using function 'parserfunc' (if defined (which it should be, because that makes sense)).
//
void TSoURDt3rd_Parse(MYFILE *f, boolean (*parserfunc)(const char *, const char *))
{
	const char *param, *val;
	INT32 prev_brackets;

	// Check for opening term brackets first! //
	param = STAR_Script_ReadNewLine(0);
	if (STAR_Script_CheckForEnclosedBrackets(param) != 1)
	{
		STAR_Script_Error("Opening bracket missing!", STAR_SCRIPT_ERROR_LINE);
		star_parseerror++;
		return;
	}
	M_TokenizerSetEndPos(M_TokenizerGetEndPos()); // set the end position for this text junk right quick please

	// Iterate through even more of our data please! //
	while (M_TokenizerGetEndPos() < f->size)
	{
		prev_brackets = star_brackets;
		param = STAR_Script_ReadNewLine(0);
		val	= STAR_Script_ReadNewLine(1); // value after the '=' sign

		if (!parserfunc)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "No parser function for term %s found!\n", param);
			break;
		}

		while (parserfunc(param, val))
		{
			if (STAR_Script_CheckForEnclosedBrackets(param) == 2)
			{
				star_parseerror++;
				break;
			}

			if (star_brackets <= 1 || star_brackets > prev_brackets)
				break;

			param = STAR_Script_ReadNewLine(0);
			val	= STAR_Script_ReadNewLine(1); // value after the '=' sign

#if 0
			if (STAR_Script_CheckForEnclosedBrackets(value) == 2)
			{
				star_parseerror++;
				break;
			}
#endif
		}
	}
}

//
// void TSoURDt3rd_ParseScript(MYFILE *f)
// Parses the STAR script, checking for any parserable terms.
//
static enum star_term_e STAR_Script_FindValidTerm(const char *tkn)
{
	enum star_term_e term;
	for (term = 0; term <= sizeof(enum star_term_e); term++)
	{
		if (fasticmp(tkn, star_term_opt[term]))
			break;
	}
	return term;
}

void TSoURDt3rd_ParseScript(MYFILE *f)
{
	const char *tkn;

	// Look for the TSoURDt3rd namespace initializer at the beginning. //
	M_TokenizerOpen(f->data);
	tkn = STAR_Script_ReadNewLine(0);

	if (STAR_Script_CheckForEnclosedBrackets(tkn) != 1)
	{
		STAR_Script_Error("No script initializing bracket!", STAR_SCRIPT_ERROR_LUMP);
		goto closeFile;
	}

	// Iterate through all of our file's info please! //
	while (M_TokenizerGetEndPos() < f->size)
	{
		// Increment our lines and check for proper brackets.
		tkn = STAR_Script_ReadNewLine(0);
#if 0		
		if (STAR_Script_CheckForEnclosedBrackets(tkn) == 2)
			goto fileError;
#endif

		// Check for valid fields.
		switch (STAR_Script_FindValidTerm(tkn))
		{
			case star_jukeboxdef:	TSoURDt3rd_Parse(f, TSoURDt3rd_ParseJukeboxDef); break;
			case star_windowtitles:	TSoURDt3rd_Parse(f, NULL); break;

			default: STAR_Script_Error("Unknown field '%s'.", STAR_SCRIPT_ERROR_FULL); break;
		}

		// Check for errors, then rinse and repeat :)
		if (star_brackets > 1 || star_parseerror)
			goto fileError;
	}

	goto closeFile; // We've made it! Close the file now please :) //

	fileError:
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "\n\tSTRING: %s\n\tBRACKETS: %d\n", tkn, star_brackets);
		if (star_brackets > 1)
			STAR_Script_Error("Some brackets are not properly enclosed!", STAR_SCRIPT_ERROR_LUMP);

		if (star_parseerror)
		{
			STAR_M_StartMessage(star_lump, 0, "Stumbled upon one or more parser errors within this lump!\n\n(Press any key to continue)\n", NULL, MM_NOTHING);
			STAR_Script_Error("Stumbled upon a parser error!", STAR_SCRIPT_ERROR_FULL);

			star_parseerror--;
		}

		goto closeFile;
	}

	closeFile:
	{
		M_TokenizerClose();
		return;
	}
}
