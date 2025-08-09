// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  core/exmusic_types/smkg-gameover.c
/// \brief An EXMusic type that specializes in playing Game Over music.

#include "../smkg-s_exmusic.h"

// ------------------------ //
//        Variables
// ------------------------ //

// This is the track data for our new EXMusic type's tracks.
typedef struct gameover_data_s {
	musicdef_t *lump; // -- The lump for this series.
} gameover_data_t;

// These are the necessary routines for our new EXMusic type.
// Their code is defined later on in this script.
static musicdef_t *__play(consvar_t *cvar);

// When a new EXMusic structure is created,
//  this is the basic info that gets inserted into that structure.
static gameover_data_t default_track_def = { &soundtestsfx };
static tsourdt3rd_exmusic_data_identifiers_t data_identifiers[] = {
	{ &cv_tsourdt3rd_audio_exmusic_gameover, NULL }
};
static tsourdt3rd_exmusic_data_t tsourdt3rd_default_gameover_data = {
	sizeof(gameover_data_t *), data_identifiers, NULL, &default_track_def, 0
};
tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_gameover = {
	false, sizeof(gameover_data_t), "gameover", &tsourdt3rd_default_gameover_data,
	NULL, NULL, __play, NULL,
	NULL,
	NULL
};

// Our new EXMusic type. Isn't it precious?
tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_gameover = NULL;

// The max amount of lumps that this EXMusic type's data can currently hold.
#define TSOURDT3RD_EXMUSIC_MAX_GAMEOVER_LUMPS 50

// ------------------------ //
//        Functions
// ------------------------ //

static musicdef_t *__play(consvar_t *cvar)
{
	(void)cvar;
	return NULL;
}
