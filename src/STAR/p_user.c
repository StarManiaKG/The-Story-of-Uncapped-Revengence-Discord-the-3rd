// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  p_user.c
/// \brief TSoURDt3rd exclusive user and player functions

#include "p_user.h"

#include "smkg-cvars.h"
#include "smkg-i_sys.h" // TSoURDt3rd_I_QuakeWindow() //
#include "ss_main.h" // STAR_CONS_Printf() //
#include "star_vars.h" // TSoURDt3rd_DetermineLevelMusic() //
#include "core/smkg-g_game.h" // tsourdt3rd_local //
#include "core/smkg-p_pads.h" // TSoURDt3rd_P_Pads_PadRumbleThink() //
#include "core/smkg-s_jukebox.h"

#include "../g_game.h"
#include "../p_local.h"
#include "../r_skins.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_MUFFLEINT (0.15f)
#define TSOURDT3RD_TIMELIMIT (((60*TICRATE) * 10) - 1)

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
	if (!CanChangeSkin(playernum)) return false;
	if ((gametyperules & GTR_RACE) && P_PlayerMoving(playernum)) return false;
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
		&& (player->lives <= 0 || tsourdt3rd_user->game.time_over))
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

	// Extra players in SP can't be allowed to continue or end game
	if (no_netgame && player->lives <= 0 && player == &players[consoleplayer] && player->deadtimer >= gameovertics)
	{
		// Continue Logic - Even if we don't have one this handles ending the game
		G_UseContinue();
		return false;
	}

	if (tsourdt3rd_user->game.time_over && !(no_netgame && G_IsSpecialStage(gamemap)))
	{
		// Don't allow "click to respawn" in special stages!
		if (!(G_GametypeUsesCoopStarposts() && (netgame || multiplayer) && cv_coopstarposts.value == 2))
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
	player_t *split_player = (splitscreen ? &players[1] : NULL);

	(void)player;

	if (display_player && display_player->mo)
	{
		tsourdt3rd_local.water_muffling.apply_effect = ((display_player->mo->eflags & MFE_UNDERWATER) && (display_player->mo->health > 0));
		if (split_player && split_player->mo)
		{
			tsourdt3rd_local.water_muffling.apply_effect = (split_player->mo->eflags & MFE_UNDERWATER);
		}
		tsourdt3rd_local.water_muffling.disable_effect = (!tsourdt3rd_local.water_muffling.apply_effect && tsourdt3rd_local.water_muffling.in_effect);
	}

	// Water muffling
	if (cv_tsourdt3rd_audio_watermuffling.value)
	{
		if (tsourdt3rd_local.water_muffling.apply_effect)
		{
			if (!tsourdt3rd_local.water_muffling.in_effect)
			{
				tsourdt3rd_local.water_muffling.prev_music_volume = S_GetInternalMusicVolume();
				tsourdt3rd_local.water_muffling.prev_music_speed = S_GetSpeedMusic();
				tsourdt3rd_local.water_muffling.prev_music_pitch = S_GetPitchMusic();
				tsourdt3rd_local.water_muffling.prev_sfx_volume = S_GetInternalSfxVolume();

				if (tsourdt3rd_local.water_muffling.prev_music_volume > 0)
				{
					tsourdt3rd_local.water_muffling.music_volume = (tsourdt3rd_local.water_muffling.prev_music_volume / 2);
					if (tsourdt3rd_local.water_muffling.music_volume < 1)
						tsourdt3rd_local.water_muffling.music_volume = 1;
				}
				tsourdt3rd_local.water_muffling.music_speed = (tsourdt3rd_local.water_muffling.prev_music_speed - TSOURDT3RD_MUFFLEINT);
				tsourdt3rd_local.water_muffling.music_pitch = (tsourdt3rd_local.water_muffling.prev_music_pitch - TSOURDT3RD_MUFFLEINT);

				if (tsourdt3rd_local.water_muffling.prev_sfx_volume > 0)
					tsourdt3rd_local.water_muffling.sfx_volume = (tsourdt3rd_local.water_muffling.prev_sfx_volume / 3);

				// Enable water muffling!
				tsourdt3rd_local.water_muffling.in_effect = true;
			}

			// Apply effects to music...
			if (!TSoURDt3rd_Jukebox_SongPlaying())
			{
				S_SetInternalMusicVolume(tsourdt3rd_local.water_muffling.music_volume);
				S_SpeedMusic(tsourdt3rd_local.water_muffling.music_speed);
				S_PitchMusic(tsourdt3rd_local.water_muffling.music_pitch);
			}

			// Apply effects to sounds...
			S_SetInternalSfxVolume(tsourdt3rd_local.water_muffling.sfx_volume);
		}

		if (tsourdt3rd_local.water_muffling.in_effect)
		{
			if (!tsourdt3rd_local.water_muffling.apply_effect/*tsourdt3rd_local.water_muffling.disable_effect*/)
			{
				// Remove effects to music...
				if (!TSoURDt3rd_Jukebox_SongPlaying())
				{
					S_SetInternalMusicVolume(tsourdt3rd_local.water_muffling.prev_music_volume);
					S_SpeedMusic(tsourdt3rd_local.water_muffling.prev_music_speed);
					S_PitchMusic(tsourdt3rd_local.water_muffling.prev_music_pitch);
				}

				// Remove effects to sounds...
				S_SetInternalSfxVolume(tsourdt3rd_local.water_muffling.prev_sfx_volume);

				// Disable water muffling!
				tsourdt3rd_local.water_muffling.in_effect = false;
			}
#if 0
			/// \todo STAR NOTE: improve
			else
			{
				if (S_GetInternalMusicVolume() != tsourdt3rd_local.water_muffling.music_volume)
					S_SetInternalMusicVolume(tsourdt3rd_local.water_muffling.music_volume);

				if ((S_GetSpeedMusic() <= tsourdt3rd_local.water_muffling.music_speed)
					&& (S_GetSpeedMusic() >= tsourdt3rd_local.water_muffling.music_speed))
					S_SpeedMusic(tsourdt3rd_local.water_muffling.music_speed);
				if ((S_GetPitchMusic() <= tsourdt3rd_local.water_muffling.music_pitch)
					&& (S_GetPitchMusic() >= tsourdt3rd_local.water_muffling.music_pitch))

				if (S_GetInternalSfxVolume() != tsourdt3rd_local.water_muffling.sfx_volume)
					S_SetInternalSfxVolume(tsourdt3rd_local.water_muffling.sfx_volume);
			}
#endif
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

			if (!playeringame[i] || player == NULL || player->spectator)
			{
				// We aren't in game, or aren't even real, so just quit...
				continue;
			}

			if (tsourdt3rd_user != NULL)
			{
				// Time over...
				if (leveltime < TSOURDT3RD_TIMELIMIT)
					tsourdt3rd_user->game.time_over = false;

				if (cv_tsourdt3rd_game_allowtimeover.value && !(netgame || demoplayback) && countdowntimeup <= 0)
				{
					tic_t levtime = ((!(netgame && multiplayer) && player->playerstate == PST_LIVE) ? leveltime : player->realtime);
					if (levtime >= TSOURDT3RD_TIMELIMIT)
					{
						tsourdt3rd_user->game.time_over = true;
						if (player->mo)
							P_DamageMobj(player->mo, NULL, NULL, 1, DMG_INSTAKILL);
						if (splitscreen || multiplayer)
							player->lives = 0;
					}
				}
			}

			// Removed Sonic (real)
			if (TSoURDt3rd_AprilFools_ModeEnabled() && P_IsLocalPlayer(player) && strstr(skins[player->skin]->name, "sonic"))
			{
				skin_t *current_skin = skins[0];

				for (INT32 skin = 0; skin < MAXSKINS; skin++)
				{
					current_skin = skins[skin];

					if (current_skin->name[0] == '\0')
						continue;
					if (strstr(current_skin->name, "sonic"))
						continue;

					if (R_SkinUsable(-1, skin))
					{
						SetPlayerSkinByNum(i, skin);
						break;
					}
				}

				if (splitscreen && i == 1)
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_APRILFOOLS|STAR_CONS_WARNING, "Your friend can't play as Sonic either, he's gone.\n");
					CV_StealthSet(&cv_skin2, current_skin->name);
				}
				else
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_APRILFOOLS|STAR_CONS_WARNING, "You can't play as Sonic, he's dead.\n");
					CV_StealthSet(&cv_skin, current_skin->name);
				}

				if (strstr(current_skin->name, "sonic") || strstr(current_skin->realname, "Sonic"))
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_APRILFOOLS|STAR_CONS_WARNING, "But no skin other than sonic found was found, so uh..............\n\tI guess you're now legally distinct Sonic then!\n");
					player->skincolor = SKINCOLOR_WHITE;
				}
			}

			if (player->mo)
			{
				// Apply controller rumble to local players
				TSoURDt3rd_P_Pads_PadRumbleThink(player->mo, NULL);
			}
		}

