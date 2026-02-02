// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_exmusic.c
/// \brief TSoURDt3rd's cool and groovy EXtended Music setup, definition, and replacement routines

#include "smkg-s_exmusic.h"

#include "smkg-s_audio.h"
#include "smkg-s_jukebox.h"
#include "../smkg-cvars.h"
#include "../ss_main.h"
#include "../core/smkg-g_game.h" // tsourdt3rd_local //

#include "../../m_misc.h"
#include "../../y_inter.h"
#include "../../z_zone.h"

//
// Needed for console command stuffs.
//
static tsourdt3rd_exmusic_findTrackResult_t exm_last_find_track_result;
static consvar_t *prev_exm_cvar = NULL;
static INT32 prev_exm_pos = 0;
static char *prev_exm_string = NULL;

//
// Sets the default names for our EXMusic starting series.
// The amount here should be corresponded to the value of 'tsourdt3rd_exmusic_starting_series_max'.
//
const char *tsourdt3rd_exmusic_default_series_names[tsourdt3rd_exmusic_starting_series_max] = {
	"Default",
	//"Random",
};

//
// The basic identifiers for all our EXMusic types.
//
tsourdt3rd_exmusic_data_identifiers_t tsourdt3rd_exmusic_data_identifier_types[tsourdt3rd_exmusic_max_types] = {
	{ "Default Map Tracks", "defaultmaptrack" },
	{ "Game Over", "gameover" },
	{ "Bosses", "boss_theme" },
		{ "Boss Pinch", "boss_pinch_theme" },
		{ "Final Boss", "final_boss_theme" },
		{ "Final Boss Pinch", "final_boss_pinch_theme" },
		{ "True Final Boss", "true_final_boss_theme" },
		{ "True Final Boss Pinch", "true_final_boss_pinch_theme" },
		{ "Race", "race_theme" }, // race
	{ "Intermission", "intermission" },
		{ "Boss Intermission", "boss" }, // intermission_boss
		{ "Final Boss Intermission", "final_boss" }, // intermission_final_boss
		{ "True Final Boss Intermission", "true_final_boss" } // intermission_true_final_boss
};

//
// Our global EXMusic structs.
// Aren't they so cool?
//
tsourdt3rd_exmusic_data_series_t **tsourdt3rd_exmusic_available_series = NULL;
INT32 tsourdt3rd_exmusic_num_series = 0;
boolean tsourdt3rd_exmusic_initialized = false;

// ===============================================================
//
// ===============================================================

tsourdt3rd_exmusic_data_series_t *TSoURDt3rd_EXMusic_AddNewSeries(const char *name, boolean verbose)
{
	tsourdt3rd_exmusic_data_series_t *exm_new_series = NULL;
	tsourdt3rd_exmusic_data_series_t *exm_prev_series = NULL;
	INT32 series_num = 0;

	if (name == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_AddNewSeries(): No series name given!\n");
		return NULL;
	}

	if (tsourdt3rd_exmusic_num_series)
	{
		exm_new_series = TSoURDt3rd_EXMusic_FindSeries(name, &series_num);
		if (series_num > 0)
		{
			exm_prev_series = tsourdt3rd_exmusic_available_series[series_num-1];
		}
	}

	if (exm_new_series == NULL)
	{
		tsourdt3rd_exmusic_available_series = (tsourdt3rd_exmusic_data_series_t **)Z_Realloc(tsourdt3rd_exmusic_available_series, ((tsourdt3rd_exmusic_num_series+1) * sizeof(*tsourdt3rd_exmusic_available_series)), PU_STATIC, NULL);

		exm_new_series = Z_Calloc(sizeof(*exm_new_series), PU_STATIC, NULL);
		exm_new_series->hardcoded = (series_num < tsourdt3rd_exmusic_starting_series_max);
		strlcpy(exm_new_series->series_name, name, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

		for (INT32 i = 0; i < tsourdt3rd_exmusic_max_types; i++)
		{
			exm_new_series->track_sets[i] = Z_Calloc(sizeof(tsourdt3rd_exmusic_musicset_t), PU_STATIC, NULL);
			exm_new_series->track_sets[i]->music = Z_Calloc(sizeof(*exm_new_series->track_sets[i]->music), PU_STATIC, NULL);
			if (exm_new_series->hardcoded)
			{
				exm_new_series->track_sets[i]->num_music_lumps = 1;
				exm_new_series->track_sets[i]->music[0] = &soundtestsfx;
			}
			else
			{
				exm_new_series->track_sets[i]->num_music_lumps = 0;
			}
		}

		if (exm_prev_series != NULL)
		{
			exm_new_series->prev = exm_prev_series;
			exm_prev_series->next = exm_new_series;
		}

		if (verbose)
		{
			STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd EXMusic: Created series \x82\"%s\"\x80!\n", exm_new_series->series_name);
		}

		tsourdt3rd_exmusic_num_series++;
	}
	else
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "TSoURDt3rd EXMusic: Series \x82\"%s\"\x80 already exists!\n", name);
	}

	tsourdt3rd_exmusic_available_series[series_num] = exm_new_series;
	return exm_new_series;
}

