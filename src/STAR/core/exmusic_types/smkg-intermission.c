// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  core/exmusic_types/smkg-intermission.c
/// \brief An EXMusic type that specializes in playing 'Intermission' music.

#include "../smkg-s_exmusic.h"

// ------------------------ //
//        Variables
// ------------------------ //

// This is the track data for our new EXMusic type's tracks.
typedef struct intermission_data_s {
	musicdef_t *normal;				// -- Normal Intermission
	musicdef_t *boss;				// -- Boss Intermission
	musicdef_t *final_boss;			// -- Final Boss Intermission
	musicdef_t *true_final_boss;	// -- True Final Boss Intermission
} intermission_data_t;

// These are the necessary routines for our new EXMusic type.
// Their code is defined later on in this script.
static musicdef_t *__play(consvar_t *cvar);

// When our new EXMusic type is created, this is the basic info that gets inserted.
static intermission_data_t default_track_def = { &soundtestsfx, &soundtestsfx, &soundtestsfx, &soundtestsfx };
static tsourdt3rd_exmusic_data_identifiers_t data_identifiers[] = {
	{ &cv_tsourdt3rd_audio_exmusic_intermission, NULL },
	{ &cv_tsourdt3rd_audio_exmusic_intermission_bosses, "Boss" },
	{ &cv_tsourdt3rd_audio_exmusic_intermission_finalbosses, "Final Boss" },
	{ &cv_tsourdt3rd_audio_exmusic_intermission_truefinalbosses, "True Final Boss" }
};
static tsourdt3rd_exmusic_data_t tsourdt3rd_default_intermission_data = {
	sizeof(intermission_data_t *), data_identifiers, NULL, &default_track_def, 0
};
tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_intermission = {
	false, sizeof(intermission_data_t), "intermission", &tsourdt3rd_default_intermission_data,
	NULL, NULL, __play, NULL,
	NULL,
	NULL
};

// Our new EXMusic type. Isn't it precious?
tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_intermission = NULL;

// The max amount of lumps that this EXMusic type's data can currently hold.
#define TSOURDT3RD_EXMUSIC_MAX_INTERMISSION_LUMPS 50

// ------------------------ //
//        Functions
// ------------------------ //

static musicdef_t *__play(consvar_t *cvar)
{
	tsourdt3rd_world_scenarios_t scenario = tsourdt3rd_local.world.scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types = tsourdt3rd_local.world.scenario_types;
	intermission_data_t *selected_data = NULL;
	musicdef_t *def = NULL;

	if (selected_data == NULL)
	{
		// Aw dang it...
		return NULL;
	}

	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS) EXMUSIC_EXISTS_BY_DEF(selected_data->true_final_boss)
	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS) EXMUSIC_EXISTS_BY_DEF(selected_data->final_boss)
	if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_POSTBOSS) EXMUSIC_EXISTS_BY_DEF(selected_data->boss)
	EXMUSIC_EXISTS_BY_DEF(selected_data->normal)

	return NULL;
}