#if 0
		// Quaking
		if (quake.time)
		{
			TSoURDt3rd_I_QuakeWindow(FixedInt(quake.x), FixedInt(quake.y));
		}
#endif
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
		if (inflictor)
		{
			// Apply controller rumble to local players
			TSoURDt3rd_P_Pads_PadRumbleThink(inflictor, target);
		}

		if (target->flags & MF_BOSS)
		{
			strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);
			mapmusname[6] = 0;
			if (TSoURDt3rd_Jukebox_IsPlaying())
				return false;
			S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
		}
	}

	return (target != NULL);
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

			if ((tsourdt3rd[consoleplayer].game.time_over || t_player->lives <= 0) && G_GametypeUsesLives())
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
// boolean TSoURDt3rd_P_ControlShieldOverlay(mobj_t *thing)
// If the function returns false, it overrides the functionality of overlaying shields.
//
boolean TSoURDt3rd_P_ControlShieldOverlay(mobj_t *thing)
{
	player_t *player = thing->target->player;

	if ((player->powers[pw_shield] & SH_NOSTACK) == SH_NONE)
		return true;

	if (player->powers[pw_super])
	{
		if (player->powers[pw_invulnerability] > 1)
			return (cv_tsourdt3rd_players_alwaysoverlayinvulnsparks.value);
		return true;
	}

	return false;
}