//
// void TSoURDt3rd_EXMusic_Init(void)
// Initializes our cool EXMusic system!
//
void TSoURDt3rd_EXMusic_Init(void)
{
	INT32 i;

	tsourdt3rd_exmusic_initialized = false;
	tsourdt3rd_exmusic_available_series = NULL;
	tsourdt3rd_exmusic_num_series = 0;

	for (i = 0; i < tsourdt3rd_exmusic_starting_series_max; i++)
	{
		if (TSoURDt3rd_EXMusic_AddNewSeries(tsourdt3rd_exmusic_default_series_names[i], false) == NULL)
		{
			STAR_CONS_Printf(STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_Init() - EXMusic wasn't properly initialized!\n");
			if (tsourdt3rd_exmusic_available_series)
				Z_Free(tsourdt3rd_exmusic_available_series);
			tsourdt3rd_exmusic_available_series = NULL;
			return;
		}
	}

	tsourdt3rd_exmusic_initialized = true;
	STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd_EXMusic_Init()...\n");
}

tsourdt3rd_exmusic_data_identifiers_t *TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(const char *identifier, INT32 *identifier_p)
{
	tsourdt3rd_exmusic_data_identifiers_t *cur_extype;
	INT32 identifier_num;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier() - EXMusic wasn't initialized!\n");
		return NULL;
	}
	else if (identifier == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier() - No valid identifier was given!\n");
		return NULL;
	}

	for (identifier_num = 0; identifier_num < tsourdt3rd_exmusic_max_types; identifier_num++)
	{
		cur_extype = &tsourdt3rd_exmusic_data_identifier_types[identifier_num];
		if (cur_extype == NULL)
			continue;
		if (!stricmp(cur_extype->type_name, identifier) || !stricmp(cur_extype->parser_name, identifier))
		{
			if (identifier_p) (*identifier_p) = identifier_num;
			return cur_extype;
		}
	}
	return NULL;
}

tsourdt3rd_exmusic_data_identifiers_t *TSoURDt3rd_EXMusic_ReturnTypeFromCVar(consvar_t *cvar, INT32 *identifier_p)
{
	consvar_t *exm_cvar;
	INT32 identifier_num;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromCVar() - EXMusic wasn't initialized!\n");
		return NULL;
	}
	else if (cvar == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromCVar() - No CVar found!\n");
		return NULL;
	}

	for (identifier_num = 0; identifier_num < tsourdt3rd_exmusic_max_types; identifier_num++)
	{
		exm_cvar = &cv_tsourdt3rd_audio_exmusic[identifier_num];
		if (!stricmp(exm_cvar->name, cvar->name) || exm_cvar == cvar)
		{
			if (identifier_p) (*identifier_p) = identifier_num;
			return &tsourdt3rd_exmusic_data_identifier_types[identifier_num];
		}
	}
	return NULL;
}

tsourdt3rd_exmusic_data_series_t *TSoURDt3rd_EXMusic_FindSeries(const char *series, INT32 *series_p)
{
	INT32 series_num = 0;
	tsourdt3rd_exmusic_data_series_t *exm_series = NULL;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_FindSeries() - EXMusic wasn't initialized at startup!\n");
	}
	else if (tsourdt3rd_exmusic_num_series)
	{
		if (M_StringToNumber(series, &series_num))
		{
			exm_series = tsourdt3rd_exmusic_available_series[series_num];
		}
		else
		{
			exm_series = tsourdt3rd_exmusic_available_series[0];
			while (exm_series != NULL && series_num < tsourdt3rd_exmusic_num_series)
			{
				if (!strnicmp(exm_series->series_name, series, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME))
				{
					break;
				}
				else
				{
					series_num++;
					exm_series = exm_series->next;
				}
			}
		}
	}

	if (series_p != NULL)
	{
		(*series_p) = series_num;
	}
	return exm_series;
}

tsourdt3rd_exmusic_musicset_t *TSoURDt3rd_EXMusic_GetSeriesTrackSet(INT32 series, INT32 musicset_type, INT32 *series_p)
{
	const char *series_string = va("%d", series);
	tsourdt3rd_exmusic_data_series_t *exm_series_trackset = TSoURDt3rd_EXMusic_FindSeries(series_string, series_p);

	if (exm_series_trackset == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_GetSeriesTrackSet() - Series given wasn't found!\n");
		return NULL;
	}

	return exm_series_trackset->track_sets[musicset_type];
}

