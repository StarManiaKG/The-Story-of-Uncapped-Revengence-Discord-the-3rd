// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_setup.c
/// \brief Unique TSoURDt3rd WAD I/O and map setup routines

#include "smkg-p_setup.h"

#include "smkg-s_exmusic.h"
#include "smkg-s_jukebox.h"
#include "../smkg-cvars.h" // cv_tsourdt3rd_game_loadingscreen vars //
#include "../ss_main.h" // tsourdt3rd_loadingscreen //
#include "../star_vars.h" // STAR_LoadingScreen() //
#include "../menus/smkg-m_sys.h"
#include "../misc/smkg-m_misc.h" // TSoURDt3rd_M_FindWordInTermTable() //
#include "../parser/smkg-ps_main.h"

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
}; static const char *const tsourdt3rd_lump_term_opt[] = {
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
	UINT16 loaded_mod = 0;

	wadfile_t *wad = wadfiles[wadnum];
	lumpinfo_t *lump_p = wad->lumpinfo;

	char *lumpData = NULL;
	size_t lumpLength = 0;
	char *text = NULL;

	tsourdt3rd_starparser_lump_loading = 0;
	tsourdt3rd_starparser_num_errored_lumps = 0;

	if (script == NULL || wad == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_P_LoadAddon(): Couldn't setup extra lump parser, exiting!\n");
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

	for (UINT16 lump = 0; lump < numlumps; lump++, lump_p++)
	{
		INT32 lump_found = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_lump_term_opt, lump_p->name, TSOURDT3RD_TERMTABLESEARCH_MEMCMP);

		if (lump_found < 0)
			continue;
		tsourdt3rd_starparser_lump_loading++; // turn on loading flag
		tsourdt3rd_starparser_num_brackets = 0;
		tsourdt3rd_starparser_num_errors = 0;

		lumpLength = W_LumpLengthPwad(wadnum, lump);
		text = Z_Malloc(lumpLength + 1, PU_STATIC, NULL);
		text[lumpLength] = '\0';

		lumpData = (char *)W_CacheLumpNumPwad(wadnum, lump, PU_STATIC);
		M_Memcpy(text, lumpData, lumpLength);

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "\nReading \x82\"%s\"\x80 (from wad \x82\"%s\"\x80)\n", lump_p->name, script->wad->filename);
		switch (lump_found)
		{
			case tsourdt3rd_lump_jukedef:
				if (tsourdt3rd_global_jukebox == NULL || !TSoURDt3rd_Jukebox_PrepareDefs())
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "Failed to prepare Jukebox, not reading \x82\"%s\"\x80!\n", lump_p->name);
					break;
				}
				{
					TSoURDt3rd_STARParser_Read(script, text, lumpLength, TSoURDt3rd_STARParser_JUKEDEF);
					if (tsourdt3rd_global_jukebox->in_menu == false)
					{
						Z_Free(tsourdt3rd_jukebox_defs);
						tsourdt3rd_jukebox_defs = NULL;
					}
				}
				break;

			case tsourdt3rd_lump_exmusdef:
				if (tsourdt3rd_global_exmusic == NULL)
				{
					STAR_CONS_Printf(STAR_CONS_DEBUG, "EXMusic wasn't properly initialized at startup!\n");
					break;
				}
				{
					S_InitMusicDefs(); // Just in case we're doing this while the game's initializing...
					TSoURDt3rd_STARParser_Read(script, text, lumpLength, TSoURDt3rd_STARParser_EXMUSDEF);
					numsoundtestdefs = 0;
				}
				break;

			default:
				STAR_CONS_Printf(STAR_CONS_DEBUG, "Parser for lump '%s' doesn't exist yet!\n", tsourdt3rd_lump_term_opt[lump_found]);
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
			else //if (tsourdt3rd_starparser_num_brackets < 0)
				TSoURDt3rd_STARParser_Error("Bracket enclosure '}' has been misplaced somewhere!", script, TSOURDT3RD_STARPARSER_ERROR_LUMP);
		}
		if (tsourdt3rd_starparser_num_errors)
			tsourdt3rd_starparser_num_errored_lumps++;

		tsourdt3rd_starparser_lump_loading--; // turn off loading flag
		loaded_mod--;
	}

	// Have we run into any errors?
	const char *wadname = TSoURDt3rd_M_WriteVariedLengthString(wad->filename, MENUMESSAGEHEADERLEN, true);
	if (tsourdt3rd_starparser_num_errored_lumps > 1)
	{
		TSoURDt3rd_M_StartMessage(wadname,
			va("%d of the lumps you've loaded\nhave encountered errors!\nCheck the logs for more information.\n", tsourdt3rd_starparser_num_errored_lumps),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, va("%d of the lumps you've loaded have encountered errors! Check the logs for more information.\n", tsourdt3rd_starparser_num_errored_lumps));
		S_StartSoundFromEverywhere(sfx_skid);
	}
	else if (tsourdt3rd_starparser_num_errors)
	{
		TSoURDt3rd_M_StartMessage(wadname,
			va("Stumbled upon\n%d parser error(s)\nwithin this lump!\n", tsourdt3rd_starparser_num_errors),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, va("Stumbled upon %d parser error(s) within this lump!\n", tsourdt3rd_starparser_num_errors));
		S_StartSoundFromEverywhere(sfx_skid);
	}
	if (loaded_mod >= 1)
		STAR_CONS_Printf(STAR_CONS_NONE, "\n");

	// Close and free our parser!
	tsourdt3rd_starparser_lump_loading = 0;
	tsourdt3rd_starparser_num_brackets = 0;
	tsourdt3rd_starparser_num_errors = 0;
	tsourdt3rd_starparser_num_errored_lumps = 0;
	Z_Free(script); script = NULL;
	return true;
}

