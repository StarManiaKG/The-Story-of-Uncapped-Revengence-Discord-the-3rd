// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-d_clisrv.c
/// \brief TSoURDt3rd's custom client and server packets and routines

#include <time.h>

#if defined (__linux__) || defined (__FreeBSD__)
#include <sys/random.h>
#elif defined (_WIN32)
#define _CRT_RAND_S
#elif defined (__APPLE__)
#include <CommonCrypto/CommonRandom.h>
#endif

#include "smkg-net.h"

#include "../smkg-cvars.h"
#include "../star_vars.h"
#include "../core/smkg-s_jukebox.h"
#include "../monocypher/smkg-m_hash.h"

#include "../../g_game.h"
#include "../../z_zone.h"

#include "../../netcode/server_connection.h"

#ifdef USE_STUN
#include "../stun/stun.h"
#endif

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

/// \brief hole punching packet, also points inside doomcom
/* See ../doc/Holepunch-Protocol.txt */
holepunch_t *holepunchpacket = NULL;

INT32 STAR_JoinSFX = sfx_kc48;
INT32 STAR_LeaveSFX = sfx_kc52;
INT32 STAR_SynchFailureSFX = sfx_kc46;
INT32 DISCORD_RequestSFX = sfx_kc5d;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_D_CheckNetgame(doomcom_t *doomcom_p)
// Runs an extended amount of net participant data for D_CheckNetgame in d_net.c.
//
void TSoURDt3rd_D_CheckNetgame(doomcom_t *doomcom_p)
{
	holepunchpacket = (holepunch_t *)(void *)&doomcom_p->data;
}

//
// void TSoURDt3rd_InitializePlayer(INT32 playernum)
// Initializes TSoURDt3rd's Structures For the Given Player
//
static void D_Hash_Seed(UINT8 *seed)
{
	size_t size = 32;
#ifdef USE_STUN
	csprng(seed, size);
#else
#if defined (_WIN32)
	for (size_t o = 0; o < size; o += sizeof(unsigned int))
		rand_s((unsigned int *)&((char *)seed)[o]);
#elif defined (__linux__)
	getrandom(seed, size, 0U);
#elif defined (__APPLE__)
	CCRandomGenerateBytes(seed, size);
#elif defined (__FreeBSD__) || defined (__NetBSD__) || defined (__OpenBSD__)
	arc4random_buf(seed, size);
#endif
#endif
}

void TSoURDt3rd_InitializePlayer(INT32 playernum)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[playernum];
	tsourdt3rd_t *tsourdt3rd_struct_user = &tsourdt3rd[playernum];
	static UINT8 seed[32];
	D_Hash_Seed(seed);
	{
		memset(tsourdt3rd_user->secret_key, 0, sizeof(tsourdt3rd_user->secret_key));
		memset(tsourdt3rd_user->public_key, 0, sizeof(tsourdt3rd_user->public_key));
		crypto_eddsa_key_pair(tsourdt3rd_user->secret_key, tsourdt3rd_user->public_key, seed);
		strcpy(tsourdt3rd_user->user_hash, TSoURDt3rd_Hash_GenerateFromID(tsourdt3rd_user->secret_key, true));
		tsourdt3rd_user->usingTSoURDt3rd			= true;
		tsourdt3rd_user->server_usingTSoURDt3rd		= true;
		tsourdt3rd_user->server_majorVersion		= TSoURDt3rd_CurrentMajorVersion();
		tsourdt3rd_user->server_minorVersion		= TSoURDt3rd_CurrentMinorVersion();
		tsourdt3rd_user->server_subVersion			= TSoURDt3rd_CurrentSubversion();
		tsourdt3rd_user->server_TSoURDt3rdVersion	= TSoURDt3rd_CurrentVersion();
	}
	{
		tsourdt3rd_struct_user->game.time_over = false;
	}
	{
		tsourdt3rd_loadingscreen.loadCount      = 0;
		tsourdt3rd_loadingscreen.loadPercentage = 0;
		tsourdt3rd_loadingscreen.bspCount       = 0;
		tsourdt3rd_loadingscreen.screenToUse    = 0;
		tsourdt3rd_loadingscreen.loadComplete   = false;
	}
}

