// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_exmusic.c
/// \brief TSoURDt3rd's cool and groovy music setup, definition, and replacement routines

#include "smkg-s_exmusic.h"

#include "smkg-s_audio.h"
#include "smkg-s_jukebox.h"
#include "../ss_main.h"

#include "../../y_inter.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_exmusic_t **tsourdt3rd_exmusic_container = NULL;

// Sets the default names of our global EXMusic tables.
// The amount here should be corresponded to the value of 'TSOURDT3RD_EXMUSIC_MAX_STARTING_TRACKS'.
const char *tsourdt3rd_exmusic_default_names[] = {
	"Default",
	"Random",
	NULL
};

// Needed for console command stuffs.
static INT32 last_exmusic_pos = 0;
static consvar_t *last_excvar = NULL;
static tsourdt3rd_exmusic_t *last_extype = NULL;

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_EXMusic_Init(tsourdt3rd_exmusic_t *extype, tsourdt3rd_exmusic_t extype_defaults)
// Initializes the given EXMusic type and stores it in our EXMusic container array.
//
boolean TSoURDt3rd_EXMusic_Init(tsourdt3rd_exmusic_t *extype, tsourdt3rd_exmusic_t extype_defaults)
{
	static boolean container_initialized = false;
	INT32 container_pos = 0;
	tsourdt3rd_exmusic_t *container_extype = NULL, *prev_container_extype = NULL;
	tsourdt3rd_exmusic_data_t *data = NULL;
	tsourdt3rd_exmusic_data_series_t **series = NULL, *prev_series = NULL;

	if (container_initialized == false)
	{
		tsourdt3rd_exmusic_container = Z_Malloc((TSOURDT3RD_EXMUSIC_TYPES * sizeof(tsourdt3rd_exmusic_t *)), PU_STATIC, NULL);
		container_initialized = true;
	}
	if (tsourdt3rd_exmusic_container == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_Init() - EXMusic container wasn't initialized, not adding new type!\n");
		return false;
	}

	extype = &extype_defaults;
	//extype = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, &extype_defaults);
	//extype = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
	if (extype == NULL) goto failed;

	data = extype->data;
	if (data == NULL)
	{
		data = Z_Calloc(sizeof(tsourdt3rd_exmusic_data_t), PU_STATIC, NULL);
		//data = Z_Calloc(sizeof(tsourdt3rd_exmusic_data_t), PU_STATIC, &extype->data);
		if (data == NULL) goto failed;
	}

	series = data->series;
	series = Z_Malloc(sizeof(tsourdt3rd_exmusic_data_series_t *) * 100, PU_STATIC, NULL);
	//series = Z_Malloc(TSOURDT3RD_EXMUSIC_MAX_BOSSES_LUMPS * sizeof(boss_data_t *), PU_STATIC, NULL);
	//series = Z_Malloc(sizeof(tsourdt3rd_exmusic_data_series_t *) * 100, PU_STATIC, &data->series);
	if (series == NULL) goto failed;

	CONS_Printf("heheow - data series is %d\n", data->num_series);
	while (data->num_series < TSOURDT3RD_EXMUSIC_MAX_STARTING_TRACKS)
	{
		//tsourdt3rd_exmusic_data_series_t *new_series = Z_Calloc(sizeof(tsourdt3rd_exmusic_data_series_t), PU_STATIC, &series[data->num_series]);
		tsourdt3rd_exmusic_data_series_t *new_series = Z_Calloc(sizeof(tsourdt3rd_exmusic_data_series_t), PU_STATIC, NULL);
		//CONS_Printf("heheow - data series name %d is %s\n", data->num_series, series[data->num_series]->name);
		//new_series->tracks = Z_Calloc(data->init_size, PU_STATIC, &series[data->num_series]->tracks);
		//new_series->tracks = Z_Calloc(data->init_size, PU_STATIC, NULL);
		new_series->tracks = Z_Calloc(extype->init_size, PU_STATIC, NULL);
		//new_series->tracks = data->default_series_data;
		memcpy(new_series->tracks, data->default_series_data, sizeof(&data->default_series_data));

		//strlcpy(new_series->name, tsourdt3rd_exmusic_default_names[data->num_series], TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);
		snprintf(new_series->name, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME, "%s", tsourdt3rd_exmusic_default_names[data->num_series]);
		//CONS_Printf("heheow - new data series name %d is %s\n", data->num_series, series[data->num_series]->name);

		if (prev_series != NULL)
		{
			new_series->prev = prev_series;
			prev_series->next = new_series;
		}
		prev_series = new_series;
#if 1
		series[data->num_series] = new_series;
		CONS_Printf("heheow - new data series name %lld is %s\n", data->num_series, series[data->num_series]->name);
#endif
		data->num_series++;
	}
	if (extype->init_routine && !extype->init_routine(extype)) goto failed;
	extype->active = true;
#if 1
	extype->data = data;
	extype->data->series = series;
#endif
	CONS_Printf("heheow - exited iterator\n");

	container_extype = tsourdt3rd_exmusic_container[0];
	while (container_extype && container_extype->active)
	{
		prev_container_extype = container_extype;
		container_extype = container_extype->next;
		container_pos++;
	}
	if (container_extype != NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_Init() - Max EXMusic types reached!\n");
		goto failed;
	}
#define LINK_TO_CONTAINER
#ifdef LINK_TO_CONTAINER
	container_extype = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, &tsourdt3rd_exmusic_container[container_pos]);