musicdef_t *TSoURDt3rd_EXMusic_GetTrackData(INT32 series, INT32 musicset_type, INT32 music_track, INT32 *series_p)
{
	tsourdt3rd_exmusic_musicset_t *exm_trackset = TSoURDt3rd_EXMusic_GetSeriesTrackSet(series, musicset_type, series_p);

	if (exm_trackset == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_GetTrackData() - Series given wasn't found!\n");
		return NULL;
	}
	else if (exm_trackset->music == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_GetTrackData() - Series given has no music!\n");
		return NULL;
	}

	return exm_trackset->music[music_track];
}

musicdef_t *TSoURDt3rd_EXMusic_GetSeriesMusic(musicdef_t *new_track, INT32 series, INT32 identifier, INT32 track)
{
	tsourdt3rd_exmusic_musicset_t *exm_series_musicset = TSoURDt3rd_EXMusic_GetSeriesTrackSet(series, identifier, NULL);
	musicdef_t **lump = NULL;

	if (exm_series_musicset != NULL)
	{
		lump = &exm_series_musicset->music[track];
		if (lump && new_track)
			(*lump) = new_track;
		return (*lump);
	}
	return NULL;
}

//
// boolean TSoURDt3rd_EXMusic_FindTrack(const char *valstr, INT32 track_set, tsourdt3rd_exmusic_findTrackResult_t *track_result)
// Finds an EXMusic track that we can use, using the value string given.
//
boolean TSoURDt3rd_EXMusic_FindTrack(const char *valstr, INT32 track_set, tsourdt3rd_exmusic_findTrackResult_t *track_result)
{
	char series[TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME], track[32];
	INT32 series_num = 0, music_track = 0;
	INT32 lump_track_num = 0;
	boolean series_found = false;

	tsourdt3rd_exmusic_data_series_t *exm_series = NULL;
	tsourdt3rd_exmusic_musicset_t *exm_series_musicset = NULL;
	musicdef_t *exm_series_trackset_music = NULL;

	INT32 scan = sscanf(valstr, "%29[^:]:%s:%d", series, track, &lump_track_num); // Parse "Series:Index:Track"

	if (scan < 2)
	{
		return false;
	}

#define EXMUSIC_CHECK_VALIDITY(var) if ((var) == NULL) return false;
	if (M_StringToNumber(series, &series_num))
	{
		// -- Find our EXMusic track based on the number.
		M_StringToNumber(track, &music_track);
		EXMUSIC_CHECK_VALIDITY(exm_series = tsourdt3rd_exmusic_available_series[series_num])
		EXMUSIC_CHECK_VALIDITY(exm_series_musicset = exm_series->track_sets[track_set])
		EXMUSIC_CHECK_VALIDITY(exm_series_trackset_music = exm_series_musicset->music[music_track])
	}
	else
	{
		// -- Find our EXMusic track based on the string.
		exm_series = tsourdt3rd_exmusic_available_series[0];

		while (exm_series)
		{
			music_track = 0;
			exm_series_musicset = exm_series->track_sets[track_set];
			exm_series_trackset_music = NULL;

			if (!strnicmp(exm_series->series_name, series, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME))
			{
				// We have a string matching a series? We can use that instead!
				// Hey, maybe we can also use the track number to find the track we want!
				if (M_StringToNumber(track, &music_track))
				{
					if (exm_series_musicset->num_music_lumps && (music_track >= 0 && music_track < exm_series_musicset->num_music_lumps))
					{
						// Found it!
						exm_series_trackset_music = exm_series_musicset->music[music_track];
					}
					else
					{
						exm_series_musicset = NULL;
						exm_series_trackset_music = NULL;
					}
					break;
				}

				// Well, that wasn't a valid number.
				// So, just mark that we found the series we wanted.
				// It's for just-in-case scenarios where the track can't be found, regardless of what we do.
				series_found = true;
			}
			else
			{
				lump_track_num = 0;
			}

			// Search through all our tracks to find the track we want, using the track's name!
			while (music_track < exm_series_musicset->num_music_lumps)
			{
				musicdef_t *def = exm_series_musicset->music[music_track];
				const char *lump_search_names[3] = { track, series, valstr };
				const char *music_name;
				INT32 search_index, track_list;

				for (search_index = 0; search_index < 3; search_index++)
				{
					music_name = lump_search_names[search_index];
					if (music_name == NULL)
					{
						continue;
					}

					for (track_list = 0; track_list < def->numtracks; track_list++)
					{
						if (!stricmp(def->name[track_list], music_name) || (def->title && !stricmp(def->title, music_name)))
						{
							lump_track_num = track_list;
							exm_series_trackset_music = def;
							break;
						}
					}
					if (exm_series_trackset_music != NULL)
					{
						break;
					}
				}
				if (exm_series_trackset_music != NULL)
				{
					break;
				}

				music_track++;
			}

			if (exm_series_trackset_music != NULL) // -- We found everything we needed!
			{
				break;
			}
			else if (series_found) // -- Well, this IS the series we wanted, but nothing could be found...
			{
				music_track = 0;
				if (exm_series_musicset->num_music_lumps)
				{
					exm_series_trackset_music = exm_series_musicset->music[0];
				}
				break;
			}

			exm_series = exm_series->next;
			series_num++;
		}

		EXMUSIC_CHECK_VALIDITY(exm_series)
		EXMUSIC_CHECK_VALIDITY(exm_series_musicset)
		EXMUSIC_CHECK_VALIDITY(exm_series_trackset_music)
	}
#undef EXMUSIC_CHECK_VALIDITY

	if (track_result != NULL)
	{
		track_result->identifier_pos	= track_set;
		track_result->series_pos		= series_num;
		track_result->track_pos			= music_track;
		track_result->cvar				= &cv_tsourdt3rd_audio_exmusic[track_set];
		track_result->identifier		= &tsourdt3rd_exmusic_data_identifier_types[track_set];
		track_result->series			= exm_series;
		track_result->track_set			= exm_series_musicset;
		track_result->lump				= exm_series_trackset_music;
		track_result->lump_track        = lump_track_num;
		track_result->all_series		= tsourdt3rd_exmusic_available_series;
		track_result->all_music_lumps	= exm_series_musicset->music;
	}
	return true;
}

