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

#include "p_user.h"

#include "smkg-cvars.h"
#include "smkg-jukebox.h"
#include "smkg-i_sys.h" // TSoURDt3rd_I_CursedWindowMovement() //
#include "core/smkg-p_pads.h" // TSoURDt3rd_P_Pads_PadRumbleThink() //
#include "smkg-misc.h" // TSoURDt3rd_FOL_CreateDirectory() //
#include "ss_main.h" // STAR_CONS_Printf() //
#include "star_vars.h" // TSoURDt3rd_DetermineLevelMusic() //

#include "../g_game.h"
#include "../p_local.h"
#include "../r_skins.h"

// ------------------------ //
//        Variables
// ------------------------ //

boolean watermuffling_alreadyineffect = false;
static float watermuffling_music_speed, watermuffling_music_pitch;
static INT32 watermuffling_music_volume, watermuffling_sfx_volume;
#define TSOURDT3RD_MUFFLEINT (0.15f)

#define TSOURDT3RD_TIMELIMIT (21004) // one tic off so the timer doesn't display 10:00.00

const char gameoverMusic[9][7] = {
	[0] = "_gover",
	"_govr1",
	"_govcd",
	"_govr3",
	"_govrr",
	"_govrm",
	"_govrs",
	"_govrc",
	"_govry"
};
INT32 gameoverMusicTics[9] = {
	[1] = 4*TICRATE,
	[7] = -5*TICRATE
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_P_MovingPlayerSetup(INT32 playernum)
// Returns whether or not the player is allowed to move and change their skin at the same time.
//
boolean TSoURDt3rd_P_MovingPlayerSetup(INT32 playernum)
{
	if (!CanChangeSkin(playernum))
		return false;
	if ((gametyperules & GTR_RACE) && P_PlayerMoving(playernum))
		return false;
	return (cv_tsourdt3rd_players_setupwhilemoving.value || (!cv_tsourdt3rd_players_setupwhilemoving.value && !P_PlayerMoving(playernum)));
}

//
// boolean TSoURDt3rd_P_DeathThink(player_t *player)
// TSoURDt3rd's player death thinker.
//
boolean TSoURDt3rd_P_DeathThink(player_t *player)
{
	tsourdt3rd_t *tsourdt3rd_user = &tsourdt3rd[consoleplayer];
	static INT32 prev_gameover_tics = -1;
	boolean no_netgame = (!(netgame || multiplayer));

	if (prev_gameover_tics < 0)
	{
		prev_gameover_tics = gameovertics;
		gameovertics += gameoverMusicTics[cv_tsourdt3rd_audio_gameover.value];
	}

	if ((TSoURDt3rd_AprilFools_ModeEnabled() && ultimatemode && !netgame)
		&& (player->lives <= 0 || tsourdt3rd_user->levels.time_over))
	{
		// This is funny.
		if (splitscreen || multiplayer)
		{
			for (INT32 i = 0; i < MAXPLAYERS; i++)
			{
				player_t *local_player = &players[i];
				if (!P_IsLocalPlayer(local_player))
					continue;
				local_player->deadtimer--;
			}
		}
		else
			player->deadtimer--;
		return true;
	}

	if (!P_IsLocalPlayer(player))
	{
		// Sorry, gotta be using TSoURDt3rd or something.
		return false;
	}

	if (no_netgame && player->lives <= 0 && player == &players[consoleplayer] && player->deadtimer >= gameovertics) // Extra players in SP can't be allowed to continue or end game
	{
		// Continue Logic - Even if we don't have one this handles ending the game
		G_UseContinue();
		return false;
	}

	if (tsourdt3rd_user->levels.time_over && !(no_netgame && G_IsSpecialStage(gamemap)))
	{
		// Don't allow "click to respawn" in special stages!
		if (G_GametypeUsesCoopStarposts() && (netgame || multiplayer) && cv_coopstarposts.value == 2)
			;
		else
		{
			if (player->deadtimer >= gameovertics)
			{
				// Single player timeover
				player->playerstate = PST_REBORN;
			}
			return ((player->deadtimer < gameovertics) && (player->playerstate != PST_REBORN));
		}
	}

	if (player->playerstate == PST_REBORN)
	{
		gameovertics = prev_gameover_tics;
		prev_gameover_tics = -1;
	}
	return false;
}

//
// void TSoURDt3rd_P_PlayerThink(player_t *player)
// Controls many new features that TSoURDt3rd allows for its players.
//
void TSoURDt3rd_P_PlayerThink(player_t *player)
{
	player_t *display_player = &players[displayplayer];
	(void)player;

	if (display_player != NULL && display_player->mo != NULL)
	{
		// Water muffling
		if (!TSoURDt3rd_Jukebox_IsPlaying() && cv_tsourdt3rd_audio_watermuffling.value)
		{
			if ((display_player->mo->eflags & MFE_UNDERWATER) && !watermuffling_alreadyineffect)
			{
				watermuffling_music_speed = (S_GetSpeedMusic() - TSOURDT3RD_MUFFLEINT);
				watermuffling_music_pitch = (S_GetPitchMusic() - TSOURDT3RD_MUFFLEINT);

				if (S_GetInternalMusicVolume() > 0)
					watermuffling_music_volume = (S_GetInternalMusicVolume() / 2);
				if (S_GetInternalSfxVolume() > 0)
				watermuffling_sfx_volume = (S_GetInternalSfxVolume() / 3);

				if (watermuffling_music_volume < 1)
					watermuffling_music_volume = 1;

				S_SpeedMusic(watermuffling_music_speed);
				S_PitchMusic(watermuffling_music_pitch);

				S_SetInternalMusicVolume(watermuffling_music_volume);
				S_SetInternalSfxVolume(watermuffling_sfx_volume);
			}
			else if (!(display_player->mo->eflags & MFE_UNDERWATER) && watermuffling_alreadyineffect)
			{
				watermuffling_music_speed += TSOURDT3RD_MUFFLEINT;
				watermuffling_music_pitch += TSOURDT3RD_MUFFLEINT;

				watermuffling_music_volume *= 2;
				watermuffling_sfx_volume *= 3;

				if (watermuffling_music_volume > 100)
					watermuffling_music_volume = 100;

				S_SpeedMusic(watermuffling_music_speed);
				S_PitchMusic(watermuffling_music_pitch);

				S_SetInternalMusicVolume(watermuffling_music_volume);
				S_SetInternalSfxVolume(watermuffling_sfx_volume);
			}
			watermuffling_alreadyineffect = (display_player->mo->eflags & MFE_UNDERWATER);
		}
	}
}

//
// void TSoURDt3rd_P_Ticker(boolean run)
// General TSoURDt3rd gameplay ticker.
//
void TSoURDt3rd_P_Ticker(boolean run)
{
	INT32 i;

	if (run)
	{
		for (i = 0; i < MAXPLAYERS; i++)
		{
			player_t *player = player = &players[i];
			tsourdt3rd_t *tsourdt3rd_user = &tsourdt3rd[i];
			INT32 skin;

			if (!playeringame[i] || player == NULL || player->spectator)
			{
				// We aren't in game, or aren't even real, so just quit...
				continue;
			}

			if (tsourdt3rd_user != NULL)
			{
				// Time over...
				if (leveltime < TSOURDT3RD_TIMELIMIT)
					tsourdt3rd_user->levels.time_over = false;

				if (cv_tsourdt3rd_game_allowtimeover.value && !(netgame || demoplayback) && countdowntimeup <= 0)
				{
					if ((!(netgame && multiplayer) && player->playerstate == PST_LIVE && leveltime >= TSOURDT3RD_TIMELIMIT)
						|| (player->realtime >= TSOURDT3RD_TIMELIMIT))
					{
						tsourdt3rd_user->levels.time_over = true;
						if (player->mo)
							P_DamageMobj(player->mo, NULL, NULL, 1, DMG_INSTAKILL);
						if (splitscreen || multiplayer)
							player->lives = 0;
					}
				}
			}

			// Removed Sonic (real)
			if (TSoURDt3rd_AprilFools_ModeEnabled() && P_IsLocalPlayer(player) && strstr(skins[player->skin].name, "sonic"))
			{
				for (skin = 0; skin < MAXSKINS; skin++)
				{
					if (skins[skin].name[0] == '\0')
						continue;
					if (strstr(skins[skin].name, "sonic"))
						continue;
					if (R_SkinUsable(-1, skin))
					{
						SetPlayerSkinByNum(i, skin);
						break;
					}
				}

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

			if (player->mo)
			{
				// Apply controller rumble to local players
				TSoURDt3rd_P_Pads_PadRumbleThink(player->mo, NULL);
			}
		}

		// Quaking
		if (quake.time)
		{
			TSoURDt3rd_I_CursedWindowMovement(FixedInt(quake.x), FixedInt(quake.y));
		}
	}
}

//
// boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
// Controls and creates unique TSoURDt3rd events should a mobj be damaged.
//
boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype)
{
	(void)source;
	(void)damage;
	(void)damagetype;

	if (target)
	{
		INT32 phealth = (target->info->damage ? target->info->damage : 3);

		if (inflictor)
		{
			// Apply controller rumble to local players
			TSoURDt3rd_P_Pads_PadRumbleThink(inflictor, target);
		}

		if ((target->flags & MF_BOSS) && target->health <= phealth)
		{
			if (!cv_tsourdt3rd_audio_bosses_pinch.value)
				return false;

			strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);
			mapmusname[6] = 0;

			if (TSoURDt3rd_Jukebox_IsPlaying())
				return false;

			S_ChangeMusicEx(mapmusname, mapmusflags, true, TSoURDt3rd_PinchMusicPosition(), 0, 0);
			if (TSoURDt3rd_SetPinchMusicSpeed())
				S_SpeedMusic(1.1f);
		}
	}

	return true;
}

