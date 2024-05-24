// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_gamesdk.h
/// \brief Discord Game SDK handling data

#ifndef _DISCORDGAMESDK__
#define _DISCORDGAMESDK__

#ifdef HAVE_DISCORDGAMESDK

#include "discord_game_sdk.h"

// ------------------------ //
//         Structs
// ------------------------ //

typedef struct discordGameSDK_s {
	struct IDiscordCore *core;
	struct IDiscordUsers *users;
	struct IDiscordAchievementManager *achievements;
	struct IDiscordActivityManager *activities;
	struct IDiscordRelationshipManager *relationships;
	struct IDiscordApplicationManager *application;
	struct IDiscordLobbyManager *lobbies;
	
	DiscordUserId user_id;
} discordGameSDK_t;

// ------------------------ //
//        Functions
// ------------------------ //

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