#else
	container_extype = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
#endif
	container_extype = extype;

	if (prev_container_extype != NULL)
	{
		container_extype->prev = prev_container_extype;
		prev_container_extype->next = container_extype;
	}
#ifndef LINK_TO_CONTAINER
#if 0
	(*tsourdt3rd_exmusic_container) = container_extype;
	(void)container_pos;
#else
	tsourdt3rd_exmusic_container[container_pos] = container_extype;
#endif
#endif

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE, "TSoURDt3rd_EXMusic_Init(): Type '\x82%s\x80' created!\n", container_extype->identifier);
	return true;

	failed:
	{
		if (extype)
		{
			if (series) { Z_Free(series); series = NULL; }
			if (data == NULL) { Z_Free(data); data = NULL; }
			Z_Free(extype); extype = NULL;
		}
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_EXMusic_Init(): Could not allocate memory for new type '\x82%s\x80'.\n", extype_defaults.identifier);
		return false;
	}
}

tsourdt3rd_exmusic_t *TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(const char *identifier)
{
	tsourdt3rd_exmusic_t *cur_extype = NULL;

	// Make sure the user isn't messing around with us...
	if (identifier == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier() - No valid identifier was given!\n");
		return NULL;
	}
	else if (tsourdt3rd_exmusic_container == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier() - EXMusic container wasn't initialized at startup, so this function can't be run!\n");
		return NULL;
	}

	// Do what we came here to do.
	cur_extype = tsourdt3rd_exmusic_container[0];
	while (cur_extype)
	{
		if (!cur_extype->active || strcmp(cur_extype->identifier, identifier))
		{
			cur_extype = cur_extype->next;
			continue;
		}
		break;
	}
	return ((cur_extype && cur_extype->return_routine != NULL) ? cur_extype->return_routine(NULL) : cur_extype);
}

