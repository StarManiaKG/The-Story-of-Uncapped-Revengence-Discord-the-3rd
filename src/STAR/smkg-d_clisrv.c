// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-d_clisrv.c
/// \brief TSoURDt3rd's custom client and server packets and routines

#include "ss_main.h"
#include "star_vars.h"

#include "../g_game.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_InitializePlayer(INT32 playernum)
// Initializes TSoURDt3rd's Structures For the Given Player
//
void TSoURDt3rd_InitializePlayer(INT32 playernum)
{
	TSoURDt3rd_t *TSoURDt3rd							= &TSoURDt3rdPlayers[playernum];

	TSoURDt3rd->usingTSoURDt3rd							= true;
	if (playeringame[playernum] && players[playernum].bot)
		TSoURDt3rd->checkedVersion						= true;
	else
		TSoURDt3rd->checkedVersion						= false;
	TSoURDt3rd->num										= playernum + 1;
	TSoURDt3rd->gamestate								= STAR_GS_NULL;
	TSoURDt3rd->masterServerAddressChanged				= false;

	TSoURDt3rd->loadingScreens.loadCount 				= 0;
	TSoURDt3rd->loadingScreens.loadPercentage 			= 0;
	TSoURDt3rd->loadingScreens.bspCount 				= 0;
	TSoURDt3rd->loadingScreens.screenToUse 				= 0;
	TSoURDt3rd->loadingScreens.loadComplete 			= false;

	TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd		= true;
	TSoURDt3rd->serverPlayers.majorVersion				= TSoURDt3rd_CurrentMajorVersion();
	TSoURDt3rd->serverPlayers.minorVersion				= TSoURDt3rd_CurrentMinorVersion();
	TSoURDt3rd->serverPlayers.subVersion				= TSoURDt3rd_CurrentSubversion();
	TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion	= TSoURDt3rd_CurrentVersion();

	TSoURDt3rd->jukebox.Unlocked 						= false;
}

//
// void TSoURDt3rd_ClearPlayer(INT32 playernum)
// Fully Resets the TSoURDt3rd Player Table for Both Servers and the Local Client
//
void TSoURDt3rd_ClearPlayer(INT32 playernum)
{
#if 0
	SINT8 node = (netgame ? (SINT8)doomcom->remotenode : playernum);
	SINT8 mynode = ((netbuffer->u.servercfg.clientnode < MAXPLAYERS) ? (SINT8)netbuffer->u.servercfg.clientnode : consoleplayer);
#else
	INT32 node = playernum;
	INT32 mynode = consoleplayer;
#endif

	for (INT32 i = 0; i < MAXPLAYERS; i++)
	{
		if (!TSoURDt3rdPlayers[i].num)
			continue;
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "num %d = %d\n", i, TSoURDt3rdPlayers[i].num);
	}

	if (node == mynode)
	{
#if 0		
		if (!memcmp(&TSoURDt3rdPlayers[node], &TSoURDt3rdPlayers[mynode], sizeof(TSoURDt3rd_t)))
			return;
#endif

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "before: tsourdt3rdnode - %d\n", TSoURDt3rdPlayers[node].num);

		TSoURDt3rdPlayers[mynode] = TSoURDt3rdPlayers[node];
		M_Memcpy(&TSoURDt3rdPlayers[mynode], &TSoURDt3rdPlayers[node], sizeof(TSoURDt3rd_t));

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "after: tsourdt3rdnode - %d\n", TSoURDt3rdPlayers[mynode].num);
		return;
	}

	if ((!netgame) || (netgame && !playeringame[node]))
		memset(&TSoURDt3rdPlayers[node], 0, sizeof(TSoURDt3rd_t));
}

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode)
{
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "ADDING USER: node - %d, consoleplayer - %d\n", node, consoleplayer);

	if (node == prevnode)
	{
		TSoURDt3rdPlayers[consoleplayer] = TSoURDt3rdPlayers[0];
		M_Memcpy(&TSoURDt3rdPlayers[consoleplayer], &TSoURDt3rdPlayers[0], sizeof(TSoURDt3rd_t));

#if 0
		// STAR NOTE: MAJOR STAR TODO NOTE: come back here later after getting better internet doofus //
		TSoURDt3rd_ClearPlayer(consoleplayer);
		memset(&TSoURDt3rdPlayers[0], 0, sizeof(TSoURDt3rd_t));
#endif
	}

	if (node != prevnode)
	{
		if (server)
		{
			if (TSoURDt3rdPlayers[newplayernode].usingTSoURDt3rd)
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Joining player is using TSoURDt3rd!\n");
			else
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Joining player doesn't seem to be using TSoURDt3rd!\nPlease be cautious of what you do!\n");
		}
		S_StartSound(NULL, STAR_JoinSFX);
	}
}

void TSoURDt3rd_HandleCustomPackets(INT32 node)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[node];

#ifdef HAVE_DISCORDSUPPORT
	if (TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd)
	{
		discordInfo.serv.maxPlayers = netbuffer->u.servercfg.maxplayer;
		discordInfo.serv.joinsAllowed = netbuffer->u.servercfg.allownewplayer;
		discordInfo.serv.everyoneCanInvite = netbuffer->u.servercfg.discordinvites;
	}
	else
	{
		discordInfo.serv.maxPlayers = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
		discordInfo.serv.joinsAllowed = cv_allownewplayer.value;
		discordInfo.serv.everyoneCanInvite = (boolean)cv_discordinvites.value;
	}
#endif

	TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd = (netbuffer->u.servercfg.tsourdt3rd != 1 ? 0 : 1);

	TSoURDt3rd->serverPlayers.majorVersion = (TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd ? netbuffer->u.servercfg.tsourdt3rdmajorversion : 0);
	TSoURDt3rd->serverPlayers.minorVersion = (TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd ? netbuffer->u.servercfg.tsourdt3rdminorversion : 0);
	TSoURDt3rd->serverPlayers.subVersion = (TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd ? netbuffer->u.servercfg.tsourdt3rdsubversion : 0);

	TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion = STAR_CombineNumbers(3, TSoURDt3rd->serverPlayers.majorVersion, TSoURDt3rd->serverPlayers.minorVersion, TSoURDt3rd->serverPlayers.subVersion);

	if (client)
	{
		if (TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd)
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Server uses TSoURDt3rd, running features!\n");
		else
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Can't find working serverside TSoURDt3rd! Proceeding to work around this...\n");
	}
}
