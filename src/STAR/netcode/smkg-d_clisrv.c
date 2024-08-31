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

#include <time.h>

#include "smkg-net.h"

#include "../smkg-cvars.h"
#include "../star_vars.h"
#include "../smkg-jukebox.h"

#include "../monocypher/monocypher.h"
#include "../monocypher/smkg-hasher.h"

#include "../../g_game.h"
#include "../../z_zone.h"

#include "../../discord/stun.h"
#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

/// \brief hole punching packet, also points inside doomcom
/* See ../doc/Holepunch-Protocol.txt */
holepunch_t *holepunchpacket = NULL;

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
void TSoURDt3rd_InitializePlayer(INT32 playernum)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];

	{
		static UINT8 seed[32];

		csprng(seed, 32);
		memset(tsourdt3rd_user->secret_key, 0, sizeof(tsourdt3rd_user->secret_key));
		memset(tsourdt3rd_user->public_key, 0, sizeof(tsourdt3rd_user->public_key));

		crypto_eddsa_key_pair
		(
			tsourdt3rd_user->secret_key,
			tsourdt3rd_user->public_key,
			seed
		);
		strcpy(tsourdt3rd_user->user_hash, TSoURDt3rd_Hash_GenerateFromID(tsourdt3rd_user->secret_key, true));
	}

	tsourdt3rd_user->usingTSoURDt3rd						= true;
	tsourdt3rd_user->checkedVersion							= (playeringame[playernum] && players[playernum].bot);
	tsourdt3rd_user->masterServerAddressChanged				= false;

	tsourdt3rd_user->loadingScreens.loadCount 				= 0;
	tsourdt3rd_user->loadingScreens.loadPercentage 			= 0;
	tsourdt3rd_user->loadingScreens.bspCount 				= 0;
	tsourdt3rd_user->loadingScreens.screenToUse 			= 0;
	tsourdt3rd_user->loadingScreens.loadComplete 			= false;

	tsourdt3rd_user->serverPlayers.serverUsesTSoURDt3rd		= true;
	tsourdt3rd_user->serverPlayers.majorVersion				= TSoURDt3rd_CurrentMajorVersion();
	tsourdt3rd_user->serverPlayers.minorVersion				= TSoURDt3rd_CurrentMinorVersion();
	tsourdt3rd_user->serverPlayers.subVersion				= TSoURDt3rd_CurrentSubversion();
	tsourdt3rd_user->serverPlayers.serverTSoURDt3rdVersion	= TSoURDt3rd_CurrentVersion();

	{
		static tsourdt3rd_jukeboxdef_t jukebox_soundtestsfx =
		{
			&soundtestsfx,
			0,
			NULL
		};
		static UINT8 jukebox_frame_list[4] = {0, 0, -1, SKINCOLOR_RUBY};

		tsourdt3rd_global_jukebox = Z_Malloc(sizeof(tsourdt3rd_jukebox_t), PU_STATIC, NULL);
		tsourdt3rd_jukebox_available_pages = Z_Malloc(64 * sizeof(tsourdt3rd_jukebox_pages_t *), PU_STATIC, NULL);

		tsourdt3rd_global_jukebox->Unlocked = false;
		tsourdt3rd_global_jukebox->playing = false;
		tsourdt3rd_global_jukebox->initHUD = false;

		tsourdt3rd_global_jukebox->jukebox_selection = 0;
		tsourdt3rd_global_jukebox->jukebox_tics = 0;

		tsourdt3rd_global_jukebox->jukebox_frames = jukebox_frame_list;
		tsourdt3rd_global_jukebox->jukebox_hscale = FRACUNIT/2;
		tsourdt3rd_global_jukebox->jukebox_vscale = FRACUNIT/2;
		tsourdt3rd_global_jukebox->jukebox_bouncing = 0;

		tsourdt3rd_global_jukebox->curtrack = NULL;
		tsourdt3rd_global_jukebox->prevtrack = NULL;
	}
}

//
// void TSoURDt3rd_ClearPlayer(INT32 playernum)
// Fully Resets the TSoURDt3rd Player Table for Both Servers and the Local Client
//
void TSoURDt3rd_ClearPlayer(INT32 playernum)
{
	TSoURDt3rd_t *tsourdt3rd_prevuser = &TSoURDt3rdPlayers[playernum];
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];

	if (!playernum && !consoleplayer)
		return;
	else if (!tsourdt3rd_prevuser || *tsourdt3rd_prevuser->user_hash == '\0')
		return;

	if (playernum == consoleplayer)
	{
		tsourdt3rd_user = tsourdt3rd_prevuser;
		return;
	}

	if (!playeringame[playernum])
		memset(tsourdt3rd_prevuser, 0, sizeof(&tsourdt3rd_prevuser));
}

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];

	if (node == prevnode)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "moving player structure %d to %d\n", prevnode, consoleplayer);
		tsourdt3rd_user = &TSoURDt3rdPlayers[0];
		memcpy(tsourdt3rd_user, &TSoURDt3rdPlayers[0], sizeof(TSoURDt3rd_t));

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

void TSoURDt3rd_D_AskForHolepunch(INT32 node)
{
	if (node != 0 && node != BROADCASTADDR &&
		cv_rendezvousserver.string[0])
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