tsourdt3rd_exmusic_t *TSoURDt3rd_EXMusic_ReturnTypeFromCVar(consvar_t *cvar)
{
	tsourdt3rd_exmusic_t *extype = NULL;

	// Make sure the user isn't messing around with us...
	if (cvar == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromCVar() - No CVar with that name was found!\n");
		return NULL;
	}
	else if (tsourdt3rd_exmusic_container == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_EXMusic_ReturnTypeFromCVar() - EXMusic container wasn't initialized at startup, so this function can't be run!\n");
		return NULL;
	}

	// Do what we came here to do.
	extype = tsourdt3rd_exmusic_container[0];
	STAR_CONS_Printf(STAR_CONS_DEBUG, "ffing eye of rah or something\n");
	while (extype != NULL)
	{
		tsourdt3rd_exmusic_data_t *exdata = extype->data;
		const char *exmusic_identifer_to_use = NULL;

		if (exdata != NULL && exdata->identifiers != NULL)
		{
			tsourdt3rd_exmusic_data_identifiers_t *current_exdata_identifier = &exdata->identifiers[0];
			consvar_t *current_cvar = (current_exdata_identifier ? current_exdata_identifier->cvar : NULL);
			INT32 data_identifier_pos = 0;
#if 0
			while (current_exdata_identifier && current_cvar)
			{
				if (current_cvar == cvar)
				{
					exmusic_identifer_to_use = extype->identifier;
					break;
				}
				current_exdata_identifier = &exdata->identifiers[++data_identifier_pos];
				current_cvar = (current_exdata_identifier ? current_exdata_identifier->cvar : NULL);
			}
#else
			break;
#endif
		}

		if (exmusic_identifer_to_use != NULL)
		{
			// We found the identifer!
			break;
		}

		extype = extype->next;
	}
	STAR_CONS_Printf(STAR_CONS_DEBUG, "ffing eye of rah or something\n");
	return extype;
}

boolean TSoURDt3rd_EXMusic_DoesDefHaveValidLump(tsourdt3rd_exmusic_t *exdef, consvar_t *cvar, boolean soundtest_valid)
{
	musicdef_t *def = NULL;

	if (exdef == NULL || cvar == NULL)
	{
		// ...Why?
		return false;
	}
	else if (exdef->lump_validity_routine == NULL)
	{
		// We... kinda need a routine for this...
		return false;
	}

	def = exdef->lump_validity_routine(cvar);
	if (def == NULL) return false;

	TSoURDt3rd_S_MusicExists(def, { return true; })
	return (soundtest_valid && (def == &soundtestsfx));
}

//
// boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
// Checks if the EXMusic commands can be updated, and if so, updates them.
//
boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
{
	tsourdt3rd_exmusic_t *extype = NULL;
	consvar_t *excvar = CV_FindVar(COM_Argv(0));
	INT32 valstr_num = atoi(valstr);
	INT32 exmusic_pos = 0;

	if (excvar == NULL)
	{
		// ...How did we even get here then?
		return false;
	}

	// Check each series for our cvar!
	TSoURDt3rd_EXMusic_ReturnType(excvar, extype);
	if (extype == NULL) return false;
#if 1
	if (extype->data == NULL) return false;
	if (extype->data->series == NULL) return false;
#endif

#if 0
	extype = exmusic_data[0];

	if (valstr_num > 0 || !strcmp(valstr, "0"))
	{
		// Set our EXMusic command based on the number.
		exmusic_pos = valstr_num;
		extype = exmusic_data[exmusic_pos];
	}
	else
	{
		// Set our EXMusic command based on the string.
		extype = exmusic_data[0];
		while (extype)
		{
			if (!stricmp(extype->series, valstr)) break;
			extype = extype->next;
			exmusic_pos++;
		}
	}
#else
	(void)valstr_num;
#endif

	// Now just input our values, and we're done!
	if (excvar->value == exmusic_pos)
	{
		// -- We're basically setting it to the same exact thing. Don't do that.
		return false;
	}
	else if (exmusic_pos > (INT32)extype->data->num_series)
	{
		// -- It doesn't even exist!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR,
			"EXMusic - \x82\"%s\"\x80 couldn't find Series \x82\"%s\"\x80, not changing.\n",
			excvar->name, valstr
		);
		return false;
	}
	CONS_Printf("Done 2!\n");
#if 0
	else if (!TSoURDt3rd_EXMusic_DoesDefHaveValidLump(extype, exmusic_option))
	{
		// -- We can't update now...
		STAR_CONS_Printf(
			STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR,
			"EXMusic - \x82\"%s\"\x80 doesn't have an existing lump, not changing.\n",
			exmusic_cvar_identifier_name[exmusic_option],
			excvar->name
		);
		return false;
	}
