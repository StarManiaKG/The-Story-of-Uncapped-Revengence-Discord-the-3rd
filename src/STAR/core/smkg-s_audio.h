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
//          Macros
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

#define TSoURDt3rd_S_MusicExists(type, func, ...) \
{ \
	boolean valid_track = false; \
	do { \
		TSOURDT3RD_CHECK_FOR_MUSIC_TYPE(type); \
		valid_track = _Generic((type), \
			char             * : TSoURDt3rd_S_MusicNameExists, \
			const char       * : TSoURDt3rd_S_MusicNameExists, \
			musicdef_t       * : TSoURDt3rd_S_MusicDefExists, \
			const musicdef_t * : TSoURDt3rd_S_MusicDefExists \
		)(type, ##__VA_ARGS__); \
	} while(0); \
	if (valid_track == true) \
	{ \
		func \
	} \
}

#define TSoURDt3rd_S_TunesAreCancelled() \
{ \
	if (TSoURDt3rd_AprilFools_ModeEnabled()) \
	{ \
		STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "Nice try. Perhaps there's a command you need to turn off first?\n"); \
		return; \
	} \
	else if (TSoURDt3rd_Jukebox_IsPlaying()) \
	{ \
		STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Sorry, you can't use this command while playing music.\n"); \
		return; \
	} \
}

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_S_MusicDefExists(musicdef_t *def);
boolean TSoURDt3rd_S_MusicNameExists(const char *music);

boolean TSoURDt3rd_S_CanModifyMusic(char *menu_mus_origin);
void TSoURDt3rd_S_ControlMusicEffects(const size_t *argc, UINT32 *position);

void TSoURDt3rd_S_RefreshMusic(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_S_AUDIO__