boolean TSoURDt3rd_EXMusic_FindCVar(const char *cvar_name, const char *valstr)
{
	consvar_t *exm_cvar = CV_FindVar(cvar_name);
	INT32 track_set_num = -1;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		return false;
	}

	memset(&exm_last_find_track_result, 0, sizeof(exm_last_find_track_result));
	TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &track_set_num);
	if (TSoURDt3rd_EXMusic_FindTrack(valstr, track_set_num, &exm_last_find_track_result) == false)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "%s - Couldn't find track \x82\"%s\"\x80.\n", exm_cvar->name, valstr);
		return false;
	}

	prev_exm_cvar = exm_cvar;
	prev_exm_pos = exm_last_find_track_result.track_pos;
	if (prev_exm_string == NULL)
		prev_exm_string = malloc(8192);
	snprintf(prev_exm_string, 256, "%s:%d", exm_last_find_track_result.series->series_name, prev_exm_pos);

	return true; // -- We can update now!
}

// ===========================
// COMMANDS
// ===========================

void Command_EXMusic_f(void)
{
	consvar_t *exm_cvar;
	const char *valstr;
	INT32 track_set_num = -1;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		return;
	}
	else if (COM_Argc() < 1)
	{
		STAR_CONS_Printf(STAR_CONS_NONE, "tsourdt3rd_exmusic_set <type/cvar_name> <series> [track] - Sets the EXMusic command to be the given track.\n");
		return;
	}

	exm_cvar = CV_FindVar(COM_Argv(0));
	valstr = COM_Argv(1);

	if (exm_cvar == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_NONE, "Invalid EXMusic CVar given!\n");
		return;
	}
	else if (!stricmp(exm_cvar->string, valstr))
	{
		// -- We're basically setting it to the same exact thing. Don't do that.
		return;
	}

	memset(&exm_last_find_track_result, 0, sizeof(exm_last_find_track_result));
	TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &track_set_num);
	if (TSoURDt3rd_EXMusic_FindTrack(valstr, track_set_num, &exm_last_find_track_result) == false)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "%s - Couldn't find track \x82\"%s\"\x80.\n", exm_cvar->name, valstr);
		return;
	}

	prev_exm_cvar = exm_cvar;
	prev_exm_pos = exm_last_find_track_result.track_pos;
	if (prev_exm_string == NULL)
		prev_exm_string = malloc(8192);
	snprintf(prev_exm_string, 256, "%s:%d", exm_last_find_track_result.series->series_name, prev_exm_pos);
}

//
// boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
// Checks if the EXMusic commands can be updated, and if so, updates them.
//
boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
{
	consvar_t *exm_cvar = CV_FindVar(COM_Argv(0));
	INT32 track_set_num = -1;

	if (tsourdt3rd_exmusic_initialized == false || exm_cvar == NULL)
	{
		return false;
	}

	memset(&exm_last_find_track_result, 0, sizeof(exm_last_find_track_result));
	TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &track_set_num);
	if (TSoURDt3rd_EXMusic_FindTrack(valstr, track_set_num, &exm_last_find_track_result) == false)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "%s - Couldn't find track \x82\"%s\"\x80.\n", exm_cvar->name, valstr);
		return false;
	}

	prev_exm_cvar = exm_cvar;
	prev_exm_pos = exm_last_find_track_result.track_pos;
	if (prev_exm_string == NULL)
		prev_exm_string = malloc(8192);
#if 1
	snprintf(prev_exm_string, 256, "%s:%d", exm_last_find_track_result.series->series_name, prev_exm_pos);
