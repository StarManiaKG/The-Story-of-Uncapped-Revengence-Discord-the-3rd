// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  core/exmusic_types/smkg-defaultmaptracks.c
/// \brief An EXMusic type that specializes in playing fallback music.

#include "../smkg-s_exmusic.h"

#include "../../../w_wad.h"

// ------------------------ //
//        Variables
// ------------------------ //

// This is the track data for our new EXMusic type's tracks.
typedef struct defaultmaptrack_data_s {
	musicdef_t *lump; // -- The lump for this series.
} defaultmaptrack_data_t;

// These are the necessary routines for our new EXMusic type.
// Their code is defined later on in this script.
static musicdef_t *__play(consvar_t *cvar);

// When a new EXMusic structure is created,
//  this is the basic info that gets inserted into that structure.
static defaultmaptrack_data_t default_track_def = { &soundtestsfx };
static tsourdt3rd_exmusic_data_identifiers_t data_identifiers[] = {
	{ &cv_tsourdt3rd_audio_exmusic_defaultmaptrack, NULL }
};
static tsourdt3rd_exmusic_data_t tsourdt3rd_default_defaultmaptrack_data = {
	sizeof(defaultmaptrack_data_t *), data_identifiers, NULL, &default_track_def, 0
};
tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_defaultmaptrack = {
	false, sizeof(defaultmaptrack_data_t), "defaultmaptrack", &tsourdt3rd_default_defaultmaptrack_data,
	NULL, NULL, __play, NULL,
	NULL,
	NULL
};

// Our new EXMusic type. Isn't it precious?
tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_defaultmaptrack = NULL;

// The max amount of lumps that this EXMusic type's data can currently hold.
#define TSOURDT3RD_EXMUSIC_MAX_DEFAULTMAPTRACK_LUMPS 25

// ------------------------ //
//        Functions
// ------------------------ //

static lumpnum_t S_GetMusicLumpNum(const char *mname)
{
	boolean midipref = cv_musicpref.value;

	if (S_PrefAvailable(midipref, mname))
		return W_GetNumForName(va(midipref ? "d_%s":"o_%s", mname));
	else if (S_PrefAvailable(!midipref, mname))
		return W_GetNumForName(va(midipref ? "o_%s":"d_%s", mname));

	return LUMPERROR;
}

static musicdef_t *__play(consvar_t *cvar)
{
	(void)cvar;
	return NULL;
}

void TSoURDt3rd_EXMusic_DefaultMapTrack_Play(const char **mname, lumpnum_t *mlumpnum)
{
	tsourdt3rd_exmusic_data_series_t **series_p = NULL, *series_to_use = NULL;
	musicdef_t *track_to_use = NULL;

#if 0
	static const char *new_mname;
#endif
	boolean set_random_entry = false;
	size_t rand_val;

#if 1
	return;
#endif
	if (tsourdt3rd_global_exmusic_defaultmaptrack == NULL)
		return;
	rand_val = M_RandomRange(TSOURDT3RD_EXMUSIC_MAX_STARTING_TRACKS, tsourdt3rd_global_exmusic_defaultmaptrack->data->num_series);
	series_p = tsourdt3rd_global_exmusic_defaultmaptrack->data->series;

	// Check if we're supposed to do anything in the first place...
	if (!cv_tsourdt3rd_audio_exmusic_defaultmaptrack.value || series_p == NULL)
	{
		// -- We can't exact do EXMusic stuff right now, so...
		return;
	}
	else if ((*mlumpnum) != LUMPERROR)
	{
		// -- The track exists, so let's NOT do anything we can't take back.
		return;
	}
	CONS_Alert(CONS_ERROR, "Music %.6s could not be loaded: lump not found!\n", (*mname));

	// Access our EXMusic entry...
	series_to_use = series_p[cv_tsourdt3rd_audio_exmusic_defaultmaptrack.value];
	track_to_use = ((defaultmaptrack_data_t *)series_to_use->tracks)->lump;
	set_random_entry = (track_to_use == (((defaultmaptrack_data_t *)series_p[TSOURDT3RD_EXMUSIC_STARTING_RANDOM]->tracks)->lump));
	while (set_random_entry)
	{
		tsourdt3rd_exmusic_data_series_t *rand_series = series_p[rand_val];
		if (rand_series != NULL)
		{
			track_to_use = ((defaultmaptrack_data_t *)rand_series->tracks)->lump;
			break;
		}
	}
	if (track_to_use)
	{
		(*mname) = track_to_use->name;
		(*mlumpnum) = S_GetMusicLumpNum(track_to_use->name);
	}

	// Play the track if we can!
	if ((*mlumpnum) == LUMPERROR)
	{
		// Oops, -- the track we WANT to override with doesn't exist! So, let's reset the command.
		COM_BufAddText(va("%s \"0\"\n", cv_tsourdt3rd_audio_exmusic_defaultmaptrack.name));
		return;
	}
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE,
		"Playing %s music lump \x82'%.6s'\x80 as requested by your EXMusic settings...\n",
		(set_random_entry ? "random" : "\b"), (*mname)
	);
}
