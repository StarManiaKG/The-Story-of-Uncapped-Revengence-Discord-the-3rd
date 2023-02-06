// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// DISCORD GAME SDK BY STARMANIAKG(#4884)
// Copyright (C) 2022-2023 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_gamesdk.h
/// \brief Discord Game SDK handling

#ifndef _DISCORDGAMESDK__
#define _DISCORDGAMESDK__

#ifdef HAVE_DISCORDGAMESDK

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "discord_game_sdk.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

extern struct discordGameSDK_s {
    struct IDiscordCore* core;
    struct IDiscordUserManager* users;
    struct IDiscordAchievementManager* achievements;
    struct IDiscordActivityManager* activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager* application;
    struct IDiscordLobbyManager* lobbies;
    
    DiscordUserId user_id;
} discordGameSDK_t;

#define DISCORD_REQUIRE(x) assert(x == DiscordResult_Ok)

/*--------------------------------------------------
	void DRPC_Init(int argc, char** argv)

		Initalizes Discord's Game SDK by linking the Application ID
		and setting the callback functions.
--------------------------------------------------*/

void DRPC_Init(int argc, char** argv);

/*--------------------------------------------------
	void DRPC_UpdatePresence(void);

		Updates what is displayed by Game SDK on the user's profile.
		Should be called whenever something that is displayed is
		changed in-game.
--------------------------------------------------*/

void DRPC_UpdatePresence(void);

#endif // HAVE_DISCORDGAMESDK
#endif // _DISCORDGAMESDK__