//
// boolean TSoURDt3rd_P_PlayerShieldThink(player_t *player, ticcmd_t *cmd, mobj_t *lockonthok, mobj_t *visual)
// If the function returns false, it overrides the functionality of using shield abilities.
//
boolean TSoURDt3rd_P_PlayerShieldThink(player_t *player, ticcmd_t *cmd, mobj_t *lockonthok, mobj_t *visual)
{
	(void)cmd;
	(void)lockonthok;
	(void)visual;

	if (!(player->powers[pw_shield] & SH_NOSTACK))
		return false;

	if (player->pflags & PF_SPINDOWN)
		return false;

	if (!player->powers[pw_super])
	{
		return (TSoURDt3rd_P_SuperReady(player));
	}
	else
	{
		if (((player->powers[pw_shield] & SH_NOSTACK) == SH_ARMAGEDDON) && cv_tsourdt3rd_players_nukewhilesuper.value)
		{
			// Armageddon pow
			player->pflags |= PF_THOKKED|PF_SHIELDABILITY;
			player->pflags &= ~PF_SPINNING;
			P_BlackOw(player);
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
	if (!(player->pflags & PF_JUMPED))
		return false;

	if (!netgame && (TSoURDt3rd_AprilFools_ModeEnabled() || (TSoURDt3rd_Easter_AllEggsCollected() && EnableEasterEggHuntBonuses && ALL7EMERALDS(emeralds))))
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

	if (player->powers[pw_super])
		return false;
	if (!(player->charflags & SF_SUPER))
		return false;
	if ((player->rings < 50) || !ALL7EMERALDS(emeralds))
		return false;

	if (player->powers[pw_tailsfly])
		return false;
	if (maptol & TOL_NIGHTS)
		return false;

	if (cv_tsourdt3rd_players_shieldblockstransformation.value)
	{
		// This command mimics vanilla behavior.
		// If we have a shield, we can't transform. Sad, but necessary.
		// (If only BT_SHIELD existed.... Oh well....)
		return (!player->powers[pw_invulnerability] && !(player->powers[pw_shield] & SH_NOSTACK));
	}
	return true;
}