#endif

	// We can update now!
	last_extype = extype;
	last_exmusic_pos = exmusic_pos;
	last_excvar = excvar;
	return true;
}

//
// void TSoURDt3rd_S_EXMusic_Update(void)
//
// Updates the EXMusic commands.
// Since the 'CanChange' function is run beforehand, we assume that everything is valid, so we cut to the chase.
//
void TSoURDt3rd_S_EXMusic_Update(void)
{
#if 0
	last_excvar->string = last_extype->series;
#endif
	last_excvar->value = last_exmusic_pos;
	last_extype = NULL;
	last_excvar = NULL;
	last_exmusic_pos = 0;
}

#if 1
//
// const char *TSoURDt3rd_DetermineLevelMusic(void)
// Determines and Returns What Music Should be Played on the Current Stage
//
#include "../smkg-cvars.h"
#include "../star_vars.h"
#include "../../doomstat.h"

#define RANDOM_STATICNESS
//#define TEST_THING
//#define MATH_TEST_THING

#ifdef RANDOM_STATICNESS
static const char *prev_selected_music = NULL;
static musicdef_t *prev_def = NULL;
static tsourdt3rd_exmusic_t *prev_exdef = NULL;
#endif

#define EXMUSIC_MUSIC_EXISTS(music) \
	CONS_Printf("music %s is valid\n", music); \
	strlcpy(selected_music, music, 7); \
	break;

#define EXMUSIC_EXISTS(slot) \
	TSoURDt3rd_S_MusicExists(slot, { \
		CONS_Printf("def %s is valid\n", slot->name); \
		strlcpy(selected_music, slot->name, 7); \
		prev_def = slot; \
		prev_exdef = exdef; \
		return; \
	})

#define EXMUSIC_EXISTS_BY_NAME(music) \
	TSoURDt3rd_S_MusicExists(music, EXMUSIC_MUSIC_EXISTS(music))

#define MUSIC_MATCHES \
	strnicmp(S_MusicName(), ((mapmusflags & MUSIC_RELOADRESET) ? map->musname : mapmusname), 7)

#define MUSICEXISTS(music) (music && S_MusicExists(music, !midi_disabled, !digital_disabled))

static void EXMusic_DetermineMusic(const char *type, char *selected_music)
{
	tsourdt3rd_exmusic_t *exdef = NULL;
	musicdef_t *def = NULL;

#if 1
	STAR_CONS_Printf(STAR_CONS_DEBUG, "currently selected_music is %s\n", selected_music);
#endif

	// This function assumes you gave your EXMusic type a play_routine.
	// If you didn't, you'll probably get a crash.

	if (type == NULL)
	{
		// Search through all EXMusic types, maybe we'll get something...
		exdef = tsourdt3rd_exmusic_container[0];
		while (exdef != NULL)
		{
			def = exdef->play_routine(NULL);
			if (def != NULL)
				break;
			exdef = exdef->next;
		}
	}
	else
	{
		// Search for a specific EXMusic type...
		TSoURDt3rd_EXMusic_ReturnType(type, exdef);
		if (exdef == NULL)
		{
			// Invalid EXMusic type, move on.
			return;
		}
		def = exdef->play_routine(NULL);
	}

	if (def != NULL)
	{
		// Set our new music track!
		EXMUSIC_EXISTS(def)
	}

#if 0
#if !defined (TEST_THING) && !defined (MATH_TEST_THING)
	exmusic_selected_series = exmusic_def_p[cvar->value];
	if (exmusic_selected_series == NULL)
	{
		// Invalid EXMusic def, move on.
		continue;
	}
#endif
#endif
}

