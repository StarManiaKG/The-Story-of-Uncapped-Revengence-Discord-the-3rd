// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parsed.c
/// \brief Contains all Info Related to Helping Parse Specific Info Within TSoURDt3rd's Custom STAR Scripts

#include <time.h>

#include "parsed.h"
#include "ss_main.h" // STAR_CONS_Printf() //
#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../s_sound.h"
#include "../fastcmp.h"

typedef struct test_s {
	INT32 page;
	char pageName[32];

	struct test_s *next;
} test_t;

test_t testMusic = {
	0,
	"Main",
	NULL
};

test_t *jukedefstart = &testMusic;

void TSoURDt3rd_ParseJukeboxDef(MYFILE *f, const char *word, const char *value)
{
	musicdef_t **defp = NULL;
	musicdef_t *def = NULL;

	test_t **jukeTest = NULL;
	test_t *juke = NULL;

	(void)defp;
	(void)jukeTest;

	// PAGETITLES
	if (ValidTerm(word, "PAGETITLES"))
	{
		test_t *prev = NULL;
		juke = jukedefstart;

		while (juke)
		{
			if (!stricmp(juke->pageName, value))
			{
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Found page replacement '%s'\n", juke->pageName);
				break;
			}

			prev = juke;
			juke = juke->next;
		}

		if (!juke)
		{
			juke = Z_Calloc(sizeof (test_t), PU_STATIC, NULL);
			STRBUFCPY(juke->pageName, value);
			strlwr(juke->pageName);
			if (prev != NULL)
				prev->next = juke;
		}
		else
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Page %s already exists! Replacing contents...\n", word);

			STRBUFCPY(juke->pageName, value);
			strlwr(juke->pageName);
		}

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "\nTEST: %s, %s\nHI: %s\n", word, value, juke->pageName);
		//(*jukeTest) = juke;
	}

	// LUMP
	else if (ValidTerm(word, "LUMP"))
	{
		//value = word;
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "made it here, %s, %s\n", word, value);

		if (!value)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT,
					"JUKEBOXDEF: Field '%s' is missing name. (file %s, near line %d)\n",
					word, wadfiles[f->wad]->filename, star_line);
			return;
		}
		else
		{
			musicdef_t *prev = NULL;
			def = musicdefstart;

			// Search if this is a replacement
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Searching for song replacement...\n");
			while (def)
			{
				if (fasticmp(def->name, value))
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Found song replacement '%s'\n", def->name);
					break;
				}

				prev = def;
				def = def->next;
			}

			// Nothing found, return an error.
			if (!def)
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Lump '%s' must already have a MUSICDEF!\n", value);

			(void)prev;
		}
	}

	// Everything Else
	else
	{
        STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "made it here instead, %s\n", word);

		if (!value)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT,
					"JUKEBOXDEF: Field '%s' is missing value. (file %s, line %d)\n",
					word, wadfiles[f->wad]->filename, star_line);
			return;
		}
		else
		{
			if (!def)
			{
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT,
						"JUKEBOXDEF: No music definition before field '%s'. (file %s, line %d)\n",
						word, wadfiles[f->wad]->filename, star_line);
				return;
			}

			if (fasticmp(word, "title"))
            {
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "title - %s, %s\n", word, value);
#if 0				
				MusicDefStrcpy(def->title, textline,
						sizeof def->title, version);
			} else if (stricmp(stoken, "alttitle") == 0) {
				MusicDefStrcpy(def->alttitle, textline,
						sizeof def->alttitle, version);
			} else if (stricmp(stoken, "authors") == 0) {
				MusicDefStrcpy(def->authors, textline,
						sizeof def->authors, version);
			} else if (stricmp(stoken, "soundtestpage" == 0)) {
				def->soundtestpage = (UINT8)atoi(value);
			} else if (stricmp(stoken, "soundtestcond") == 0) {
				// Convert to map number
				if (textline[0] >= 'A' && textline[0] <= 'Z' && textline[2] == '\0')
					value = M_MapNumber(textline[0], textline[1]);
				def->soundtestcond = (INT16)value;
			} else if (stricmp(stoken, "stoppingtime") == 0) {
				double stoppingtime = atof(textline)*TICRATE;
				def->stoppingtics = (tic_t)stoppingtime;
			} else if (stricmp(stoken, "bpm") == 0) {
				double bpm = atof(textline);
				fixed_t bpmf = FLOAT_TO_FIXED(bpm);
				if (bpmf > 0)
					def->bpm = FixedDiv((60*TICRATE)<<FRACBITS, bpmf);
			} else if (stricmp(stoken, "loopms") == 0) {
				def->loop_ms = atoi(textline);
#endif
			} else {
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT,
						"JUKEBOXDEF: Invalid field '%s'. (file %s, line %d)\n",
						word, wadfiles[f->wad]->filename, star_line);
			}
		}
	}
}