#else
	if (exm_last_find_track_result.lump)
	{
#if 0
		if (*exm_last_find_track_result.lump->title != '\0')
			snprintf(prev_exm_string, 256, "%s : %s", exm_last_find_track_result.series->series_name, exm_last_find_track_result.lump->title);
		else
			snprintf(prev_exm_string, 256, "%s : %s", exm_last_find_track_result.series->series_name, exm_last_find_track_result.lump->name);
#else
		if (*exm_last_find_track_result.lump->title != '\0')
			snprintf(prev_exm_string, 256, "%s", exm_last_find_track_result.lump->title);
		else
			snprintf(prev_exm_string, 256, "%s", exm_last_find_track_result.lump->name);
#endif
	}
	else
		snprintf(prev_exm_string, 256, "%s", exm_last_find_track_result.series->series_name);
#endif

	return true; // -- We can update now!
}

//
// void TSoURDt3rd_S_EXMusic_Update(void)
//
// Updates the EXMusic commands.
// Since the 'CanChange' function is run beforehand, we assume that everything is valid, so we cut to the chase.
//
void TSoURDt3rd_S_EXMusic_Update(void)
{
	prev_exm_cvar->string = prev_exm_string;
	prev_exm_cvar->value = prev_exm_pos;

	//free(prev_exm_string);
	prev_exm_cvar = NULL;
	prev_exm_pos = 0;
	prev_exm_string = NULL;

	if (sound_started && Playing() && !TSoURDt3rd_Jukebox_SongPlaying())
	{
		S_ChangeMusicEx(TSoURDt3rd_EXMusic_DetermineLevelMusic(), mapmusflags, true, mapmusposition, 0, 0);
	}
}

//
// EXMusic – Determining and Playing Stage Music
//

#include "../../i_time.h" // I_GetTime
#include "../../p_setup.h" // levelloading

// ------------------------ //
//   Small string helpers
// ------------------------ //

typedef struct { char track[8]; } mus7_t; // 7 chars + NUL

static inline void copy7(mus7_t *dst, const char *src)
{
	if (dst == NULL) return;
	if (src == NULL) { dst->track[0] = 0; return; }
	//strncpy(dst->track, src, 8-1);
	STRBUFCPY(dst->track, src);
}

static inline boolean same7(const char *track_a, const char *track_b)
{
	if (track_a == NULL || track_b == NULL) return false;
	return (!strnicmp(track_a, track_b, 8-1));
}

// ------------------------ //
//   Global EXMusic state
// ------------------------ //

typedef struct exm_state_s
{
	mapheader_t *map;          // last map we evaluated
	mus7_t last_played;		// last track played by the determiner
	mus7_t vanilla_last;       // last vanilla track
	mus7_t exm_last;           // last EXMusic track chosen
	mus7_t returned_last;      // last returned to engine
	UINT32 set_serial;         // global counter for EXMusic sets
	UINT32 last_exm_serial;    // serial of last EXMusic set
	tic_t exm_grace_until;     // until this tic, ignore mismatches
} exm_state_t;
static exm_state_t gexm = {0};

#define EXMUSIC_GRACE_TICS 5
//#define EXMUSIC_GRACE_TICS 0

#ifdef _TSOURDT3RD_DEBUGGING
	#define DEBUG_PRINT(...) CONS_Printf(__VA_ARGS__);
#else
	#define DEBUG_PRINT(...) ;
#endif

// ------------------------ //
//   Music existence checks
// ------------------------ //

static void EXM_SetEXMusic(const char *name)
{
	copy7(&gexm.exm_last, name);
	gexm.last_exm_serial = ++gexm.set_serial;
	gexm.exm_grace_until = (I_GetTime() + EXMUSIC_GRACE_TICS);
}

static inline void EXM_SetVanilla(const char *name)
{
	copy7(&gexm.vanilla_last, name);
}

static inline const char* EXM_Return(const char *name)
{
	copy7(&gexm.last_played, name);
	copy7(&gexm.returned_last, name);
	DEBUG_PRINT("EXM_Return() - Returning %s\n", gexm.returned_last.track);
	return gexm.returned_last.track;
}

// ------------------------ //
//   Safe macros
// ------------------------ //

