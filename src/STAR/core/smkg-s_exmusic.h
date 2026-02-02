// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_exmusic.h
/// \brief TSoURDt3rd's cool and groovy EXtended Music setup, definition, and replacement routines.

#ifndef __SMKG_S_EXMUSIC__
#define __SMKG_S_EXMUSIC__

#include "smkg-s_audio.h"

#define TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME 30

//
// Default starting data for each EXMusic type and track.
//

enum
{
	tsourdt3rd_exmusic_series_default,
	//tsourdt3rd_exmusic_series_random,
	tsourdt3rd_exmusic_starting_series_max
};

extern const char *tsourdt3rd_exmusic_default_series_names[tsourdt3rd_exmusic_starting_series_max];

//
// Number identifiers for all our EXMusic types.
// Helps identify things like our EXMusic commands.
//

enum
{
	tsourdt3rd_exmusic_defaultmaptrack = 0,
	tsourdt3rd_exmusic_gameover,
	tsourdt3rd_exmusic_bosses,
		tsourdt3rd_exmusic_bosses_pinch,
		tsourdt3rd_exmusic_bosses_finalboss,
		tsourdt3rd_exmusic_bosses_finalboss_pinch,
		tsourdt3rd_exmusic_bosses_truefinalboss,
		tsourdt3rd_exmusic_bosses_truefinalboss_pinch,
		tsourdt3rd_exmusic_bosses_race,
	tsourdt3rd_exmusic_intermission,
		tsourdt3rd_exmusic_intermission_boss,
		tsourdt3rd_exmusic_intermission_finalboss,
		tsourdt3rd_exmusic_intermission_truefinalboss,
	tsourdt3rd_exmusic_max_types
};

extern consvar_t cv_tsourdt3rd_audio_exmusic[tsourdt3rd_exmusic_max_types];

//
// The identifier data for our various EXMusic types.
// Used by things like the STAR parser and the EXMusic menu in order to get the right tracks and settings.
//

typedef struct tsourdt3rd_exmusic_data_identifier_types_s {
	const char *type_name;
	const char *parser_name;
} tsourdt3rd_exmusic_data_identifiers_t;

extern tsourdt3rd_exmusic_data_identifiers_t tsourdt3rd_exmusic_data_identifier_types[tsourdt3rd_exmusic_max_types];

//
// Structs needed for EXMusic to function properly.
//

typedef struct tsourdt3rd_exmusic_trackset_s {
	INT32 num_music_lumps;
	musicdef_t **music;
} tsourdt3rd_exmusic_musicset_t;

typedef struct tsourdt3rd_exmusic_data_series_s {
	char series_name[TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME];
	boolean hardcoded;
	tsourdt3rd_exmusic_musicset_t *track_sets[tsourdt3rd_exmusic_max_types];
	struct tsourdt3rd_exmusic_data_series_s *prev;
	struct tsourdt3rd_exmusic_data_series_s *next;
} tsourdt3rd_exmusic_data_series_t;

typedef struct
{
	INT32 identifier_pos;
	INT32 series_pos;
	INT32 track_pos;
	consvar_t *cvar;
	tsourdt3rd_exmusic_data_identifiers_t *identifier;
	tsourdt3rd_exmusic_data_series_t *series;
	tsourdt3rd_exmusic_musicset_t *track_set;
	musicdef_t *lump;
	INT32 lump_track;
	tsourdt3rd_exmusic_data_series_t **all_series;
	musicdef_t **all_music_lumps;
} tsourdt3rd_exmusic_findTrackResult_t;

//
// A unified collection of all the structs above.
// I call it, EXMusic.
// (I don't know what the 'EX' stands for I just find it cool)
// (9/9/25 - Nevermind it stands for 'EXtended Music' I just forgot :p)
//
// Users can define their own tracks and everything.
// It's all local too.
//
// It definitely isn't the best thing ever, but *I'm* proud of it, and I'm all for user customizability.
// As long as users can make this build be their own special little thing, that's all that matters.
//
// Thanks goes to Marilyn for making CusMusic in her Final Demo source port a while back.
// It allowed the music of a given level to be customized, and this is pretty similar to that.
// I forgot about it until she reminded me of it, but this is kinda a sequel to it in a way.
//

extern tsourdt3rd_exmusic_data_series_t **tsourdt3rd_exmusic_available_series;
extern INT32 tsourdt3rd_exmusic_num_series;
extern boolean tsourdt3rd_exmusic_initialized;

tsourdt3rd_exmusic_data_series_t *TSoURDt3rd_EXMusic_AddNewSeries(const char *name, boolean verbose);
void TSoURDt3rd_EXMusic_Init(void);

tsourdt3rd_exmusic_data_identifiers_t *TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(const char *identifier, INT32 *identifier_p);
tsourdt3rd_exmusic_data_identifiers_t *TSoURDt3rd_EXMusic_ReturnTypeFromCVar(consvar_t *cvar, INT32 *identifier_p);

tsourdt3rd_exmusic_data_series_t *TSoURDt3rd_EXMusic_FindSeries(const char *series, INT32 *series_p);
tsourdt3rd_exmusic_musicset_t *TSoURDt3rd_EXMusic_GetSeriesTrackSet(INT32 series, INT32 identifier, INT32 *series_p);
musicdef_t *TSoURDt3rd_EXMusic_GetTrackData(INT32 series, INT32 identifier, INT32 track, INT32 *series_p);
musicdef_t *TSoURDt3rd_EXMusic_GetSeriesMusic(musicdef_t *new_track, INT32 series, INT32 identifier, INT32 track);
boolean TSoURDt3rd_EXMusic_FindTrack(const char *valstr, INT32 track_set, tsourdt3rd_exmusic_findTrackResult_t *track_result);
boolean TSoURDt3rd_EXMusic_FindCVar(const char *cvar_name, const char *valstr);

void Command_EXMusic_f(void);
boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr);
void TSoURDt3rd_S_EXMusic_Update(void);

lumpnum_t TSoURDt3rd_EXMusic_DefaultMapTrack_Play(const char **mname);
const char *TSoURDt3rd_EXMusic_DetermineLevelMusic(void);

#endif // __SMKG_S_EXMUSIC__