//
// void TSoURDt3rd_P_KillMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, UINT8 damagetype)
// Controls and creates unique TSoURDt3rd events should a mobj be killed.
//
void TSoURDt3rd_P_KillMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, UINT8 damagetype)
{
	INT32 i;

	(void)source;
	(void)damagetype;

	if (target)
	{
		player_t *t_player = target->player;

		if (t_player)
		{
			boolean gameovermus = false;

			if ((tsourdt3rd[consoleplayer].levels.time_over || t_player->lives <= 0) && G_GametypeUsesLives())
			{
				if ((netgame || multiplayer) && G_GametypeUsesCoopLives() && (cv_cooplives.value != 1))
				{
					for (i = 0; i < MAXPLAYERS; i++)
					{
						if (!playeringame[i])
							continue;

						if (players[i].lives > 0)
							break;
					}
					if (i == MAXPLAYERS)
						gameovermus = true;
				}
				else if (P_IsLocalPlayer(t_player))
					gameovermus = true;

				if (gameovermus) // Yousa dead now, Okieday? Tails 03-14-2000 - (With changes from StarManiaKG in 2024)
					S_ChangeMusicEx(gameoverMusic[cv_tsourdt3rd_audio_gameover.value], 0, 0, 0, (2*MUSICRATE) - (MUSICRATE/25), 0);

				// Kinda hacky but gets the job done
				if ((tsourdt3rd[consoleplayer].levels.time_over && t_player->lives <= 0) && (!(netgame || multiplayer || demoplayback || demorecording || metalrecording || modeattacking) && numgameovers < maxgameovers))
				{
					numgameovers++;
					if (!usedCheats && cursaveslot > 0)
						G_SaveGameOver((UINT32)cursaveslot, (target->player->continues <= 0));
				}
			}
		}

		if (inflictor)
		{
			// Apply controller rumble to local players
			TSoURDt3rd_P_Pads_PadRumbleThink(inflictor, target);
		}
	}
}

//
// boolean TSoURDt3rd_P_SuperReady(player_t *player)
// Checks if the player meets the unique TSoURDt3rd conditions in order to turn super.
//
boolean TSoURDt3rd_P_SuperReady(player_t *player)
{
	if (!netgame &&
		(TSoURDt3rd_AprilFools_ModeEnabled()
		|| (TSoURDt3rd_Easter_AllEggsCollected() && EnableEasterEggHuntBonuses && ALL7EMERALDS(emeralds))))
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
	&& ((cv_tsourdt3rd_players_shieldblockstransformation.value && !player->powers[pw_invulnerability]) || (!cv_tsourdt3rd_players_shieldblockstransformation.value))
	&& !player->powers[pw_tailsfly]
	&& (player->charflags & SF_SUPER)
	&& (player->pflags & PF_JUMPED)
	&& ((!(player->powers[pw_shield] & SH_NOSTACK) && cv_tsourdt3rd_players_shieldblockstransformation.value) || (!cv_tsourdt3rd_players_shieldblockstransformation.value))
	&& !(maptol & TOL_NIGHTS)
	&& ALL7EMERALDS(emeralds)
	&& (player->rings >= 50))
		return true;

	return false;
}
