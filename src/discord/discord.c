// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord.c
/// \brief Main Discord Rich Presence handling

#ifdef HAVE_DISCORDRPC

#include <time.h>
#include "discord.h"
#include "discord_cmds.h"
#include "stun.h"
#include "../i_system.h"
#include "../d_clisrv.h"
#include "../d_netcmd.h"
#include "../i_net.h"
#include "../g_game.h"
#include "../mserv.h" // msServerId
#include "../z_zone.h"
#include "../byteptr.h"
#include "../i_tcp.h" // current_port
#include "../STAR/star_vars.h" // DISCORD_RequestSFX //

// ------------------------ //
//        Variables
// ------------------------ //

struct discordInfo_s discordInfo;

discordRequest_t *discordRequestList = NULL;

static char self_ip[IP_SIZE];

static char discord_username[128];

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	static char *DRPC_HideUsername(char *input);

		Handle usernames while cv_discordstreamer is activated.
		(The loss of discriminators is still a dumbass regression
		that I will never forgive the Discord developers for.)
--------------------------------------------------*/
static char *DRPC_HideUsername(char *input)
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
	char *DRPC_ReturnUsername(void);

		Returns the Discord username of the user.
		Properly accomdiates for streamer mode.
	
	Input Arguments:-
		None

	Return:-
		Discord Username String
--------------------------------------------------*/
char *DRPC_ReturnUsername(void)
{
	if (cv_discordstreamer.value)
		return DRPC_HideUsername(discord_username);
	return discord_username;
}

