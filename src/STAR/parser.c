// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parser.c
/// \brief Contains all the Info Portraying to Parsing TSoURDt3rd's Custom STAR Scripts

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

//// VARIABLES ////
INT32 star_lumploading = 0; // is TSoURDt3rd_LoadLump being called?
INT32 star_line = -1; // are we checking for our lines?
INT32 star_brackets = 1; // are we checking for our brackets?

//// FUNCTIONS ////
#if 0
static void MusicDefStrcpy (char *p, const char *s, size_t n)
{
	strlcpy(p, s, n);
	while (( p = strchr(p, '_') ))
		*p++ = ' '; // turn _ into spaces.
}
#endif

// Load a script from a MYFILE
static inline void TSoURDt3rd_LoadFile(MYFILE *f, char *name)
{
	star_lumploading++; // turn on loading flag

	if (!name)
		name = wadfiles[f->wad]->filename;
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Loading STAR script from %s\n", name);
    TSoURDt3rd_ParseScript(f);

	star_line = 0; // clear lines checked
	star_lumploading--; // turn off loading flag
}

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

boolean ValidTerm(const char *string, const char *word)
{
	if (!fasticmp(string, word))
		return false;

	string = M_TokenizerRead(0);
	star_line++;

	if (!fastcmp(string, "{"))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Opening bracket at line %d missing!\n", star_line);
		return false;
	}

	return true;
}

static boolean BracketFound(const char *string)
{
	if (fastcmp(string, "}"))
	{
		star_brackets--;
		return true;
	}
	else if (fastcmp(string, "{"))
	{
		star_brackets++;
		return true;
	}

	return false;
}

static void TSoURDt3rd_Parse(MYFILE *f, void (*function)(MYFILE *, const char *, const char *))
{
	const char *param, *val;

	M_TokenizerSetEndPos(M_TokenizerGetEndPos());
	while (true)
	{
		param = M_TokenizerRead(0);
		star_line++;

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
		if (BracketFound(param))
		{
			if (!star_brackets)
				break;
			else
				continue;
		}
#endif

		val = M_TokenizerRead(1);
		if (function)
		{
			function(f, param, val);
			continue;
		}
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "coming soon! - %s and %s\n", param, val);
	}
}

void TSoURDt3rd_ParseScript(MYFILE *f)
{
	M_TokenizerOpen(f->data);

	char *lump = wadfiles[f->wad]->filename;
	const char *tkn = M_TokenizerRead(0); // skips the initial bracket

	FILE *af;
	const char *path;

	path = va("%s"PATHSEP"%s", srb2home, "STAR_starfilecontents.star");
	af = fopen(path, "w+");

	// Look for namespace at the beginning.
	if (!fastcmp(tkn, "{"))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "No initializing bracket at beginning of lump '%s'!\n", lump);
		M_TokenizerClose();

		return;
	}

	fputs(va("%s\n", tkn), af);
	while ((tkn = M_TokenizerRead(0)) && M_TokenizerGetEndPos() < f->size)
	{
		fputs(va("%s\n", tkn), af);

		// Increment our lines.
		star_line++;

		// Check for brackets.
		if (fastcmp(tkn, "}"))
		{
#if 0
			if (fastcmp(M_TokenizerRead(0), ";") == 0)
			{
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Missing semicolon directly after bracket! (near line %d)\n", star_line);
				M_TokenizerClose();

				return false;
			}
#endif

			star_brackets--;
		}
		else if (fastcmp(tkn, "{"))
			star_brackets++;

		// Check for valid fields.
		else if (ValidTerm(tkn, "JUKEBOXDEF"))
			TSoURDt3rd_Parse(f, TSoURDt3rd_ParseJukeboxDef);
		else if (ValidTerm(tkn, "WINDOWTITLES"))
			TSoURDt3rd_Parse(f, NULL);

		// Found invalid field, skip it.
		else
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Unknown field '%s' in lump '%s'.\n", tkn, lump);

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "BRACKETS: %d\n", star_brackets);
		if (star_brackets)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Some brackets are not properly enclosed! (in lump %s)\n", lump);
			M_TokenizerClose();

			return;
		}
	}

	M_TokenizerClose();
	fclose(af);
}
