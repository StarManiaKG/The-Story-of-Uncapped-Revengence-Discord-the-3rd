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
#include "ss_main.h" // STAR_CONS_Printf() //
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

INT32 star_lumploading = 0; // is TSoURDt3rd_LoadLump being called? //
INT32 star_line = -1; // are we checking for our lines? //
INT32 star_brackets = 1; // are we checking for our brackets? //

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

#if 0
static void MusicDefStrcpy (char *p, const char *s, size_t n)
{
	strlcpy(p, s, n);
	while (( p = strchr(p, '_') ))
		*p++ = ' '; // turn _ into spaces.
}
#endif

//
// static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
// Loads a script from the given 'MYFILE'.
//
static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
{
	star_lumploading++; // turn on loading flag

	if (!name)
		name = wadfiles[f->wad]->filename;
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Loading STAR script from %s\n", name);
    TSoURDt3rd_ParseScript(f);

	star_line = 0; // clear lines checked
	star_lumploading--; // turn off loading flag

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
	star_line = 0; // clear lines checked

	free(name);
	Z_Free(f.data);
}

//
// static boolean CheckForEnclosedBrackets(const char *string)
// Checks for brackets within the STAR script and whether or not they're enclosed.
//
// Return values:
//	0 - No bracket found.
//	1 - Enclosed bracket found!
//	2 - Bracket, but not enclosed properly...
//
static boolean CheckForEnclosedBrackets(const char *string)
{
	if (fastcmp(string, "}"))
	{
		if (!fastcmp(M_TokenizerRead(1), ";"))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Missing semicolon directly after bracket! (near line %d)\n", star_line);
			M_TokenizerClose();
			return 2;
		}
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
// void TSoURDt3rd_Parse(MYFILE *f, void (*parserfunc)(MYFILE *, const char *, const char *))
// Parses the (already) given STAR script term, using function 'parserfunc' (if defined (which it should be)).
//
void TSoURDt3rd_Parse(MYFILE *f, void (*parserfunc)(MYFILE *, const char *, const char *))
{
	const char *param, *val;

	// Check for opening term brackets first! //
	string = M_TokenizerRead(0);
	star_line++;

	if (!fastcmp(string, "{"))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Opening bracket at line %d missing!\n", star_line);
		star_parseerror++;
		return;
	}

	star_brackets++;
	M_TokenizerSetEndPos(M_TokenizerGetEndPos()); // set the end position for this text junk right quick please

	// Iterate through even more of our data please! //
	while ((param = M_TokenizerRead(0)) && M_TokenizerGetEndPos() < f->size)
	{

		// Check for properly enclosed brackets...
#if 0
		if (fastcmp(param, "}"))
		{
			star_brackets--;
			if (!star_brackets)
				break;
			else
				continue;
		}
		else if (fastcmp(param, "{"))
		{
			star_brackets++;
			continue;
		}
#else
		if (CheckForEnclosedBrackets(param) != 2)
		{
			if (!star_brackets)
				break;
			continue;
		}
		else
		{
			star_parseerror++;
			return;
		}
#endif

		val = M_TokenizerRead(1); // value after the '=' sign
		if (parserfunc) // here for non-crashing reasons mainly (although, you should make sure each term has a function :p)
			parserfunc(f, param, val);
		else
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "coming soon! - %s and %s\n", param, val);
	}	
}

//
// void TSoURDt3rd_ParseScript(MYFILE *f)
// Parses the STAR script, checking for any parserable terms.
//
void TSoURDt3rd_ParseScript(MYFILE *f)
{
	char *lump = wadfiles[f->wad]->filename;
	const char *tkn;

	FILE *af;
	const char *path;

	path = va("%s"PATHSEP"%s", srb2home, "STAR_starfilecontents.star");
	af = fopen(path, "w+");

	// Look for the TSoURDt3rd namespace initializer at the beginning. //
	M_TokenizerOpen(f->data);
	tkn = M_TokenizerRead(0);

	if (!fastcmp(tkn, "{"))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "No initializing bracket at beginning of lump '%s'!\n", lump);
		goto closeFile;
	}

	fputs(va("%s\n", tkn), af);

	// Iterate through all of our file's info please! //
	while ((tkn = M_TokenizerRead(0)) && M_TokenizerGetEndPos() < f->size)
	{
		fputs(va("%s\n", tkn), af);

		// Increment our lines.
		star_line++;

		// Check for proper brackets.
		if (CheckForEnclosedBrackets(tkn) == 2)
			goto closeFile;

		// Check for valid fields.
		if (star_term_opt[tkn])
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "TOKEN OPT THING: %s\n", star_term_opt[tkn]);
		switch (star_term_opt[tkn])
		{
			case star_jukeboxdef:
				TSoURDt3rd_Parse(f, TSoURDt3rd_ParseJukeboxDef);
				break;
			
			case star_windowtitles:
				TSoURDt3rd_Parse(f, NULL);
				break;

			default:
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Unknown field '%s' in lump '%s'.\n", tkn, lump);
				break;
		}

		// Check if all brackets have been properly enclosed, then rinse and repeat :)
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "BRACKETS: %d\n", star_brackets);
		if (star_brackets)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Some brackets are not properly enclosed! (in lump %s)\n", lump);
			goto closeFile;
		}

		if (star_parseerror)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Stumbled upon a parser error (in lump %s, near line %d)\n", lump, star_line);
			star_parseerror--;
		}
	}

	goto closeFile; // We've made it! Close the file now please :) //

	closeFile:
	{
		M_TokenizerClose();
		fclose(af);
		return;
	}
}
