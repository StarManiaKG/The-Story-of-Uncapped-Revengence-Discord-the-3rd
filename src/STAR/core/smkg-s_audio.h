// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_audio.h
/// \brief Globalizes TSoURDt3rd's sound library data

#ifndef __SMKG_S_AUDIO__
#define __SMKG_S_AUDIO__

#include "../ss_main.h"

#include "../../doomstat.h"
#include "../../s_sound.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Functions
// ------------------------ //

#define TSOURDT3RD_CHECK_FOR_MUSIC_TYPE(type) _Static_assert( \
	_Generic((type), \
		char             * : 1, \
		const char       * : 1, \
		musicdef_t       * : 1, \
		const musicdef_t * : 1, \
		default            : 0 \
	), \
#type": incorrect type.")

boolean TSoURDt3rd_S_MusicDefExists(musicdef_t *def, INT32 track);
boolean TSoURDt3rd_S_MusicNameExists(const char *music, INT32 track);
#define TSoURDt3rd_S_MusicExists(type, track, func) { \
	boolean valid_track = false; \
	do { \
		TSOURDT3RD_CHECK_FOR_MUSIC_TYPE(type); \
		valid_track = _Generic((type), \
			char             * : TSoURDt3rd_S_MusicNameExists, \
			const char       * : TSoURDt3rd_S_MusicNameExists, \
			musicdef_t       * : TSoURDt3rd_S_MusicDefExists, \
			const musicdef_t * : TSoURDt3rd_S_MusicDefExists \
		)(type, track); \
	} while(0); \
	if (valid_track == true) { func } \
}

void TSoURDt3rd_S_RefreshMusic(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_S_AUDIO__