/*--------------------------------------------------
	static char *DRPC_XORIPString(const char *input)

		Simple XOR encryption/decryption. Not complex or
		very secretive because we aren't sending anything
		that isn't easily accessible via our Master Server anyway.

	Input Arguments:-
		input - Struct that will be used to make an XOR IP String.
	
	Return:-
		XOR IP String
--------------------------------------------------*/
static char *DRPC_XORIPString(const char *input)
{
	const UINT8 xor[IP_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
	char *output = malloc(sizeof(char) * (IP_SIZE+1));
	UINT8 i;

	for (i = 0; i < IP_SIZE; i++)
	{
		char xorinput;

		if (!input[i])
			break;

		xorinput = input[i] ^ xor[i];

		if (xorinput < 32 || xorinput > 126)
			xorinput = input[i];

		output[i] = xorinput;
	}

	output[i] = '\0';

	return output;
}

/*--------------------------------------------------
	static void DRPC_HandleReady(const DiscordUser *user)

		Callback function, ran when the game connects to Discord.

	Input Arguments:-
		user - Struct containing Discord user info.

	Return:-
		None
--------------------------------------------------*/
static void DRPC_HandleReady(const DiscordUser *user)
{
	snprintf(discord_username, 128, "%s#%s (%s)", user->username, user->discriminator, user->userId);
	CONS_Printf("Discord: connected to %s\n", DRPC_ReturnUsername());
	discordInfo.ConnectionStatus = DRPC_CONNECTED;
}

/*--------------------------------------------------
	static void DRPC_HandleDisconnect(int err, const char *msg)

		Callback function, ran when disconnecting from Discord.

	Input Arguments:-
		err - Error type
		msg - Error message

	Return:-
		None
--------------------------------------------------*/
static void DRPC_HandleDisconnect(int err, const char *msg)
{
	CONS_Printf("Discord: disconnected (%d: %s)\n", err, msg);
	discordInfo.ConnectionStatus = DRPC_DISCONNECTED;
}

/*--------------------------------------------------
	static void DRPC_HandleError(int err, const char *msg)

		Callback function, ran when Discord outputs an error.

	Input Arguments:-
		err - Error type
		msg - Error message

	Return:-
		None
--------------------------------------------------*/
static void DRPC_HandleError(int err, const char *msg)
{
	CONS_Alert(CONS_WARNING, "Discord error (%d: %s)\n", err, msg);
}

/*--------------------------------------------------
	static void DRPC_HandleJoin(const char *secret)

		Callback function, ran when Discord wants to
		connect a player to the game via a channel invite
		or a join request.

	Input Arguments:-
		secret - Value that links you to the server.

	Return:-
		None
--------------------------------------------------*/
static void DRPC_HandleJoin(const char *secret)
{
	char *ip = DRPC_XORIPString(secret);
	CONS_Printf("Connecting to %s via Discord\n", ip);
	M_ClearMenus(true); // Don't have menus open during connection screen
	if (demoplayback && titledemo)
		G_CheckDemoStatus(); // Stop the title demo, so that the connect command doesn't error if a demo is playing
	COM_BufAddText(va("connect \"%s\"\n", ip));
	free(ip);
}

/*--------------------------------------------------
	static boolean DRPC_InvitesAreAllowed(void)

		Determines whenever or not invites or
		ask to join requests are allowed.

	Input Arguments:-
		None

	Return:-
		true if invites are allowed, false otherwise.
--------------------------------------------------*/
static boolean DRPC_InvitesAreAllowed(void)
{
	if (!Playing())
	{
		// We're not playing, so we should not be getting invites.
		return false;
	}

	if (cv_discordasks.value == 0)
	{
		// Client has the CVar set to off, so never allow invites from this client.
		return false;
	}

	if (discordInfo.serv.joinsAllowed == true)
	{
		if (discordInfo.serv.everyoneCanInvite == true)
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

	// Did not pass any of the checks
	return false;
}

/*--------------------------------------------------
	static void DRPC_HandleJoinRequest(const DiscordUser *requestUser)

		Callback function, ran when Discord wants to
		ask the player if another Discord user can join
		or not.

	Input Arguments:-
		requestUser - DiscordUser struct for the user trying to connect.

	Return:-
		nil if an error occured or duplicate requests were found, or none otherwise
--------------------------------------------------*/
static void DRPC_HandleJoinRequest(const DiscordUser *requestUser)
{
	discordRequest_t *append = discordRequestList;
	discordRequest_t *newRequest;

	if (DRPC_InvitesAreAllowed()  == false)
	{
		// Something weird happened if this occurred...
		Discord_Respond(requestUser->userId, DISCORD_REPLY_IGNORE);
		return;
	}

	newRequest = Z_Calloc(sizeof(discordRequest_t), PU_STATIC, NULL);

	newRequest->username = Z_Calloc(344, PU_STATIC, NULL);
	snprintf(newRequest->username, 344, "%s", requestUser->username);

#ifdef DISCORD_DISCRIMINATORS
	newRequest->discriminator = Z_Calloc(8, PU_STATIC, NULL);
	snprintf(newRequest->discriminator, 8, "%s", requestUser->discriminator);
#endif

	newRequest->userID = Z_Calloc(32, PU_STATIC, NULL);
	snprintf(newRequest->userID, 32, "%s", requestUser->userId);

	if (append != NULL)
	{
		discordRequest_t *prev = NULL;

		while (append != NULL)
		{
			// CHECK FOR DUPES!! Ignore any that already exist from the same user.
			if (!strcmp(newRequest->userID, append->userID))
			{
				Discord_Respond(newRequest->userID, DISCORD_REPLY_IGNORE);
				DRPC_RemoveRequest(newRequest);
				return;
			}

			prev = append;
			append = append->next;
		}

		newRequest->prev = prev;
		prev->next = newRequest;
	}
	else
	{
		discordRequestList = newRequest;
		//M_RefreshPauseMenu();
	}

	// Made it to the end, request was valid, so play the request sound :)
	S_StartSound(NULL, DISCORD_RequestSFX);
}

/*--------------------------------------------------
	void DRPC_RemoveRequest(discordRequest_t *removeRequest)

		See header file for description.
	
	Input Arguments:-
		None
	
	Return:-
		None
--------------------------------------------------*/
void DRPC_RemoveRequest(discordRequest_t *removeRequest)
{
	if (removeRequest->prev != NULL)
	{
		removeRequest->prev->next = removeRequest->next;
	}

	if (removeRequest->next != NULL)
	{
		removeRequest->next->prev = removeRequest->prev;

		if (removeRequest == discordRequestList)
		{
			discordRequestList = removeRequest->next;
		}
	}
	else
	{
		if (removeRequest == discordRequestList)
		{
			discordRequestList = NULL;
		}
	}

	Z_Free(removeRequest->username);
#ifdef DISCORD_DISCRIMINATORS
	Z_Free(removeRequest->discriminator);
#endif
	Z_Free(removeRequest->userID);
	Z_Free(removeRequest);
}

/*--------------------------------------------------
	void DRPC_Init(void)

		See header file for description.
--------------------------------------------------*/
void DRPC_Init(void)
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));

	discordInfo.ConnectionStatus = DRPC_INITIALIZED;

	CONS_Printf("DRPC_Init(): Initalizing Discord Rich Presence...\n");

	handlers.ready = DRPC_HandleReady;
	handlers.disconnected = DRPC_HandleDisconnect;
	handlers.errored = DRPC_HandleError;
	handlers.joinGame = DRPC_HandleJoin;
	handlers.joinRequest = DRPC_HandleJoinRequest;

	Discord_Initialize(DISCORD_APPID, &handlers, 1, NULL);
	I_AddExitFunc(Discord_Shutdown);
	DRPC_UpdatePresence();
}