const char *TSoURDt3rd_DetermineLevelMusic(void)
{
#if 1
	static char selected_music[7] = "";
	//static char selected_music[7];
#else
	char selected_music[7];
#endif
	mapheader_t *map = mapheaderinfo[gamemap-1];

	memset(selected_music, 0, sizeof(selected_music));

	if (TSoURDt3rd_AprilFools_ModeEnabled())
	{
		// April Fools Mode overrides literally everything.
		return "_hehe";
	}
	else if (TSoURDt3rd_Jukebox_IsPlaying())
	{
		// No, don't override my music.
		return tsourdt3rd_global_jukebox->curtrack->name;
	}
#if 0
	else if (gamestate == GS_TITLESCREEN || titlemapinaction)
	{
		//TSoURDt3rd_S_MusicExists(map->musname, { return map->musname; })
		TSoURDt3rd_S_MusicExists(mapmusname, { return mapmusname; })
		return "_title";
	}
#endif
	else if (map == NULL)
	{
		// ...How?
		return mapmusname;
	}
#if 1
	else if (gamestate == GS_TITLESCREEN || titlemapinaction)
	{
		TSoURDt3rd_S_MusicExists(map->musname, { return map->musname; })
		TSoURDt3rd_S_MusicExists(mapmusname, { return mapmusname; })
		return "_title";
	}
#endif

#if 0
	static char old_mapmusname[6];
	if (memcmp(old_mapmusname, mapmusname))
		snprintf(old_mapmusname, 6, "%s", mapmusname);
#endif

#if 0
	// Now, determine our music!
	EXMusic_DetermineMusic(NULL, selected_music);
#endif

#if 0

	// Made it here? Play the map's default track, and we're done :) //
	if (gamestate == GS_TITLESCREEN || titlemapinaction)
	{
		if (MUSICEXISTS(mapheaderinfo[gamemap-1]->musname))
			return mapheaderinfo[gamemap-1]->musname;
		if (MUSICEXISTS("_title"))
			return "_title";
		return mapmusname;
	}

	if (RESETMUSIC || strnicmp(S_MusicName(),
		((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname), 7))
		return ((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname);
	else
		return mapheaderinfo[gamemap-1]->musname;

#if 0
	if (strnicmp(TSoURDt3rd_DetermineLevelMusic(), S_MusicName(), 7))
		return mapmusname;
#endif
	return ((!mapmusname[0] || !strnicmp(mapmusname, S_MusicName(), 7)) ? mapheaderinfo[gamemap-1]->musname : mapmusname);

#else

#if 0
	if (selected_def == NULL)
	{
		if (mapmusflags & MUSIC_RELOADRESET)
			return map->musname;
		return mapmusname;
	}
#else
	goto finished;

finished:
{
	if (*selected_music != '\0')
	{
		// We found a different piece of music, so let's leave!
#ifdef RANDOM_STATICNESS
		prev_selected_music = selected_music;
#endif
		return selected_music;
	}

#if 0
	return NULL;
#endif

	switch (gamestate)
	{
		case GS_EVALUATION:
		case GS_GAMEEND:
			// We don't *HAVE* to play anything here, it's just cool if we *CAN*.
			EXMUSIC_EXISTS_BY_NAME(mapmusname);
			/* FALLTHRU */

		case GS_INTERMISSION:
			//EXMUSIC_EXISTS_BY_NAME(mapmusname);
			EXMUSIC_EXISTS_BY_NAME(map->musintername);
			switch (intertype)
			{
				case int_coop:
					EXMUSIC_EXISTS_BY_NAME("_clear");
					break;
				case int_spec:
					EXMUSIC_EXISTS_BY_NAME(stagefailed ? "CHFAIL" : "CHPASS");
					break;
				default:
					break;
			}
			/* FALLTHRU */

		default:
			if (*selected_music != '\0') break;
			if (RESETMUSIC || MUSIC_MATCHES)
			{
				if (mapmusflags & MUSIC_RELOADRESET)
					EXMUSIC_EXISTS_BY_NAME(map->musname)
				else
					EXMUSIC_EXISTS_BY_NAME(mapmusname)
			}
			else
			{
				EXMUSIC_EXISTS_BY_NAME(map->musname);
				EXMUSIC_EXISTS_BY_NAME(mapmusname);
			}
			break;
	}
#ifdef RANDOM_STATICNESS
	prev_selected_music = selected_music;
#endif
	return selected_music;
}
#endif
#endif
}
#endif
