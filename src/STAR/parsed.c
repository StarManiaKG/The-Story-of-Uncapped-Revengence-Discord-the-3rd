// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parsed.c
/// \brief Contains STAR script property parsing functions

#include <time.h>

#include "parser.h"

// ------------------------ //
//         Structs
// ------------------------ //

typedef struct test_s {
	INT32 page;
	char pageName[32];

	struct test_s *next;
} test_t;

test_t testMusic = {
	1,
	"Main",
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

//#define JUKEDEF_MEM

boolean TSoURDt3rd_ParseJukeboxDef(const char *word, const char *value)
{
	INT32 ivalue;

#ifdef JUKEDEF_MEM
	musicdef_t **defp;
	musicdef_t *def;

	test_t **jukep;
	test_t *juke;
#else
	musicdef_t *def = NULL;
	test_t *juke = NULL;
#endif

	if (STAR_Script_ValidTerm(word, value, "PAGETITLES"))
	{
		test_t *prev = NULL;
		juke = &testMusic;

		do
		{
			while (juke)
			{
				if (!stricmp(juke->pageName, value))
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Page %s, %s, already exists! Replacing contents...\n", word, juke->pageName);

					STRBUFCPY(juke->pageName, value);
					strlwr(juke->pageName);

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

				ivalue = (INT32)atoi(word);
				juke->page = ivalue;

				if (prev != NULL)
					prev->next = juke;
			}

			word = STAR_Script_ReadNewLine(0);
#ifdef JUKEDEF_MEM
			(*jukep) = juke;
#endif

			if (!STAR_Script_ValidTerm(word, value, ","))
				break;
		} while (true); //while (STAR_Script_ValidTerm(word, value, ",")); // run until I say we're done :)
	}
	else if (STAR_Script_ValidTerm(word, value, "LUMP"))
	{
		//value = word;
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "made it here, %s, %s\n", word, value);

		if (!value)
		{
			STAR_Script_Error(va("JUKEBOXDEF: Field '%s' is missing name.\n", word), STAR_SCRIPT_ERROR_FULL);
			return false;
		}
		else
		{
			musicdef_t *prev = NULL;
			def = musicdefstart;

			// Check if this is a replacement
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

			if (def) // Found it!
			{
				def = Z_Calloc(sizeof (musicdef_t), PU_STATIC, NULL);
				STRBUFCPY(def->name, value);
				strlwr(def->name);
				def->bpm = TICRATE<<(FRACBITS-1); // FixedDiv((60*TICRATE)<<FRACBITS, 120<<FRACBITS)
				if (prev != NULL)
					prev->next = def;
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Replaced song '%s'\n", def->name);
#ifdef JUKEDEF_MEM
				(*defp) = def;
#endif
			}
			else // Nothing found, return an error.
			{
				STAR_Script_Error(va("JUKEBOXDEF: Lump '%s' must already have a MUSICDEF!\n", value), STAR_SCRIPT_ERROR_STANDARD);
				return false;
			}
		}
	}
	else
	{
        STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "made it here instead, %s + %s\n", word, value);

		if (!value)
		{
			STAR_Script_Error(va("JUKEBOXDEF: Field '%s' is missing value.", word), STAR_SCRIPT_ERROR_FULL);
			return false;
		}
		else
		{
#ifdef JUKEDEF_MEM
			def = (*defp);
#endif

			if (!def)
			{
				STAR_Script_Error(va("JUKEBOXDEF: No music definition before field '%s'.", word), STAR_SCRIPT_ERROR_FULL);
				return false;
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
				STAR_Script_Error(va("JUKEBOXDEF: Invalid field '%s'.", word), STAR_SCRIPT_ERROR_FULL);
			}
		}
	}

	return true;
}