//
// void TSoURDt3rd_ClearPlayer(INT32 playernum)
// Fully Resets the TSoURDt3rd Player Table for Both Servers and the Local Client
//
void TSoURDt3rd_ClearPlayer(INT32 playernum)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];
	TSoURDt3rd_t *tsourdt3rd_prevuser = &TSoURDt3rdPlayers[playernum];

	tsourdt3rd_t *tsourdt3rd_struct_user = &tsourdt3rd[consoleplayer];
	tsourdt3rd_t *tsourdt3rd_struct_prevuser = &tsourdt3rd[playernum];

	if (!playernum && !consoleplayer)
		return;
	if (!tsourdt3rd_prevuser || *tsourdt3rd_prevuser->user_hash == '\0')
		return;

	if (playernum == consoleplayer)
	{
		memcpy(tsourdt3rd_user, tsourdt3rd_prevuser, sizeof(&tsourdt3rd_prevuser));
		memcpy(tsourdt3rd_struct_user, tsourdt3rd_struct_prevuser, sizeof(&tsourdt3rd_struct_prevuser));
		return;
	}

	if (!playeringame[playernum])
	{
		memset(tsourdt3rd_prevuser, 0, sizeof(&tsourdt3rd_prevuser));
		memset(tsourdt3rd_struct_prevuser, 0, sizeof(&tsourdt3rd_struct_prevuser));
	}
}

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];
	tsourdt3rd_t *tsourdt3rd_struct_user = &tsourdt3rd[consoleplayer];

	(void)newplayernode;

	if (node == prevnode)
	{
		STAR_CONS_Printf(STAR_CONS_DEBUG, "moving player structure %d to %d\n", prevnode, consoleplayer);
		tsourdt3rd_user = &TSoURDt3rdPlayers[0];
		tsourdt3rd_struct_user = &tsourdt3rd_struct_user[0];
		memcpy(tsourdt3rd_user, &TSoURDt3rdPlayers[0], sizeof(TSoURDt3rd_t));
		memcpy(tsourdt3rd_struct_user, &tsourdt3rd[0], sizeof(tsourdt3rd_t));
	}

	if (node != prevnode)
	{
		if (server)
		{
			if (netbuffer->u.clientcfg.tsourdt3rd)
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE, "Joining player is using TSoURDt3rd!\n");
			else
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE, "Joining player doesn't seem to be using TSoURDt3rd!\nPlease be cautious of what you do!\n");
		}
		S_StartSound(NULL, STAR_JoinSFX);
	}
}

void TSoURDt3rd_HandleCustomPackets(INT32 node)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[node];
	const char *server_text = NULL;

#ifdef HAVE_DISCORDSUPPORT
	if (tsourdt3rd_user->server_usingTSoURDt3rd)
	{
		discordInfo.net.maxPlayers = netbuffer->u.servercfg.maxplayer;
		discordInfo.net.joinsAllowed = netbuffer->u.servercfg.allownewplayer;
		discordInfo.net.everyoneCanInvite = netbuffer->u.servercfg.discord_invites;
	}
	else
	{
		discordInfo.net.maxPlayers = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
		discordInfo.net.joinsAllowed = cv_allownewplayer.value;
		discordInfo.net.everyoneCanInvite = (boolean)cv_discordinvites.value;
	}
#endif

	tsourdt3rd_user->server_usingTSoURDt3rd = (netbuffer->u.servercfg.tsourdt3rd != 1 ? 0 : 1);
	if (tsourdt3rd_user->server_usingTSoURDt3rd)
	{
		server_text = "Server uses TSoURDt3rd, running features!\n";
		tsourdt3rd_user->server_majorVersion = netbuffer->u.servercfg.tsourdt3rd_majorversion;
		tsourdt3rd_user->server_minorVersion = netbuffer->u.servercfg.tsourdt3rd_minorversion;
		tsourdt3rd_user->server_subVersion = netbuffer->u.servercfg.tsourdt3rd_subversion;
		tsourdt3rd_user->server_TSoURDt3rdVersion = netbuffer->u.servercfg.tsourdt3rd_fullversion;
	}
	else
		server_text = "Can't find working serverside TSoURDt3rd! Proceeding to work around this...\n";

	if (client)
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE, server_text);
}

void TSoURDt3rd_D_AskForHolepunch(INT32 node)
{
	if (node != 0 && node != BROADCASTADDR &&
		cv_tsourdt3rd_servers_holepunchrendezvous.string[0])
	{
		I_NetRequestHolePunch(node);
	}
}

//
// void TSoURDt3rd_D_RenewHolePunch(void)
// Renews the holepunch packet data.
//
void TSoURDt3rd_D_RenewHolePunch(void)
{
	static time_t past;
	const time_t now = time(NULL);

	if (netgame && serverrunning)
	{
		if ((now - past) > 20)
		{
			I_NetRegisterHolePunch();
			past = now;
		}
	}
}
