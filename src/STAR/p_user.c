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

#include "ss_cmds.h" // cv_watermuffling, cv_allowtypicaltimeover, & TSOURDT3RD_TIMELIMIT //
#include "ss_main.h" // STAR_CONS_Printf() //
#include "p_user.h"
#include "s_sound.h"
#include "../doomdef.h"
#include "../g_game.h"
#include "../s_sound.h"
#include "../z_zone.h"
//#include "../fastcmp.h"
//#include "../v_video.h"
//#include "../d_main.h"

#include "../m_menu.h" // cv_bosspinchmusic //

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h" // DRPC_UpdatePresence() //
#endif

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

	static float prev_musicspeed, prev_musicpitch;
	static INT32 prev_musicvolume, prev_sfxvolume;

	static boolean alreadyInWater;

	if (!TSoURDt3rd || !player || !player->mo)
		return;

	// Water muffling
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
// void TSoURDt3rd_P_Ticker(void)
// General TSoURDt3rd gameplay ticker.
//
void TSoURDt3rd_P_Ticker(void)
{
	INT32 i, j;

#ifdef HAVE_DISCORDSUPPORT
	// All 7 Emeralds //
	if (gametyperules & GTR_POWERSTONES)
	{
		UINT16 MAXTEAMS = 3;

		UINT16 match_emeralds[MAXTEAMS];
		static tic_t emerald_time;

		if (G_GametypeHasTeams()) // If this gametype has teams, check every player on your team for emeralds.
		{
			for (i = 0, j = 1; j < MAXTEAMS; i++)
			{
				if (i >= MAXPLAYERS)
				{
					if (ALL7EMERALDS(match_emeralds[j]))
					{
						all7matchemeralds = true;
						break;
					}
					match_emeralds[j++] = 0;
					i = 0;
				}
				if (players[i].ctfteam == j)
					match_emeralds[j] |= players[i].powers[pw_emeralds];
			}
		}
		else if (ALL7EMERALDS(players[consoleplayer].powers[pw_emeralds]))
			all7matchemeralds = true;

		if (all7matchemeralds)
		{
			if (!emerald_time)
				DRPC_UpdatePresence();
			if (++emerald_time >= 20*TICRATE)
			{
				all7matchemeralds = false;
				emerald_time = 0;
				DRPC_UpdatePresence();
			}
		}
	}
#endif

	for (i = 0, j = 0; i < MAXPLAYERS; i++)
	{
		if (!playeringame[i] || players[i].spectator)
			continue;

		if (!players[i].mo)
			continue;

		// Time over... //
		if (!netgame && ((leveltime >= TSOURDT3RD_TIMELIMIT && cv_allowtypicaltimeover.value) || countdowntimeup))
		{
			TSoURDt3rdPlayers[i].timeOver = true;
			P_DamageMobj(&players[i].mo, NULL, NULL, 1, DMG_INSTAKILL);
		}

		// Removed Sonic (real) //
		if (TSoURDt3rd_InAprilFoolsMode())
		{
			if (!fastncmp(skins[players[i].skin].name, "sonic", 5))
				continue;

			for (j = 0; j < MAXSKINS; j++)
			{
				if (skins[j].name[0] != '\0' && R_SkinUsable(-1, j))
					break;
			}

			if ((splitscreen && i == 1) && P_IsLocalPlayer(&players[i]))
			{
				STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "Your friend can't play as Sonic either, he's gone.\n");
				CV_StealthSet(&cv_skin2, skins[1].name);
			}
			else if (P_IsLocalPlayer(&players[i]))
			{
				STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "You can't play as Sonic, he's dead.\n");
				CV_StealthSet(&cv_skin, skins[1].name);
			}

			SetPlayerSkinByNum(i, j);
			if (fastcmp(skins[j].name, "Sonic") && P_IsLocalPlayer(&players[i]))
			{
				STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "But no skin other than sonic found was found, so uh..............\n\tI guess you're now legally distinct Sonic then!\n");
				players[i].skincolor = SKINCOLOR_WHITE;
			}
		}
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