#define SET_EXMUSIC_MUSIC_BYDEF(defptr) \
	DEBUG_PRINT("exm def '%s' (%s), is valid\n", defptr->name[track], #defptr); \
	EXM_SetEXMusic(defptr->name[track]); \
	return true;

#define SET_EXMUSIC_MUSIC_BYNAME(name) \
	DEBUG_PRINT("exm name '%s' (%s) is valid\n", name, #name); \
	EXM_SetEXMusic(name); \
	return true;


#define MUSIC_EXISTS(name, BODY) \
	TSoURDt3rd_S_MusicExists(name, track, { BODY })

#define SET_VANILLA_MUSIC(name) \
	DEBUG_PRINT("music %s is valid (vanilla, %s)\n", name, #name); \
	EXM_SetVanilla(name);

#define VANILLA_MUSIC_EXISTS(name) \
	SET_VANILLA_MUSIC(name); \
	break;

#define VANILLA_MUSIC_EXISTS_BY_NAME(name) \
	MUSIC_EXISTS(name, VANILLA_MUSIC_EXISTS(name))


// ------------------------ //
//   Override detection
// ------------------------ //

static boolean EXM_UserOverrideActive(void)
{
	const char *cur = S_MusicName();
	tic_t now = I_GetTime();

	if (now < gexm.exm_grace_until)
		return false;

#if 1
	// DEFAULT
	if (gexm.exm_last.track[0] && !same7(cur, gexm.exm_last.track))
		return true;
#endif

#if 0
	if (gexm.exm_last.track[0])
		return !same7(cur, gexm.exm_last.track);
#endif

#if 1
	if (gexm.last_played.track[0] && (!same7(cur, gexm.vanilla_last.track) || !same7(cur, gexm.last_played.track)))
	{
		EXM_SetVanilla(cur);
		return true;
	}
#endif

	return false;
}

// ------------------------ //
//   EXMusic logic
// ------------------------ //

static boolean EXMusic_DetermineMusic(void)
{
	tsourdt3rd_exmusic_findTrackResult_t exm_find_track_result;
	tsourdt3rd_world_scenarios_t scenario = tsourdt3rd_local.world.scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types = tsourdt3rd_local.world.scenario_types;
	INT32 track = 0;

#define EXMUSIC_GET_TYPE(scenario_type, exm_track_set) \
	memset(&exm_find_track_result, 0, sizeof(exm_find_track_result)); \
	if ((scenario_type) == -1 || (scenario_types & (scenario_type))) { \
		if (TSoURDt3rd_EXMusic_FindTrack(cv_tsourdt3rd_audio_exmusic[(exm_track_set)].string, (exm_track_set), &exm_find_track_result)) { \
			track = exm_find_track_result.lump_track; \
			if (exm_find_track_result.series_pos >= tsourdt3rd_exmusic_starting_series_max) { \
				MUSIC_EXISTS(exm_find_track_result.lump->name[track], { \
					SET_EXMUSIC_MUSIC_BYDEF(exm_find_track_result.lump); \
				}); \
			} \
		} \
	}

	if (scenario & TSOURDT3RD_WORLD_SCENARIO_GAMEOVER)
	{
		EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_gameover);
		return false;
	}

	if (scenario & TSOURDT3RD_WORLD_SCENARIO_INTERMISSION)
	{
		if (scenario & TSOURDT3RD_WORLD_SCENARIO_BOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS, tsourdt3rd_exmusic_intermission_truefinalboss);
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS, tsourdt3rd_exmusic_intermission_finalboss);
			if ((scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS) && gamestate == GS_EVALUATION)
			{
				return false;
			}
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_intermission_boss);
		}
		EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_intermission);
		return false;
	}

	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_POSTBOSS)
	{
		if (gexm.map && cv_tsourdt3rd_audio_bosses_postboss.value)
			MUSIC_EXISTS(gexm.map->muspostbossname, { SET_EXMUSIC_MUSIC_BYNAME(gexm.map->muspostbossname); });
	}
	if (scenario & TSOURDT3RD_WORLD_SCENARIO_BOSS)
	{
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_truefinalboss_pinch);
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses_truefinalboss);
		}
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_finalboss_pinch);
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses_finalboss);
		}
		EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_RACE, tsourdt3rd_exmusic_bosses_race);
		EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_pinch);
		EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses);
	}

#undef EXMUSIC_GET_TYPE

	return false;
}

// ------------------------ //
//   Public API
// ------------------------ //

//
// lumpnum_t TSoURDt3rd_EXMusic_DefaultMapTrack_Play(const char **mname)
// Plays default music if the game can't find a specified track to play.
//
lumpnum_t TSoURDt3rd_EXMusic_DefaultMapTrack_Play(const char **mname)
{
	const boolean midipref = cv_musicpref.value;
	const char *track_string = cv_tsourdt3rd_audio_exmusic[tsourdt3rd_exmusic_defaultmaptrack].string;
	tsourdt3rd_exmusic_findTrackResult_t exm_find_track_result;

	if (tsourdt3rd_exmusic_initialized == false)
	{
		return LUMPERROR;
	}

	if (TSoURDt3rd_EXMusic_FindTrack(track_string, tsourdt3rd_exmusic_defaultmaptrack, &exm_find_track_result) == false)
	{
		COM_BufInsertText(va("%s \"Default:0\"\n", cv_tsourdt3rd_audio_exmusic[tsourdt3rd_exmusic_defaultmaptrack].name));
		//STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "EXMUSIC (Default Map Track) - Couldn't find music to use!\n", (*mname));
		return LUMPERROR;
	}
	else if (exm_find_track_result.series_pos >= tsourdt3rd_exmusic_starting_series_max)
	{
		const char *new_name = exm_find_track_result.lump->name[exm_find_track_result.lump_track];
		lumpnum_t lumpnum = LUMPERROR;

		if (S_PrefAvailable(midipref, new_name))
			lumpnum = W_GetNumForName(va(midipref ? "d_%s":"o_%s", new_name));
		else if (S_PrefAvailable(!midipref, new_name))
			lumpnum = W_GetNumForName(va(midipref ? "o_%s":"d_%s", new_name));

		if (lumpnum != LUMPERROR)
		{
			//STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE, "EXMUSIC (Default Map Track) - Using '%.6s' instead!\n", new_name);
			return lumpnum;
		}
	}

	(void)mname;
	return LUMPERROR;
}

