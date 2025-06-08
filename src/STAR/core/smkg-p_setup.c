// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_setup.c
/// \brief Unique TSoURDt3rd WAD I/O and map setup routines

#include "smkg-p_setup.h"
#include "../menus/smkg-m_sys.h"
#include "../parser/smkg-ps_main.h"
#include "../smkg-misc.h" // TSoURDt3rd_M_FindWordInTermTable() //
#include "../smkg-cvars.h" // cv_tsourdt3rd_game_loadingscreen vars //
#include "../ss_main.h" // tsourdt3rd_loadingscreen //
#include "../star_vars.h" // STAR_LoadingScreen() //

#include "../../f_finale.h"
#include "../../i_video.h" // rendermode
#include "../../p_local.h"
#include "../../r_state.h" // level data
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum tsourdt3rd_lump_term_e
{
	tsourdt3rd_lump_jukedef,
	tsourdt3rd_lump_exmusdef,
};
static const char *const tsourdt3rd_lump_term_opt[] = {
	"JUKEDEF",
	"EXMUSDEF",
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_P_LoadAddon(INT32 wadnum, INT32 numlumps)
// Loads addons using unique TSoURDt3rd data and structures.
//
boolean TSoURDt3rd_P_LoadAddon(INT32 wadnum, INT32 numlumps)
{
	tsourdt3rd_starparser_t *script = Z_Malloc(sizeof(tsourdt3rd_starparser_t), PU_STATIC, NULL);

	UINT16 i;
	wadfile_t *wad = wadfiles[wadnum];
	lumpinfo_t *lump_p = wad->lumpinfo;

	char *lumpData = NULL;
	size_t lumpLength = 0;
	char *text = NULL;

	tsourdt3rd_starparser_lump_loading = 0;
	tsourdt3rd_starparser_num_errored_lumps = 0;

	if (script == NULL || wad == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "TSoURDt3rd_P_LoadAddon(): Couldn't setup extra lump parser, existing!\n");
		if (script)
		{
			Z_Free(script);
			script = NULL;
		}
		return false;
	}
	script->wad = wad;
	script->tokenizer = NULL;
	script->tkn = script->val = NULL;

	for (i = 0; i < numlumps; i++, lump_p++)
	{
		INT32 lump_found = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_lump_term_opt, lump_p->name, TSOURDT3RD_TERMTABLESEARCH_MEMCMP);

		if (lump_found < 0)
			continue;

		tsourdt3rd_starparser_lump_loading++; // turn on loading flag
		tsourdt3rd_starparser_num_brackets = 0;
		tsourdt3rd_starparser_num_errors = 0;

		S_LoadMusicDefs(wadnum);

		lumpData = (char *)W_CacheLumpNumPwad(wadnum, i, PU_STATIC);
		lumpLength = W_LumpLengthPwad(wadnum, i);
		text = (char *)Z_Malloc((lumpLength + 1), PU_STATIC, NULL);

		memmove(text, lumpData, lumpLength);
		text[lumpLength] = '\0';

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Reading \x82\"%s\"\x80 (from wad \x82\"%s\"\x80)\n", lump_p->name, script->wad->filename);
		switch (lump_found)
		{
			case tsourdt3rd_lump_jukedef:
				if (tsourdt3rd_global_jukebox == NULL)
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "The Jukebox wasn't initialized, so not reading \x82\"%s\"\x80!\n", lump_p->name);
					break;
				}
				else if (!TSoURDt3rd_Jukebox_PrepareDefs())
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Failed to prepare Jukebox, so not reading \x82\"%s\"\x80!\n", lump_p->name);
					break;
				}
				TSoURDt3rd_STARParser_Read(script, text, lumpLength, TSoURDt3rd_STARParser_JUKEDEF);
				if (tsourdt3rd_global_jukebox->in_menu == false)
				{
					Z_Free(tsourdt3rd_jukebox_defs);
					tsourdt3rd_jukebox_defs = NULL;
				}
				break;
			case tsourdt3rd_lump_exmusdef:
				S_InitMusicDefs(); // Just in case we're doing this while the game's initializing...
				TSoURDt3rd_STARParser_Read(script, text, lumpLength, TSoURDt3rd_STARParser_EXMUSDEF);
				numsoundtestdefs = 0;
				break;
			default:
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Parser for lump '%s' doesn't exist yet!\n", tsourdt3rd_lump_term_opt[lump_found]);
				break;
		}

		Z_Free(lumpData);
		Z_Free((void *)text);

		if (script->tokenizer != NULL)
		{
			Tokenizer_Close(script->tokenizer);
			script->tokenizer = NULL;
		}

		// Now check for errors.
		if (tsourdt3rd_starparser_num_brackets != 0)
		{
			// Brackets
			if (tsourdt3rd_starparser_num_brackets > 0)
				TSoURDt3rd_STARParser_Error("Some brackets are not properly enclosed!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
			else if (tsourdt3rd_starparser_num_brackets < 0)
				TSoURDt3rd_STARParser_Error("Bracket enclosure '}' has been misplaced somewhere!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		}
		if (tsourdt3rd_starparser_num_errors)
			tsourdt3rd_starparser_num_errored_lumps++;

		tsourdt3rd_starparser_lump_loading--; // turn off loading flag
	}

	// Have we run into any errors?
	if (tsourdt3rd_starparser_num_errored_lumps > 1)
	{
		TSoURDt3rd_M_StartMessage(
			TSoURDt3rd_M_WriteVariedLengthString(wad->filename, true),
			va("%d of the lumps you've loaded\nhave encountered errors!\nCheck the logs for more information.\n", tsourdt3rd_starparser_num_errored_lumps),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, va("%d of the lumps you've loaded have encountered errors! Check the logs for more information.\n", tsourdt3rd_starparser_num_errored_lumps));
		S_StartSound(NULL, sfx_skid);
	}
	else if (tsourdt3rd_starparser_num_errors)
	{
		TSoURDt3rd_M_StartMessage(
			TSoURDt3rd_M_WriteVariedLengthString(wad->filename, true),
			va("Stumbled upon\n%d parser error(s)\nwithin this lump!\n", tsourdt3rd_starparser_num_errors),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, va("Stumbled upon %d parser error(s) within this lump!\n", tsourdt3rd_starparser_num_errors));
		S_StartSound(NULL, sfx_skid);
	}

	tsourdt3rd_starparser_lump_loading = 0;
	tsourdt3rd_starparser_num_errored_lumps = 0;
	tsourdt3rd_starparser_num_brackets = 0;
	tsourdt3rd_starparser_num_errors = 0;

	Z_Free(script);
	script = NULL;
	return true;
}

//
// void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate)
// Loads various bits of level data, exclusively for TSoURDt3rd.
//
void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate)
{
	const char *determinedMusic = TSoURDt3rd_DetermineLevelMusic();

	tsourdt3rd_loadingscreen.loadCount = tsourdt3rd_loadingscreen.loadPercentage = 0; // reset loading status
	tsourdt3rd_loadingscreen.bspCount = 0; // reset bsp count
	tsourdt3rd_loadingscreen.loadComplete = false; // reset loading finale

#ifdef HAVE_SDL
	STAR_SetWindowTitle();
#endif
	TSoURDt3rd_FIL_CreateSavefileProperly();

	if (!(reloadinggamestate || titlemapinaction))
	{
		// Display a loading screen...
		if (rendermode != render_none)
		{
			if (cv_tsourdt3rd_game_loadingscreen.value && tsourdt3rd_loadingscreen.loadCount-- <= 0 && !tsourdt3rd_loadingscreen.loadComplete)
			{
				while (tsourdt3rd_loadingscreen.bspCount != 1 && (((tsourdt3rd_loadingscreen.loadPercentage)<<1) < 100) && rendermode == render_soft)
				{
					tsourdt3rd_loadingscreen.loadCount = numsubsectors/50;
					STAR_LoadingScreen();
				}

				tsourdt3rd_loadingscreen.loadCount = tsourdt3rd_loadingscreen.loadPercentage = 0; // reset the loading status
				tsourdt3rd_loadingscreen.screenToUse = 0; // reset the loading screen to use
				tsourdt3rd_loadingscreen.loadComplete = true; // loading... load complete.
			}
		}

		// Change the music :)
		if (strnicmp(S_MusicName(),
			((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : determinedMusic), 7))
		{
			strncpy(mapmusname, determinedMusic, 7);

			mapmusname[6] = 0;
			mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
			mapmusposition = mapheaderinfo[gamemap-1]->muspos;
		}

		// Fade music, by the way.
		if (RESETMUSIC || strnicmp(S_MusicName(),
			(mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname, 7))
		{
			S_FadeMusic(0, FixedMul(
				FixedDiv((F_GetWipeLength(wipedefs[wipe_level_toblack])-2)*NEWTICRATERATIO, NEWTICRATE), MUSICRATE));
		}

		// Set the music.
		S_Start();
	}
}
