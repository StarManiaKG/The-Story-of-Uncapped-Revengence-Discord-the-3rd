// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_audio.c
/// \brief TSoURDt3rd's cool and groovy jukebox features

#include "smkg-s_audio.h"

#include "../core/smkg-s_jukebox.h"
#include "../menus/smkg-m_sys.h"

#include "../../g_game.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// Music validation routines
//
#define MUSIC_EXISTS(music) \
	if (music == NULL || *music == '\0') return false; \
	return (S_MusicExists(music, !midi_disabled, !digital_disabled));

boolean TSoURDt3rd_S_MusicDefExists(musicdef_t *def)
{
	if (def == NULL) return false;
	MUSIC_EXISTS(def->name);
}
boolean TSoURDt3rd_S_MusicNameExists(const char *music)
{
	MUSIC_EXISTS(music);
}

//
// boolean TSoURDt3rd_S_CanModifyMusic(char *menu_mus_origin)
// Prevents TSoURDt3rd's music stuff, like Jukebox music, from being forcibly reset or modified. (YAY!)
//
boolean TSoURDt3rd_S_CanModifyMusic(char *menu_mus_origin)
{
	if (menuactive && (menu_mus_origin != NULL && *menu_mus_origin != '\0') && tsourdt3rd_currentMenu != NULL)
		return false;

	if (!TSoURDt3rd_Jukebox_IsPlaying())
		return true;

	if (paused)
		S_ResumeAudio();

	return false;
}

//
// void TSoURDt3rd_ControlMusicEffects(const size_t *argc, UINT32 *position)
// Controls the Effects of the Currently Playing Music, Based on Factors like Vape Mode
//
void TSoURDt3rd_S_ControlMusicEffects(const size_t *argc, UINT32 *position)
{
	float new_music_speed = 0.0f, new_music_pitch = 0.0f;

	if (TSoURDt3rd_Jukebox_IsPlaying())
	{
		new_music_speed = atof(cv_tsourdt3rd_jukebox_speed.string);
		new_music_pitch = atof(cv_tsourdt3rd_jukebox_pitch.string);
	}
	else
	{
		switch (cv_tsourdt3rd_audio_vapemode.value)
		{
			case 1:
				new_music_speed = new_music_pitch = 0.9f;
				break;
			case 2:
				new_music_speed = 0.75;
				new_music_pitch = 0.5f;
				break;
			default:
				new_music_speed = new_music_pitch = 1.0f;
				break;
		}
		if (argc)
		{
			if ((*argc) > 3)
				new_music_speed = (float)atof(COM_Argv(3));
			if ((*argc) > 4)
				new_music_pitch = (float)atof(COM_Argv(4));
			if ((*argc) > 5 && position)
				(*position) = (UINT32)atoi(COM_Argv(5));
		}
	}

	if (new_music_speed > 0.0f) S_SpeedMusic(new_music_speed);
	if (new_music_pitch > 0.0f) S_PitchMusic(new_music_pitch);
}

//
// void TSoURDt3rd_S_RefreshMusic(void)
// Refreshes game music.
//
void TSoURDt3rd_S_RefreshMusic(void)
{
	player_t *player = &players[consoleplayer];
	boolean loop = (Playing() || player->playerstate != PST_DEAD);

	if (TSoURDt3rd_Jukebox_IsPlaying())
	{
		// No.
		return;
	}

	if (player)
	{
		if (player->powers[pw_super])
			P_PlayJingle(player, JT_SUPER);

		if (!S_MusicPlaying())
		{
			if (netgame || multiplayer)
				P_RestoreMultiMusic(player);
			else
				P_RestoreMusic(player);
		}
	}

	if (!S_MusicPlaying())
	{
		// Last resort...
		S_ChangeMusicEx(TSoURDt3rd_DetermineLevelMusic(), mapmusflags, loop, mapmusposition, 0, 0);
	}
}