/*--------------------------------------------------
	static void DRPC_GotServerIP(UINT32 address)

		Callback triggered by successful STUN response.

	Input Arguments:-
		address - IPv4 address of this machine, in network byte order.

	Return:-
		None
--------------------------------------------------*/
static void DRPC_GotServerIP(UINT32 address)
{
	const unsigned char * p = (const unsigned char *)&address;
	sprintf(self_ip, "%u.%u.%u.%u:%u", p[0], p[1], p[2], p[3], current_port);
	DRPC_UpdatePresence();
}

/*--------------------------------------------------
	static const char *DRPC_GetServerIP(void)

		Retrieves the IP address of the server that you're
		connected to. Will attempt to use curl for getting your
		own IP address, if it's not yours.
--------------------------------------------------*/
static const char *DRPC_GetServerIP(void)
{
	const char *address; 

	// If you're connected
	if (I_GetNodeAddress && (address = I_GetNodeAddress(servernode)) != NULL)
	{
		if (strcmp(address, "self"))
		{
			// We're not the server, so we could successfully get the IP!
			// No need to do anything else :)
			return address;
		}
	}

	if (self_ip[0])
	{
		return self_ip;
	}
	else
	{
		// There happens to be a good way to get it after all! :D
		STUN_bind(DRPC_GotServerIP);
		return NULL;
	}
}

/*--------------------------------------------------
	static void DRPC_EmptyRequests(void)

		Empties the request list. Any existing requests
		will get an ignore reply.
--------------------------------------------------*/
static void DRPC_EmptyRequests(void)
{
	while (discordRequestList != NULL)
	{
		Discord_Respond(discordRequestList->userID, DISCORD_REPLY_IGNORE);
		DRPC_RemoveRequest(discordRequestList);
	}
}

