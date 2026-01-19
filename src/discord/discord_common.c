// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
// Copyright (C) 2018-2025 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_common.c
/// \brief Discord integration - Shared structures and routines between RPC and Game SDK

#include "discord.h"

#include "../byteptr.h"
#include "../g_game.h"
#include "../i_system.h"
#include "../m_menu.h"
#include "../p_local.h"
#include "../p_tick.h"
#include "../z_zone.h"

#include "../netcode/d_clisrv.h"
#include "../netcode/i_net.h"
#include "../netcode/i_tcp.h" // current_port
#include "../netcode/mserv.h" // ms_RoomId

#include "../STAR/smkg-defs.h"
#include "../STAR/ss_main.h" // STAR_CONS_Printf() //
#include "../STAR/stun/stun.h"

INT32 DISCORD_RequestSFX = sfx_kc5d;

#ifdef HAVE_DISCORDSUPPORT

struct discordInfo_s discordInfo;
DISC_Request_t *discordRequestList = NULL;

#ifdef HAVE_DISCORDRPC
const char *discord_integration_type = "Discord Rich Presence";
#else
const char *discord_integration_type = "Discord Game SDK";
#endif

static char discord_self_ip[DISCORD_IP_SIZE];

// -----------------------------------
// Handlers
// -----------------------------------

/*--------------------------------------------------
	char *DISC_HideUsername(char *input)

		See header file for description.
--------------------------------------------------*/
char *DISC_HideUsername(char *input)
{
	static char buffer[5];
	int i;

	buffer[0] = input[0];

	for (i = 1; i < 4; ++i)
	{
		buffer[i] = '.';
	}

	buffer[4] = '\0';
	return buffer;
}

/*--------------------------------------------------
	char *DISC_ReturnUsername(void)

		See header file for description.
--------------------------------------------------*/
char *DISC_ReturnUsername(void)
{
	if (discordInfo.username == NULL)
	{
		return NULL;
	}
	return (cv_discordstreamer.value ? DISC_HideUsername(discordInfo.username) : discordInfo.username);
}