//
// const char *TSoURDt3rd_EXMusic_DetermineLevelMusic(void)
// Determines what music should play in the level based on the current EXMusic scenario.
//
const char *TSoURDt3rd_EXMusic_DetermineLevelMusic(void)
{
	INT32 track = 0;
	const char *current_music = S_MusicName();

	gexm.returned_last.track[0] = 0;
	TSoURDt3rd_WORLD_UpdateScenarios();

	// Update map / reset if changed
	if (!gexm.map || gexm.map != mapheaderinfo[gamemap-1] || levelloading)
	{
		gexm.map = mapheaderinfo[gamemap-1];
		gexm.vanilla_last.track[0] = 0;
		gexm.exm_last.track[0] = 0;
		gexm.returned_last.track[0] = 0;
		gexm.last_played.track[0] = 0;
		gexm.exm_grace_until = 0;
		if (gexm.map && *gexm.map->musname != '\0')
			copy7(&gexm.vanilla_last, gexm.map->musname);
		if (*gexm.vanilla_last.track == '\0')
			copy7(&gexm.vanilla_last, mapmusname);
	}
	else
	{
		if (!TSoURDt3rd_Jukebox_SongPlaying() && *current_music != '\0')
		{
			if (!same7(current_music, gexm.vanilla_last.track) && !same7(gexm.exm_last.track, current_music))
			{
				DEBUG_PRINT("updating map music to current music (%s)\n", current_music);
				copy7(&gexm.vanilla_last, current_music);
			}
#if 0 // DEFAULT: OFF
			else if (!same7(gexm.vanilla_last.track, mapmusname))
			{
				DEBUG_PRINT("updating map music to mapmusname (%s)\n", mapmusname);
				copy7(&gexm.vanilla_last, mapmusname);
			}
#endif
		}
	}

	// Hard overrides
	if (TSoURDt3rd_AprilFools_ModeEnabled())
	{
		// Not to be confused with 'BwehHehHe'
		return EXM_Return("_hehe");
	}
	else if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// LEAVE MY JUKEBOX ALONE!
		return "";
	}
	else if (gamestate == GS_TITLESCREEN || titlemapinaction || !gexm.map)
	{
		// Made it here? Play the map's default track, and we're done :)
		DEBUG_PRINT("DetermineLevelMusic() - Title screen or no map, using map default music\n");
		if (gexm.map) MUSIC_EXISTS(gexm.map->musname, { return EXM_Return(gexm.map->musname); });
		MUSIC_EXISTS(mapmusname, { return EXM_Return(mapmusname); });
		return EXM_Return("_title");
	}

#if 1
	// extremely hacky hack to get intermission music to always work with EXMusic
	/// \todo: check for scenario, not scenario_type, before this, if scenario doesn't match last one, then last exmusic gets reset
	tsourdt3rd_exmusic_findTrackResult_t exm_find_track_result;
	tsourdt3rd_world_scenarios_t scenario = tsourdt3rd_local.world.scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types = tsourdt3rd_local.world.scenario_types;
#define EXMUSIC_GET_TYPE_HERE(scenario_type, exm_track_set) \
	memset(&exm_find_track_result, 0, sizeof(exm_find_track_result)); \
	if ((scenario_type) == -1 || (scenario_types & (scenario_type))) { \
		if (TSoURDt3rd_EXMusic_FindTrack(cv_tsourdt3rd_audio_exmusic[(exm_track_set)].string, (exm_track_set), &exm_find_track_result)) { \
			track = exm_find_track_result.lump_track; \
			if (exm_find_track_result.series_pos >= tsourdt3rd_exmusic_starting_series_max) { \
				MUSIC_EXISTS(exm_find_track_result.lump->name[track], { \
					DEBUG_PRINT("exm def '%s' is valid\n", exm_find_track_result.lump->name[track]); \
					EXM_SetEXMusic(exm_find_track_result.lump->name[track]); \
					return exm_find_track_result.lump->name[track]; \
				}); \
			} \
		} \
	}
	if (scenario & TSOURDT3RD_WORLD_SCENARIO_GAMEOVER)
	{
		EXMUSIC_GET_TYPE_HERE(-1, tsourdt3rd_exmusic_gameover);
	}
	if (scenario & TSOURDT3RD_WORLD_SCENARIO_INTERMISSION)
	{
		if (scenario & TSOURDT3RD_WORLD_SCENARIO_BOSS)
		{
			EXMUSIC_GET_TYPE_HERE(TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS, tsourdt3rd_exmusic_intermission_truefinalboss);
			EXMUSIC_GET_TYPE_HERE(TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS, tsourdt3rd_exmusic_intermission_finalboss);
			if ((scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS) && gamestate == GS_EVALUATION)
			{
				gexm.exm_last.track[0] = 0;
				gexm.last_played.track[0] = 0;
				return "";
			}
			EXMUSIC_GET_TYPE_HERE(-1, tsourdt3rd_exmusic_intermission_boss);
		}
		EXMUSIC_GET_TYPE_HERE(-1, tsourdt3rd_exmusic_intermission);
		DEBUG_PRINT("Intermission - No EXMusic found, clearing last EXMusic\n");
		gexm.exm_last.track[0] = 0;
		gexm.last_played.track[0] = 0;
	}
