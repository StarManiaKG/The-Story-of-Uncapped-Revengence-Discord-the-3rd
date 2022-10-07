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

#ifdef HAVE_DISCORDRPC

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

extern consvar_t cv_discordrp;
extern consvar_t cv_discordstreamer;
extern consvar_t cv_discordasks;
extern consvar_t cv_discordshowonstatus;
extern consvar_t cv_customdiscorddetails;
extern consvar_t cv_customdiscordstate;
extern consvar_t cv_discordstatusmemes;

typedef struct srb2gsdk_s {
    struct IDiscordCore* core;
    struct IDiscordUserManager* users;
    struct IDiscordAchievementManager* achievements;
    struct IDiscordActivityManager* activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager* application;
    struct IDiscordLobbyManager* lobbies;
    DiscordUserId user_id;
} srb2gsdk_t;

extern srb2gsdk_t *discordRequestList;

#endif // HAVE_DISCORDRPC
#endif // __DISCORD__