//
// boolean TSoURDt3rd_WORLD_MapIsDangerous(mapheader_t *map)
// Checks if the given map meets the requirements to be considered "dangerous."
//
// I.E, the map has a boss, goes to a hardcoded level number, or has specific parameters.
//
boolean TSoURDt3rd_WORLD_MapIsDangerous(mapheader_t *map)
{
	switch (map->nextlevel)
	{
		case 1101:
		case 1102:
		case 1103:
			return true;
		default:
			break;
	}
	return (map->bonustype == 2 || (map->typeoflevel & TOL_ERZ3) || (map->levelflags & LF_WARNINGTITLE));
}

//
// void TSoURDt3rd_WORLD_UpdateScenarios(void)
// Updates the current scenario for our level's world.
//
void TSoURDt3rd_WORLD_UpdateScenarios(void)
{
	tsourdt3rd_world_scenarios_t scenario = TSOURDT3RD_WORLD_SCENARIO_NORMAL;
	tsourdt3rd_world_scenarios_types_t scenario_type = TSOURDT3RD_WORLD_SCENARIO_TYPES_NONE;

	mapheader_t *map = mapheaderinfo[gamemap-1];

	memset(&tsourdt3rd_local.world, 0, sizeof(tsourdt3rd_local.world));

	// Fighting the big bad guy!
	if (map)
	{
		mobj_t *boss_mobj = TSoURDt3rd_BossInMap();

		if (boss_mobj && (map->levelflags & LF_WARNINGTITLE))
		{
			INT32 boss_pinch_health = (boss_mobj->info->damage ? boss_mobj->info->damage : 3);

			if (boss_mobj->type == MT_METALSONIC_RACE)
			{
				// We gotta move! (With the groove!)
				scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_RACE;
			}

			if (boss_mobj->health <= boss_pinch_health)
			{
				// Ooh, nice pinch phase you got there!
				scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH;

				if (boss_mobj->health <= 0)
				{
					// Cool, the boss is dead, we can go home now!
					scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_POSTBOSS;
				}
			}

			scenario |= TSOURDT3RD_WORLD_SCENARIO_BOSS; // Nice boss you got there!
		}
		else if (map->bonustype == 1)
		{
			// Nice boss you got there!
			scenario |= TSOURDT3RD_WORLD_SCENARIO_BOSS;
		}

		if (TSoURDt3rd_WORLD_MapIsDangerous(map))
		{
			// You made it!
			// So, there's multiple things that can trigger a 'final boss' type of stage.
			// There's the type of level, and the nextlevel, with 1101 being Evaluation, 1102 being the Credits, and 1103 being the ending cutscene.
			// ERZ3 bonustypes, ERZ3 typeoflevels, or level warning titles can bring you here too!
			if (map->bonustype == 2 || (map->typeoflevel & TOL_ERZ3)
				|| map->nextlevel == 1100 || map->nextlevel == 1101 || map->nextlevel == 1102 || map->nextlevel == 1103)
			{
				if (ALL7EMERALDS(emeralds)) scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS;
				scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS;
			}
			scenario |= TSOURDT3RD_WORLD_SCENARIO_BOSS;
		}
	}

	// Check for downtimes and intermissions.
	if (gamestate == GS_INTERMISSION || gamestate == GS_EVALUATION || gamestate == GS_GAMEEND)
	{
		if (gamestate == GS_EVALUATION || gamestate == GS_GAMEEND)
		{
			// End of super boss!
			if (ALL7EMERALDS(emeralds)) scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS;
			scenario_type |= TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS;
		}
		scenario |= TSOURDT3RD_WORLD_SCENARIO_INTERMISSION; // End of chapter!
	}

	// Set our new world scenario!
	tsourdt3rd_local.world.scenario = scenario;
	tsourdt3rd_local.world.scenario_types = scenario_type;
}

//
// void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate)
// Loads various bits of level data, exclusively for TSoURDt3rd.
//
void TSoURDt3rd_P_LoadLevel(boolean reloadinggamestate)
{
	mapheader_t *map = mapheaderinfo[gamemap-1];

	tsourdt3rd_loadingscreen.loadCount = tsourdt3rd_loadingscreen.loadPercentage = 0; // reset loading status
	tsourdt3rd_loadingscreen.bspCount = 0; // reset bsp count
	tsourdt3rd_loadingscreen.loadComplete = false; // reset loading finale

#ifdef HAVE_SDL
	STAR_SetWindowTitle();
#endif
	TSoURDt3rd_FIL_CreateSavefileProperly();
	TSoURDt3rd_WORLD_UpdateScenarios();

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

		// Do music...
		const char *determinedMusic = TSoURDt3rd_DetermineLevelMusic();
		if (determinedMusic && strnicmp(S_MusicName(), ((mapmusflags & MUSIC_RELOADRESET) ? map->musname : determinedMusic), 7))
		{
			strncpy(mapmusname, determinedMusic, 7);
			mapmusname[6] = 0;
#if 0
			if (strnicmp(determinedMusic, map->musname, 7))
#endif
			{
				mapmusflags = (map->mustrack & MUSIC_TRACKMASK);
				mapmusposition = map->muspos;
			}
		}

		// Also fade the music, by the way...
		if (RESETMUSIC || strnicmp(S_MusicName(), (mapmusflags & MUSIC_RELOADRESET) ? map->musname : mapmusname, 7))
		{
			S_FadeMusic(0, FixedMul(
				FixedDiv((F_GetWipeLength(wipedefs[wipe_level_toblack])-2)*NEWTICRATERATIO, NEWTICRATE), MUSICRATE));
		}

		// Finally, set the music...
		S_Start();
	}
}