/*--------------------------------------------------
	void DRPC_UpdatePresence(void)

		See header file for description.
--------------------------------------------------*/
void DRPC_UpdatePresence(void)
{
	boolean joinSecretSet = false;
	char *clientJoinSecret = NULL;

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	char detailstr[128] = "";
	char statestr[128] = "";

	char imagestr[64] = "";
	char imagetxtstr[128] = "";

	char s_imagestr[64] = "";
	char s_imagetxtstr[128] = "";

	if (dedicated)
	{
		return;
	}

	if (discordInfo.ConnectionStatus != DRPC_CONNECTED)
	{
		// Discord isn't connected, so why bother?
		return;
	}

	// Statuses //
	// Discord RPC off
	if (!cv_discordrp.value)
	{
		// User doesn't want to show their game information, so update with empty presence.
		// This just shows that they're playing SRB2Kart. (If that's too much, then they should disable game activity :V)
		DRPC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	// Servers
	if (netgame)
	{
		if (DRPC_InvitesAreAllowed() == true)
		{
			const char *join;

			// Grab the host's IP for joining.
			if ((join = DRPC_GetServerIP()) != NULL)
			{
#ifdef DISCORD_SECRETIVE
				clientJoinSecret = DRPC_XORIPString(join);
				discordPresence.joinSecret = clientJoinSecret;
				joinSecretSet = true;
#else
				discordPresence.joinSecret = DRPC_XORIPString(join);
				joinSecretSet = true;
#endif
			}
			else
			{
				return;
			}
		}

#ifdef DISCORD_SECRETIVE
		{
#else
		discordPresence.partyId	= server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!

		if (cv_discordshowonstatus.value != 9) // Custom statuses don't get this, thanks
		{
			if (Playing() && !playeringame[consoleplayer])
				DRPC_StringPrintf(detailstr, NULL, 128, "Looking for Server");
			else
			{
				if (server)
					DRPC_StringPrintf(detailstr, NULL, 128, "Hosting ");
				switch (ms_RoomId)
				{
					case 33: DRPC_StringPrintf(detailstr, NULL, 128, "Standard"); break;
					case 28: DRPC_StringPrintf(detailstr, NULL, 128, "Casual"); break;
					case 38: DRPC_StringPrintf(detailstr, NULL, 128, "Custom Gametype"); break;
					case 31: DRPC_StringPrintf(detailstr, NULL, 128, "OLDC"); break;

					case 0: DRPC_StringPrintf(detailstr, NULL, 128, "Public"); break;
					case -1: DRPC_StringPrintf(detailstr, NULL, 128, "Private"); break;

					default: DRPC_StringPrintf(detailstr, NULL, 128, "Unknown"); break;
				}
			}
#endif

			discordPresence.partySize = D_NumPlayers(); // Players in server
			discordPresence.partyMax = discordInfo.serv.maxPlayers; // Max players
		}
	}
	else
		memset(&discordInfo.serv, 0, sizeof(discordInfo.serv));

#ifdef DISCORD_SECRETIVE
	// Minimum Status
	DRPC_StringPrintf(detailstr, " | ", 128, "Developing a Masterpiece");
	DRPC_StringPrintf(statestr, " | ", 128, "Keep your Eyes Peeled!");
	DRPC_ImagePrintf(imagestr, 128, "map", "custom");
	DRPC_StringPrintf(imagetxtstr, NULL, 128, "Hey! No Peeking!");
#else
	// Main
	switch (cv_discordshowonstatus.value)
	{
		case 1:
			DRPC_EmblemStatus(detailstr);
			DRPC_EmeraldStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			DRPC_ExtendedStatus(statestr);
			DRPC_GamestateStatus(statestr, imagestr, imagetxtstr);
			DRPC_CharacterStatus(imagestr, imagetxtstr, s_imagestr, s_imagetxtstr);
			break;

		case 2:
			DRPC_CharacterStatus(imagestr, imagetxtstr, s_imagestr, s_imagetxtstr);
			if (*imagetxtstr != '\0')
				DRPC_StringPrintf(detailstr, NULL, 128, imagetxtstr);
			if (*s_imagetxtstr != '\0')
				DRPC_StringPrintf(statestr, NULL, 128, s_imagetxtstr);
			DRPC_GeneralStatus(statestr, (*imagestr == '\0' ? imagestr : NULL), (*imagestr == '\0' ? imagetxtstr : NULL));
			break;

		case 3:
			DRPC_ScoreStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 4:
			DRPC_EmeraldStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 5:
			DRPC_EmblemStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 6:
			DRPC_GamestateStatus(detailstr, imagestr, imagetxtstr);
			DRPC_StringPrintf(detailstr, NULL, 128, imagetxtstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 7:
			DRPC_ExtendedStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 8:
			DRPC_PlaytimeStatus(detailstr);
			DRPC_GeneralStatus(statestr, imagestr, imagetxtstr);
			break;

		case 9:
			DRPC_CustomStatus(detailstr, statestr, imagestr, imagetxtstr, s_imagestr, s_imagetxtstr);
			break;

		default: // Basic
			DRPC_StringPrintf(detailstr, NULL, 128, "In-Game");
			if (paused)
				DRPC_StringPrintf(statestr, NULL, 128, "Paused");
			else if (menuactive || !Playing())
				DRPC_StringPrintf(statestr, NULL, 128, "In a Menu");
			else
				DRPC_StringPrintf(statestr, NULL, 128, "Active");
			DRPC_StringPrintf(imagestr, NULL, 128, "misctitle");
			DRPC_StringPrintf(imagetxtstr, NULL, 128, "Sonic Robo Blast 2");
			break;
	}

	// Level Time //
	if (gamestate == GS_LEVEL && Playing())
	{
		const time_t currentTime = time(NULL);
		const time_t mapTimeStart = currentTime - (leveltime / TICRATE);

		discordPresence.startTimestamp = mapTimeStart;

		if (cv_timelimit.value && timelimitintics > 0 && (gametyperules & GTR_TIMELIMIT))
		{
			const time_t mapTimeEnd = mapTimeStart + ((timelimitintics + TICRATE) / TICRATE);
			discordPresence.endTimestamp = mapTimeEnd;
		}
	}
#endif

	// We can finally push our new status! :) //
	discordPresence.details = detailstr;
	discordPresence.state = statestr;

	discordPresence.smallImageKey = s_imagestr;
	discordPresence.smallImageText = s_imagetxtstr;

	discordPresence.largeImageKey = imagestr;
	discordPresence.largeImageText = imagetxtstr;

	if (joinSecretSet == false)
	{
		// Not able to join? Flush the request list, if it exists.
		DRPC_EmptyRequests();
	}
	Discord_UpdatePresence(&discordPresence);

	free(clientJoinSecret);
}

#endif // HAVE_DISCORDRPC
