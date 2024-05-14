// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  p_user.c
/// \brief TSoURDt3rd exclusive user and player functions

#include "star_vars.h"
#include "ss_cmds.h" // cv_watermuffling //
#include "ss_main.h"
#include "p_user.h"
#include "s_sound.h"
#include "../doomdef.h"
#include "../g_game.h"
#include "../s_sound.h"
#include "../z_zone.h"

#include "../m_menu.h" // cv_bosspinchmusic //

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_PlayerThink(player_t *player)
// Controls many new features that TSoURDt3rd allows for its players.
//
void TSoURDt3rd_PlayerThink(player_t *player)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];
	player = &players[displayplayer];

	if (!TSoURDt3rd || !player || !player->mo)
		return;

	static float prev_musicspeed, prev_musicpitch;
	static INT32 prev_musicvolume, prev_sfxvolume;

	static boolean alreadyInWater;

	// Water muffling //
	if (!TSoURDt3rd->jukebox.musicPlaying && cv_watermuffling.value)
	{
		if ((player->mo->eflags & MFE_UNDERWATER) && !alreadyInWater)
		{
			prev_musicspeed = S_GetSpeedMusic()-0.15f;
			prev_musicpitch = S_GetPitchMusic()-0.15f;

			prev_musicvolume = S_GetInternalMusicVolume()/2;
			prev_sfxvolume = S_GetInternalSfxVolume()/3;

			if (!TSoURDt3rd->jukebox.musicPlaying)
			{
				S_SpeedMusic(prev_musicspeed);
				S_PitchMusic(prev_musicpitch);
			}

			if (S_GetInternalMusicVolume() >= prev_musicvolume)
				S_SetInternalMusicVolume(prev_musicvolume);
			S_SetInternalSfxVolume(prev_sfxvolume);
		}
		else if (!(player->mo->eflags & MFE_UNDERWATER) && alreadyInWater)
		{
			if (!TSoURDt3rd->jukebox.musicPlaying)
			{
				S_SpeedMusic(prev_musicspeed+0.15f);
				S_PitchMusic(prev_musicpitch+0.15f);
			}

			if (prev_musicvolume*2 >= S_GetInternalMusicVolume())
				S_SetInternalMusicVolume(prev_musicvolume*2); 
			S_SetInternalSfxVolume(prev_sfxvolume*3);
		}

		alreadyInWater = (player->mo->eflags & MFE_UNDERWATER);
	}
}

//
// boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
// Controls and creates unique TSoURDt3rd events should a mobj be damaged.
//
boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
{
	if (target)
	{
		INT32 pinchHealth = (target->info->damage ? target->info->damage : 3);

		if (((target->flags & MF_BOSS) && target->health == pinchHealth) && cv_bosspinchmusic.value)
		{
			strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);
			mapmusname[6] = 0;

			S_StopMusic();
			S_ChangeMusicEx(mapmusname, mapmusflags, true, TSoURDt3rd_PinchMusicPosition(), 0, 0);
			if (TSoURDt3rd_SetPinchMusicSpeed())
				S_SpeedMusic(1.1f);

			return true;
		}
	}

	(void)inflictor;
	(void)source;
	(void)damage;
	(void)damagetype;

	return false;
}

//
// boolean TSoURDt3rd_P_SuperReady(player_t *player)
// Checks if the player meets the unique TSoURDt3rd conditions in order to turn super.
//
boolean TSoURDt3rd_P_SuperReady(player_t *player)
{
	if (player->powers[pw_super])
		return false;

	if (!player->rings)
		return false;

	if (TSoURDt3rd_InAprilFoolsMode())
		return true;
	else if (TSoURDt3rd_Easter_AllEggsCollected() && ALL7EMERALDS(emeralds))
		return true;

	return false;
}
