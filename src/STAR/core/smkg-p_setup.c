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

#include "../parser/smkg-ps_main.h"
#include "../smkg-misc.h" // TSoURDt3rd_M_FindWordInTermTable() //
#include "../smkg-cvars.h" // cv_tsourdt3rd_game_loadingscreen vars //
#include "../ss_main.h" // tsourdt3rd_loadingscreen //
#include "../star_vars.h" // STAR_LoadingScreen() //

#include "../../i_video.h" // rendermode
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum tsourdt3rd_lump_term_e
{
	tsourdt3rd_lump_jukedef,
	tsourdt3rd_lump_windef,
	tsourdt3rd_lump_exmusdef,
};

static const char *const tsourdt3rd_lump_term_opt[] = {
	"JUKEDEF",
	"WINDEF",
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
	tsourdt3rd_starparser_num_brackets = 0;
	tsourdt3rd_starparser_num_errors = 0;

	script->wad = wad;
	if (wad == NULL || script->wad == NULL)
		return false;

	for (i = 0; i < numlumps; i++, lump_p++)
	{
		INT32 lump_found = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_lump_term_opt, lump_p->name, TSOURDT3RD_TERMTABLESEARCH_MEMCMP);

		if (lump_found < 0)
			continue;

		lumpData = (char *)W_CacheLumpNumPwad(wadnum, i, PU_STATIC);
		lumpLength = W_LumpLengthPwad(wadnum, i);
		text = (char *)Z_Malloc((lumpLength + 1), PU_STATIC, NULL);

		memmove(text, lumpData, lumpLength);
		text[lumpLength] = '\0';

		switch (lump_found)
		{
			case tsourdt3rd_lump_jukedef:
				S_LoadMusicDefs(numwadfiles-1);
				if (!TSoURDt3rd_Jukebox_PrepareDefs())
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Failed to prepare Jukebox, not reading '\x82%s\x80'!\n", lump_p->name);
					break;
				}
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Reading '\x82%s\x80' (from wad '\x82%s\x80')\n", lump_p->name, script->wad->filename);
				TSoURDt3rd_STARParser_Read(script, text, lumpLength, TSoURDt3rd_STARParser_JukeDefs);
				Z_Free(tsourdt3rd_jukebox_defs);
				tsourdt3rd_jukebox_defs = NULL;
				break;
			default:
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Parser for lump '%s' doesn't exist yet!\n", tsourdt3rd_lump_term_opt[lump_found]);
				break;
		}

		Z_Free(lumpData);
		Z_Free((void *)text);
	}

	Z_Free(script);
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
