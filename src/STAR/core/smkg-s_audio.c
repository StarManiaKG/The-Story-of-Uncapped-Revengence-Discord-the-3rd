// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_audio.c
/// \brief TSoURDt3rd's wavey audio junk

#include "smkg-s_audio.h"
#include "smkg-s_exmusic.h"
#include "smkg-s_jukebox.h"

#include "../menus/smkg-m_sys.h"

#include "../../g_game.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// Music validation routines
//
#define MUSIC_EXISTS(music) \
	if (music == NULL || music[0] == '\0') return false; \
	return (S_MusicExists(music, !midi_disabled, !digital_disabled));

boolean TSoURDt3rd_S_MusicDefExists(musicdef_t *def, INT32 track)
{
	if (def == NULL) return false;
	MUSIC_EXISTS(def->name[track]);
}
boolean TSoURDt3rd_S_MusicNameExists(const char *music, INT32 track)
{
	(void)track;
	MUSIC_EXISTS(music);
}

//
// void TSoURDt3rd_S_RefreshMusic(void)
// Refreshes game music.
//
void TSoURDt3rd_S_RefreshMusic(void)
{
	player_t *player = &players[displayplayer];

	if (TSoURDt3rd_Jukebox_IsPlaying() || S_MusicPlaying())
		return;

	if (Playing())
	{
		if (player->powers[pw_super])
			P_PlayJingle(player, JT_SUPER);

		if (!S_MusicPlaying() && (netgame || multiplayer))
			P_RestoreMultiMusic(player);

		if (!S_MusicPlaying())
			P_RestoreMusic(player);
	}
	else
	{
		TSoURDt3rd_M_PlayMenuJam();

		if (!S_MusicPlaying())
			S_ChangeMusicInternal("_title", false);
	}
}
