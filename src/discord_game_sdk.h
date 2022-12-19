//-----------------------------------------------------------------------------
// ORIGINAL DISCORD RPC PORT BY THE KART KREW
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
//
// DISCORD GAME SDK PORT BY STARMANIAKG(#4884)
// Copyright (C) 2022 by StarMania "Not Telling You My Real Name" KG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord.h
/// \brief Discord Game SDK handling

#ifndef __DISCORD__
#define __DISCORD__

#ifdef HAVE_DISCORDGAMESDK

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "discord/discord.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

typedef struct discord_gamesdk_s {
    struct DiscordCreateParams params;
    struct IDiscordCore* core;
    struct IDiscordUserManager* users;
    struct IDiscordAchievementManager* achievements;
    struct IDiscordActivityManager* activities;
    struct IDiscordApplicationManager* application;
    struct IDiscordCoreEvents events;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordLobbyManager* lobbies;
    DiscordUserId user_id;
} discord_gamesdk_t;

extern discord_gamesdk_t

#endif // HAVE_DISCORDGAMESDK
#endif // __DISCORD__