/*--------------------------------------------------
	char *DISC_XORIPString(const char *input)

		See header file for description.
--------------------------------------------------*/
char *DISC_XORIPString(const char *input)
{
	const UINT8 xor[DISCORD_IP_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
	char *output = malloc(sizeof(char) * (DISCORD_IP_SIZE+1));
	UINT8 i;

	for (i = 0; i < DISCORD_IP_SIZE; i++)
	{
		char xorinput;

		if (!input[i])
		{
			break;
		}

		xorinput = input[i] ^ xor[i];

		if (xorinput < 32 || xorinput > 126)
		{
			xorinput = input[i];
		}

		output[i] = xorinput;
	}

	output[i] = '\0';

	return output;
}

/*--------------------------------------------------
	const char *DISC_GetServerIP(void)

		See header file for description.
--------------------------------------------------*/
const char *DISC_GetServerIP(void)
{
	const char *address;

	// If you're connected
	if (I_GetNodeAddress)
	{
		address = I_GetNodeAddress(servernode);
		if (address != NULL && strcmp(address, "self"))
		{
			// We're not the server, so we could successfully get the IP!
			// No need to do anything else :)
			return address;
		}
	}

	if (discord_self_ip[0])
	{
		return discord_self_ip;
	}
	else
	{
		// There happens to be a good way to get it after all! :D
		STUN_bind(DISC_GotServerIP);
		return NULL;
	}
}

/*--------------------------------------------------
	void DISC_GotServerIP(UINT32 address)

		See header file for description.
--------------------------------------------------*/
void DISC_GotServerIP(UINT32 address)
{
	const unsigned char *p = (const unsigned char *)&address;
	sprintf(discord_self_ip, "%u.%u.%u.%u:%u", p[0], p[1], p[2], p[3], current_port);
	DISC_UpdatePresence();
}

/*--------------------------------------------------
	void DISC_Initialize(void)

		See header file for description.
--------------------------------------------------*/
void DISC_Initialize(void)
{
	if (discordInfo.connected == true)
	{
		DISC_UpdatePresence();
		return;
	}
	else
	{
		static boolean set_exit_func = false;

		memset(&discordInfo, 0, sizeof(discordInfo));
		discordInfo.username = NULL;

		if (cv_discordintegration.value == 0)
		{
			return;
		}

		discordInfo.initialized = DISC_HandleInit();
		if (discordInfo.initialized == false)
		{
			STAR_CONS_Printf(STAR_CONS_ERROR, "DISC_Initialize(): Failed to initialized %s!\n", discord_integration_type);
		}
		else
		{
			if (set_exit_func == false)
			{
				I_AddExitFunc(DISC_Quit);
				set_exit_func = true;
			}
			DISC_UpdatePresence();
			STAR_CONS_Printf(STAR_CONS_NOTICE, "DISC_Initialize(): Initialized %s!\n", discord_integration_type);
		}
	}
}

/*--------------------------------------------------
	void DISC_HandleConnected(const char *username, const char *discriminator, const char *userId)

		See header file for description.
--------------------------------------------------*/
void DISC_HandleConnected(const char *username, const char *discriminator, const char *userId)
{
	discordInfo.connected = true;
	discordInfo.username = malloc(314);
	snprintf(discordInfo.username, 314, "%s#%s (%s)", username, discriminator, userId);
	DISC_UpdatePresence();
	STAR_CONS_Printf(STAR_CONS_DISCORD|STAR_CONS_NOTICE|STAR_CONS_COLORWHOLELINE, "Connected to %s\n", DISC_ReturnUsername());
}

/*--------------------------------------------------
	void DISC_HandleDisconnected(int err, const char *msg)

		See header file for description.
--------------------------------------------------*/
void DISC_HandleDisconnected(int err, const char *msg)
{
	STAR_CONS_Printf(STAR_CONS_DISCORD|STAR_CONS_WARNING|STAR_CONS_COLORWHOLELINE, "Disconnected user %s (%d: %s)\n", DISC_ReturnUsername(), err, msg);
	DISC_UpdatePresence();
	free(discordInfo.username);
	discordInfo.username = NULL;
	discordInfo.connected = false;
}

/*--------------------------------------------------
	void DISC_HandleError(int err, const char *msg)

		See header file for description.
--------------------------------------------------*/
void DISC_HandleError(int err, const char *msg)
{
	STAR_CONS_Printf(STAR_CONS_DISCORD|STAR_CONS_ERROR|STAR_CONS_COLORWHOLELINE, "Discord: Encountered error (%d: %s)\n", err, msg);
}

/*--------------------------------------------------
	void DISC_HandleJoin(const char *join_secret)

		See header file for description.
--------------------------------------------------*/
void DISC_HandleJoin(const char *join_secret)
{
	char *ip = DISC_XORIPString(join_secret);
	STAR_CONS_Printf(STAR_CONS_DISCORD|STAR_CONS_NOTICE, "Joining server with IP %s...\n", ip);
	M_ClearMenus(); // Don't have menus open during connection screen
	if (demoplayback && titledemo)
	{
		// Stop the title demo, so that the connect command doesn't error if a demo is playing
		G_CheckDemoStatus();
	}
	COM_BufAddText(va("connect \"%s\"\n", ip));
	free(ip);
}

/*--------------------------------------------------
	void DISC_Quit(void)

		See header file for description.
--------------------------------------------------*/
void DISC_Quit(void)
{
	if (discordInfo.initialized == false)
	{
		CONS_Printf("DISC_Quit(): %s was never initialized!\n", discord_integration_type);
	}
	else
	{
		CONS_Printf("DISC_Quit(): Closing %s...\n", discord_integration_type);
		DISC_HandleQuit();
		free(discordInfo.username);
		memset(&discordInfo, 0, sizeof(discordInfo));
		discordInfo.username = NULL;
	}
}

/*--------------------------------------------------
	void DISC_EmptyRequests(void)

		See header file for description.
--------------------------------------------------*/
void DISC_EmptyRequests(void)
{
	while (discordRequestList != NULL)
	{
		DISC_Respond(discordRequestList->userID, DISC_REQUEST_REPLY_IGNORE);
		DISC_RemoveRequest(discordRequestList);
	}
}

// -----------------------------------
// Activity Status
// -----------------------------------

/*--------------------------------------------------
	boolean DISC_InvitesAllowed(void)

		See header file for description.
--------------------------------------------------*/
boolean DISC_InvitesAllowed(void)
{
	if (discordInfo.initialized == false || discordInfo.connected == false)
	{
		// Discord was never initialized/connected!
		return false;
	}

	if (Playing() == false)
	{
		// We're not playing, so we should not be getting invites.
		return false;
	}

	if (cv_discordasks.value == 0)
	{
		// Client has the CVar set to off, so never allow invites from this client.
		return false;
	}

	if (discordInfo.net.joinsAllowed == true)
	{
		if (discordInfo.net.everyoneCanInvite == true)
		{
			// Everyone's allowed!
			return true;
		}
		else if (consoleplayer == serverplayer || IsPlayerAdmin(consoleplayer))
		{
			// Only admins are allowed!
			return true;
		}
	}

	// Did not pass any of the checks...
	return false;
}

/*--------------------------------------------------
	void DISC_SetActivityStatus(
		char *details, char *state,
		char *image, char *imagetxt,
		char *s_image, char *s_imagetxt,
		time_t *timestamp_start, time_t *timestamp_end,
		char **clientJoinSecret,
		char **partyID, int *partySize, int *partyMax
	)

		See header file for description.
--------------------------------------------------*/
void DISC_SetActivityStatus(
	char *details, char *state,
	char *image, char *imagetxt,
	char *s_image, char *s_imagetxt,
	time_t *timestamp_start, time_t *timestamp_end,
	char **clientJoinSecret,
	char **partyID, int *partySize, int *partyMax
)
{
	memset(details, 0, sizeof(char)); memset(state, 0, sizeof(char));
	memset(image, 0, sizeof(char)); memset(imagetxt, 0, sizeof(char));
	memset(s_image, 0, sizeof(char)); memset(s_imagetxt, 0, sizeof(char));
	if ((*partyID) != NULL) free(*partyID);
	if ((*clientJoinSecret) != NULL) free(*clientJoinSecret);

#ifdef DISCORD_SECRETIVE
	// Main - Set a bare minimum status...
	// This way, we can use the invite feature in-dev, but not have snoopers seeing any potential secrets! :P
	DISC_StatusPrintf(false, details, " | ", TSOURDT3RD_SRB2_APP_FULL " - Development EXE");
	DISC_StatusPrintf(false, state, " | ", "Developing a Masterpiece!");
	DISC_StatusPrintf(true, image, "misc", "develop");
		DISC_StatusPrintf(false, imagetxt, NULL, "Keep your eyes peeled!");
	DISC_StatusPrintf(true, s_image, "map", "custom");
		DISC_StatusPrintf(false, s_imagetxt, NULL, "Hey, no peeking!");
#else
	// Main - Custom status enabled? This basically overrides everything, sorry :p
	if (cv_discordshowonstatus.value == 9)
	{
		DISC_CustomStatus(details, state, image, imagetxt, s_image, s_imagetxt);
		return;
	}
#endif

	// Netgame - Set status...
	if (netgame)
	{
		if (!(Playing() && playeringame[consoleplayer]))
			DISC_StatusPrintf(false, details, NULL, "Looking for Server");
		else
		{
			DISC_StatusPrintf(false, details, NULL, (server ? "Hosting in Room:" : "Room:"));
			switch (cv_masterserver_room_id.value)
			{
				case 33: DISC_StatusPrintf(false, details, " ", "Standard"); break;
				case 28: DISC_StatusPrintf(false, details, " ", "Casual"); break;
				case 38: DISC_StatusPrintf(false, details, " ", "Custom Gametype"); break;
				case 31: DISC_StatusPrintf(false, details, " ", "OLDC"); break;
				case 0: DISC_StatusPrintf(false, details, " ", "Public"); break;
				case -1: DISC_StatusPrintf(false, details, " ", "Private"); break;
				default: DISC_StatusPrintf(false, details, " ", "Unknown"); break;
			}

			if (DISC_InvitesAllowed() == true)
			{
				// Grab the host's IP for joining.
				const char *join = DISC_GetServerIP();
				if (join != NULL)
				{
					// Allow users to join!
					(*clientJoinSecret) = DISC_XORIPString(join);
				}
				else
				{
					// Not able to join? Flush the request list, if it exists.
					DISC_EmptyRequests();
				}
			}

			(*partyID) = server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
			(*partySize) = D_NumPlayers(); // Players in server
			(*partyMax) = discordInfo.net.maxPlayers; // Max players
		}
	}
	else
	{
		// Reset discord info if you're not in a place that uses it!
		// Important for if you join a server that compiled without HAVE_DISCORDSUPPORT,
		//  so that you don't ever end up using bad information from another server.
		memset(&discordInfo.net, 0, sizeof(discordInfo.net));
	}

#ifndef DISCORD_SECRETIVE
	// Main - Set status...
	switch (cv_discordshowonstatus.value)
	{
		case 0:
			DISC_BasicStatus(state, image, imagetxt);
			break;
		default:
			DISC_EmblemStatus(details);
			DISC_EmeraldStatus(details);
			DISC_PlayerStatus(state);
			DISC_GamestateStatus(state, image, imagetxt);
			DISC_CharacterStatus(image, imagetxt, s_image, s_imagetxt);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 2:
			DISC_CharacterStatus(image, imagetxt, s_image, s_imagetxt);
				// -- The above has the possibility to return nothing.
				// -- So, let's make sure we reflect that here as well.
				if (*imagetxt != '\0') DISC_StatusPrintf(false, details, " | ", imagetxt);
				if (*s_imagetxt != '\0') DISC_StatusPrintf(false, state, " | ", s_imagetxt);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 3:
			DISC_ScoreStatus(details);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 4:
			DISC_EmeraldStatus(details);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 5:
			DISC_EmblemStatus(details);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 6:
			DISC_GamestateStatus(details, image, imagetxt);
				DISC_StatusPrintf(false, details, NULL, imagetxt);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 7:
			DISC_PlayerStatus(details);
			DISC_BasicStatus(state, image, imagetxt);
			break;
		case 8:
			DISC_PlaytimeStatus(details);
			DISC_BasicStatus(state, image, imagetxt);
			break;
	}
#endif // DISCORD_SECRETIVE

	// Main - Set the level time...
	if (gamestate == GS_LEVEL && Playing())
	{
		const boolean tickingDown = ((countdowntimer && G_PlatformGametype()) || (gametyperules & GTR_TIMELIMIT));
		const time_t currentTime = time(NULL);
		const time_t mapTimeStart = (currentTime - (leveltime / TICRATE));

		(*timestamp_start) = mapTimeStart;

		if (timelimitintics > 0 && tickingDown)
		{
			const time_t mapTimeEnd = (mapTimeStart + ((timelimitintics + 1) / TICRATE));
			(*timestamp_end) = mapTimeEnd;
		}
	}
}

#endif // HAVE_DISCORDSUPPORT
