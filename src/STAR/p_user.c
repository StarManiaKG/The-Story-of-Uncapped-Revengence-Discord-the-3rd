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

#include "smkg-cvars.h" // cv_tsourdt3rd_audio_watermuffling, cv_allowtypicaltimeover, & TSOURDT3RD_TIMELIMIT //
#include "smkg-i_sys.h" // TSoURDt3rd_I_CursedWindowMovement() //

#include "ss_main.h" // STAR_CONS_Printf() //
#include "p_user.h"
#include "smkg-jukebox.h"

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
// boolean TSoURDt3rd_P_MovingPlayerSetup(INT32 playernum)
//
// Checks if the player is allowed to move and change their skin at the same time.
// Returns true if so, false otherwise.
//
boolean TSoURDt3rd_P_MovingPlayerSetup(INT32 playernum)
{
	if (!CanChangeSkin(playernum))
		return false;
	if ((gametyperules & GTR_RACE) && P_PlayerMoving(playernum))
		return false;
	return (cv_movingplayersetup.value || (!cv_movingplayersetup.value && !P_PlayerMoving(playernum)));
}

//
// void TSoURDt3rd_P_PlayerThink(void)
// Controls many new features that TSoURDt3rd allows for its players.
//
void TSoURDt3rd_P_PlayerThink(void)
{
	player_t *splayer = &players[displayplayer];
	mobj_t *smo = NULL;
	static boolean alreadyInWater;

	if (!tsourdt3rd_global_jukebox || !splayer)
		return;
	smo = splayer->mo;

	// Water muffling
	if (!tsourdt3rd_global_jukebox->curtrack && cv_tsourdt3rd_audio_watermuffling.value)
	{
		if ((smo->eflags & MFE_UNDERWATER) && !alreadyInWater)
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
		else if (!(smo->eflags & MFE_UNDERWATER) && alreadyInWater)
		{
			S_SpeedMusic(prev_musicspeed+TSOURDT3RD_MUFFLEINT);
			S_PitchMusic(prev_musicpitch+TSOURDT3RD_MUFFLEINT);

			if (prev_musicvolume*2 >= S_GetInternalMusicVolume())
				S_SetInternalMusicVolume(prev_musicvolume*2); 
			S_SetInternalSfxVolume(prev_sfxvolume*3);
		}

		alreadyInWater = (smo->eflags & MFE_UNDERWATER);
	}
}

#undef TSOURDT3RD_MUFFLEINT

#if 1
// STAR NOTE: TODO: come back //
#include "drrr/kg_input.h" // TSoURDt3rd_Pads_G_PlayerDeviceRumble() //

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
				low = high += FRACUNIT * (3+player->powers[pw_sneakers]);
			}
			if (player->mo->state-states == S_PLAY_PAIN || player->mo->state-states == S_PLAY_DEAD)
			{
				low = high += FRACUNIT / 6;
			}
			if ((player->pflags & PF_GLIDING) && (player->mo->eflags & MFE_JUSTHITFLOOR))
			{
				low = high += FRACUNIT / 32;
			}
#endif
		}

		TSoURDt3rd_Pads_G_PlayerDeviceRumble(player, low, high);
	}
}
#endif

//
// void TSoURDt3rd_P_Ticker(void)
// General TSoURDt3rd gameplay ticker.
//
void TSoURDt3rd_P_Ticker(void)
{
	INT32 i, skin;

	for (i = 0; i < MAXPLAYERS; i++)
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

		if (TSoURDt3rd_AprilFools_ModeEnabled())
		{
			// Removed Sonic (real)
			if (strstr(skins[player->skin].name, "sonic"))
				continue;

			for (skin = MAXSKINS-1; skin > 0; skin++)
			{
				if (skins[skin].name[0] != '\0' && R_SkinUsable(-1, skin))
					break;
			}

			SetPlayerSkinByNum(i, skin);
			if (P_IsLocalPlayer(player))
			{
				if (splitscreen && i == 1)
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "Your friend can't play as Sonic either, he's gone.\n");
					CV_StealthSet(&cv_skin2, skins[skin].name);
				}
				else
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "You can't play as Sonic, he's dead.\n");
					CV_StealthSet(&cv_skin, skins[skin].name);
				}

				if (strstr(skins[skin].name, "sonic") || strstr(skins[skin].realname, "Sonic"))
				{
					STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "But no skin other than sonic found was found, so uh..............\n\tI guess you're now legally distinct Sonic then!\n");
					player->skincolor = SKINCOLOR_WHITE;
				}
			}
		}
	}

	// Quaking
	if (quake.time)
	{
		TSoURDt3rd_I_CursedWindowMovement(FixedInt(quake.x), FixedInt(quake.y));
	}

	// Apply rumble to local players
	if (!demoplayback)
	{
		P_DeviceRumbleTick();
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

			if (tsourdt3rd_global_jukebox->curtrack)
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
	if (TSoURDt3rd_AprilFools_ModeEnabled())
		return true;
	else if (TSoURDt3rd_Easter_AllEggsCollected() && EnableEasterEggHuntBonuses && ALL7EMERALDS(emeralds) && !netgame)
	{
		if (gametyperules & GTR_POWERSTONES)
		{
			if (players[consoleplayer].powers[pw_emeralds] != 127)
				players[consoleplayer].powers[pw_emeralds] = ((EMERALD7)*2)-1;
		}
		else
		{
			if (emeralds != 127)
				emeralds = ((EMERALD7)*2)-1;
		}

		if (!(player->charflags & SF_SUPER))
			player->charflags += SF_SUPER;

		return true;
	}

	if (!player->powers[pw_super]
	&& ((cv_shieldblockstransformation.value && !player->powers[pw_invulnerability]) || (!cv_shieldblockstransformation.value))
	&& !player->powers[pw_tailsfly]
	&& (player->charflags & SF_SUPER)
	&& (player->pflags & PF_JUMPED)
	&& ((!(player->powers[pw_shield] & SH_NOSTACK) && cv_shieldblockstransformation.value) || (!cv_shieldblockstransformation.value))
	&& !(maptol & TOL_NIGHTS)
	&& ALL7EMERALDS(emeralds)
	&& (player->rings >= 50))
		return true;

	return false;
}
