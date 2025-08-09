// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  core/exmusic_types/smkg-bosses.c
/// \brief An EXMusic type that specializes in playing Boss music.

#include "../smkg-s_exmusic.h"

// ------------------------ //
//        Variables
// ------------------------ //

// This is the track data for our new EXMusic type's tracks.
typedef struct boss_data_s {
	musicdef_t *regular_boss[2];		// -- 0 - Boss; 1 - Pinch Mode
	musicdef_t *final_boss[2];			// -- 0 - Final Boss; 1 - Pinch Mode
	musicdef_t *true_final_boss[2];		// -- 0 - True Final Boss; 1 - Pinch Mode
	musicdef_t *race;					// -- Do you like races?
} boss_data_t;

// These are the necessary routines for our new EXMusic type.
// Their code is defined later on in this script.
static musicdef_t *__play(consvar_t *cvar);

// When our new EXMusic type is created, this is the basic info that gets inserted.
static boss_data_t default_track_def = {
	{ &soundtestsfx, &soundtestsfx },
	{ &soundtestsfx, &soundtestsfx },
	{ &soundtestsfx, &soundtestsfx },
	&soundtestsfx,
};
static tsourdt3rd_exmusic_data_identifiers_t data_identifiers[] = {
	{ &cv_tsourdt3rd_audio_exmusic_bosses, NULL },
	{ &cv_tsourdt3rd_audio_exmusic_bosspinch, "Pinch" },
	{ &cv_tsourdt3rd_audio_exmusic_finalbosses, "Final Boss" },
	{ &cv_tsourdt3rd_audio_exmusic_finalbosspinch, "Final Boss Pinch" },
	{ &cv_tsourdt3rd_audio_exmusic_truefinalbosses, "True Final Boss" },
	{ &cv_tsourdt3rd_audio_exmusic_truefinalbosspinch, "True Final Boss Pinch" },
	{ &cv_tsourdt3rd_audio_exmusic_racebosses, "Race" }
};
static tsourdt3rd_exmusic_data_t tsourdt3rd_default_bosses_data = {
	sizeof(boss_data_t *), data_identifiers, NULL, &default_track_def, 0
};
tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_bosses = {
	false, sizeof(boss_data_t), "bosses", &tsourdt3rd_default_bosses_data,
	NULL, NULL, __play, NULL,
	NULL,
	NULL
};

// Our new EXMusic type. Isn't it precious?
tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_bosses = NULL;

// The max amount of lumps that this EXMusic type's data can currently hold.
#define TSOURDT3RD_EXMUSIC_MAX_BOSSES_LUMPS 100

// ------------------------ //
//        Functions
// ------------------------ //

static musicdef_t *__play(consvar_t *cvar)
{
	tsourdt3rd_world_scenarios_t scenario = tsourdt3rd_local.world.scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types = tsourdt3rd_local.world.scenario_types;
	boss_data_t *tracks = NULL;
	musicdef_t *def = NULL;

	if (tracks == NULL)
	{
		// Aw dang it...
		return NULL;
	}

	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS)
	{
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH) EXMUSIC_EXISTS_BY_DEF(tracks->true_final_boss[1])
		EXMUSIC_EXISTS_BY_DEF(tracks->true_final_boss[0])
	}
	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS)
	{
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH) EXMUSIC_EXISTS_BY_DEF(tracks->final_boss[1])
		EXMUSIC_EXISTS_BY_DEF(tracks->final_boss[0])
	}
	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_RACE) EXMUSIC_EXISTS_BY_DEF(tracks->race)
	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH) EXMUSIC_EXISTS_BY_DEF(tracks->regular_boss[1])
	EXMUSIC_EXISTS_BY_DEF(tracks->regular_boss[0])

	return NULL;
}
