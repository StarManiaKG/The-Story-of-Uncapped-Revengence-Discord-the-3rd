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

#include "smkg-cvars.h" // cv_watermuffling, cv_allowtypicaltimeover, & TSOURDT3RD_TIMELIMIT //
#include "smkg-i_sys.h" // TSoURDt3rd_I_CursedWindowMovement() //

#include "ss_main.h" // STAR_CONS_Printf() //
#include "p_user.h"
#include "s_sound.h"
#include "../doomdef.h"
#include "../g_game.h"
#include "../s_sound.h"
#include "../z_zone.h"

#include "../m_menu.h" // cv_bosspinchmusic //

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

static float prev_musicspeed, prev_musicpitch;
static INT32 prev_musicvolume, prev_sfxvolume;

#define TSOURDT3RD_TIMELIMIT (20999) // one tic off so the timer doesn't display 10:00.00

#define TSOURDT3RD_MUFFLEINT (0.15f)

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

	static boolean alreadyInWater;

	if (!TSoURDt3rd || !player || !player->mo)
		return;

	// Water muffling
	if (!TSoURDt3rd->jukebox.curtrack && cv_watermuffling.value)
	{
		if ((player->mo->eflags & MFE_UNDERWATER) && !alreadyInWater)
		{
			prev_musicspeed = S_GetSpeedMusic()-TSOURDT3RD_MUFFLEINT;
			prev_musicpitch = S_GetPitchMusic()-TSOURDT3RD_MUFFLEINT;

			prev_musicvolume = S_GetInternalMusicVolume()/2;
			prev_sfxvolume = S_GetInternalSfxVolume()/3;

			S_SpeedMusic(prev_musicspeed);
			S_PitchMusic(prev_musicpitch);

			if (S_GetInternalMusicVolume() >= prev_musicvolume)
				S_SetInternalMusicVolume(prev_musicvolume);
			S_SetInternalSfxVolume(prev_sfxvolume);
		}
		else if (!(player->mo->eflags & MFE_UNDERWATER) && alreadyInWater)
		{
			S_SpeedMusic(prev_musicspeed+TSOURDT3RD_MUFFLEINT);
			S_PitchMusic(prev_musicpitch+TSOURDT3RD_MUFFLEINT);

			if (prev_musicvolume*2 >= S_GetInternalMusicVolume())
				S_SetInternalMusicVolume(prev_musicvolume*2); 
			S_SetInternalSfxVolume(prev_sfxvolume*3);
		}

		alreadyInWater = (player->mo->eflags & MFE_UNDERWATER);
	}
}

#undef TSOURDT3RD_MUFFLEINT

#if 0
// STAR NOTE: TODO: come back //
#include "drrr/kg_input.h" // G_PlayerDeviceRumble() //

static inline void P_DeviceRumbleTick(void)
{
	UINT8 i;

	for (i = 0; i <= MAXPLAYERS; i++)
	{
		player_t *player = &players[i];

		UINT16 low = 0;
		UINT16 high = 0;

		if (!P_IsLocalPlayer(player))
			continue;

		if (player->mo != NULL && !player->exiting)
		{
#if 0
			if ((player->mo->eflags & MFE_DAMAGEHITLAG) && player->mo->hitlag)
			{
				low = high = 65536 / 2;
			}
			else if (player->sneakertimer > (sneakertime-(TICRATE/2)))
			{
				low = high = 65536 / (3+player->numsneakers);
			}
			else if (((player->boostpower < FRACUNIT) || (player->stairjank > 8))
				&& P_IsObjectOnGround(player->mo) && player->speed != 0)
			{
				low = high = 65536 / 32;
			}
#else
			if (player->powers[pw_sneakers])
			{
				low = high = 65536 / (3+player->powers[pw_sneakers]);
			}
#endif
		}

		G_PlayerDeviceRumble(i, low, high);
	}
}
#endif

//
// void TSoURDt3rd_P_Ticker(void)
// General TSoURDt3rd gameplay ticker.
//
void TSoURDt3rd_P_Ticker(void)
{
	INT32 i, j;

	for (i = 0, j = 0; i < MAXPLAYERS; i++)
	{
		if (!playeringame[i] || players[i].spectator)
			continue;

		if (!players[i].mo)
			continue;

        player_t *player = &players[i];

		if (!netgame && ((leveltime >= TSOURDT3RD_TIMELIMIT && cv_allowtypicaltimeover.value) || countdowntimeup))
		{
			// Time over...
			TSoURDt3rdPlayers[i].timeOver = true;
			P_DamageMobj(player->mo, NULL, NULL, 1, DMG_INSTAKILL);
		}

		if (TSoURDt3rd_InAprilFoolsMode())
		{
			// Removed Sonic (real)
			if (strstr(skins[player->skin].name, "sonic"))
				continue;

			for (j = MAXSKINS-1; j > 0; j++)
			{
				if (skins[j].name[0] != '\0' && R_SkinUsable(-1, j))
					break;
			}

			SetPlayerSkinByNum(i, j);
			if (P_IsLocalPlayer(player))
			{
				if (splitscreen && i == 1)
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "Your friend can't play as Sonic either, he's gone.\n");
					CV_StealthSet(&cv_skin2, skins[j].name);
				}
				else
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "You can't play as Sonic, he's dead.\n");
					CV_StealthSet(&cv_skin, skins[j].name);
				}

				if (strstr(skins[j].name, "sonic") || strstr(skins[j].realname, "Sonic"))
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "But no skin other than sonic found was found, so uh..............\n\tI guess you're now legally distinct Sonic then!\n");
					player->skincolor = SKINCOLOR_WHITE;
				}
			}
		}
	}

	if (quake.time)
	{
		// Quaking
		TSoURDt3rd_I_CursedWindowMovement(FixedInt(quake.x), FixedInt(quake.y));
	}
}

//
// boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
// Controls and creates unique TSoURDt3rd events should a mobj be damaged.
//
boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
{
	(void)inflictor;
	(void)source;
	(void)damage;
	(void)damagetype;

	if (target)
	{
		INT32 phealth = (target->info->damage ? target->info->damage : 3);

		if ((target->flags & MF_BOSS) && target->health <= phealth)
		{
			if (!cv_bosspinchmusic.value)
				return false;

			strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);
			mapmusname[6] = 0;

			if (TSoURDt3rdPlayers[consoleplayer].jukebox.curtrack)
				return false;

			S_ChangeMusicEx(mapmusname, mapmusflags, true, TSoURDt3rd_PinchMusicPosition(), 0, 0);
			if (TSoURDt3rd_SetPinchMusicSpeed())
				S_SpeedMusic(1.1f);

			return true;
		}
	}

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