#undef EXMUSIC_GET_TYPE_HERE
#endif

	// Respect user/Lua override
	if (EXM_UserOverrideActive())
	{
		DEBUG_PRINT("DetermineLevelMusic() - Using USER-DEFINED EXMusic!\n");

		if (!same7(S_MusicName(), mapmusname))
		{
			EXM_SetVanilla(S_MusicName());
			return S_MusicName();
		}

		EXM_SetVanilla(mapmusname);
		return mapmusname;
	}

	// Let EXMusic try!
	if (EXMusic_DetermineMusic() == false)
	{
		gexm.exm_last.track[0] = 0;
		gexm.last_played.track[0] = 0;
	}

	// If EXMusic set something new, prefer it and return it!
	if (gexm.exm_last.track[0]) // DEFAULT
	//if (gexm.returned_last.track[0])
	{
		if (!same7(S_MusicName(), gexm.exm_last.track))
		{
			DEBUG_PRINT("EXMusic - Using the current EXMusic track\n");
			return EXM_Return(gexm.exm_last.track);
		}
		DEBUG_PRINT("EXMusic - Using the currently playing track instead\n");
		return S_MusicName();
	}

	// ...Otherwise, fallback to vanilla's tracks!
	switch (gamestate)
	{
		case GS_EVALUATION:
		case GS_GAMEEND:
			//VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
			return "";
			/* FALLTHRU */
		case GS_INTERMISSION:
			switch (intertype)
			{
				case int_coop:
					VANILLA_MUSIC_EXISTS_BY_NAME(gexm.map->musintername);
					VANILLA_MUSIC_EXISTS_BY_NAME("_clear");
					break;
				case int_spec:
					VANILLA_MUSIC_EXISTS_BY_NAME(gexm.map->musintername);
					VANILLA_MUSIC_EXISTS_BY_NAME(stagefailed ? "CHFAIL" : "CHPASS");
					break;
				default:
					VANILLA_MUSIC_EXISTS_BY_NAME("_inter");
					break;
			}

			// hacky hack, fix later
			if (gexm.vanilla_last.track[0]) break;

			/* FALLTHRU */
		default:
#if 0
			if (!same7(S_MusicName(), mapmusname) && !same7(gexm.exm_last.track, S_MusicName()))
				VANILLA_MUSIC_EXISTS_BY_NAME(S_MusicName());
#endif

#if 1
			// DEFAULT
			if (!same7(S_MusicName(), gexm.map->musname))
				VANILLA_MUSIC_EXISTS_BY_NAME(gexm.map->musname);
			if (!same7(S_MusicName(), mapmusname))
				VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
#endif
#if 0
			if (gexm.vanilla_last.track[0] && !same7(S_MusicName(), gexm.vanilla_last.track))
				VANILLA_MUSIC_EXISTS_BY_NAME(gexm.vanilla_last.track);
			if (!same7(S_MusicName(), mapmusname))
				VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
			if (!same7(S_MusicName(), gexm.map->musname))
				VANILLA_MUSIC_EXISTS_BY_NAME(gexm.map->musname);
#endif
#if 0
			if (!same7(S_MusicName(), mapmusname) && !same7(gexm.exm_last.track, S_MusicName()))
				VANILLA_MUSIC_EXISTS_BY_NAME(S_MusicName());
			if (!same7(gexm.map->musname, mapmusname) && !same7(gexm.exm_last.track, mapmusname))
				VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
			if (!same7(S_MusicName(), gexm.map->musname))
				VANILLA_MUSIC_EXISTS_BY_NAME(gexm.map->musname);
#endif
			if (gexm.vanilla_last.track[0] && !same7(S_MusicName(), gexm.vanilla_last.track))
				VANILLA_MUSIC_EXISTS_BY_NAME(gexm.vanilla_last.track);
			break;
	}

	if (gexm.vanilla_last.track[0])
	{
		return EXM_Return(gexm.vanilla_last.track);
	}

	// If somehow, SOMEHOW, we made it here, let's *not* return anything NULL, please.
	// That can cause crashes. Crashes are bad, you know.
	return "";
}
