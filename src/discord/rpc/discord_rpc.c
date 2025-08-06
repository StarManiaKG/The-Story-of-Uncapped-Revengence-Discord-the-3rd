// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_rpc.c
/// \brief Discord Rich Presence handling and routines

#ifdef HAVE_DISCORDRPC

#include <discord_rpc.h>

#include "../discord.h"

#include "../../m_menu.h"
#include "../../z_zone.h"

#include "../../STAR/star_vars.h" // DISCORD_RequestSFX //

// ------------------------ //
//        Functions
// ------------------------ //

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
	DISC_HandleConnected(user->username, user->discriminator, user->userId);
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
	DISC_HandleDisconnected(err, msg);
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
	DISC_HandleError(err, msg);
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
	DISC_HandleJoining(secret);
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
	DISC_Request_t *append = discordRequestList;
	DISC_Request_t *newRequest;

	if (DISC_InvitesAllowed() == false)
	{
		// Something weird happened if this occurred...
		DISC_Respond(requestUser->userId, DISC_REQUEST_REPLY_IGNORE);
		return;
	}

	newRequest = Z_Calloc(sizeof(DISC_Request_t), PU_STATIC, NULL);

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
		DISC_Request_t *prev = NULL;

		while (append != NULL)
		{
			// CHECK FOR DUPES!! Ignore any that already exist from the same user.
			if (!strcmp(newRequest->userID, append->userID))
			{
				DISC_Respond(newRequest->userID, DISC_REQUEST_REPLY_IGNORE);
				DISC_RemoveRequest(newRequest);
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
	void DISC_Init(void)

		See header file for description.
--------------------------------------------------*/
void DISC_Init(void)
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));

	handlers.ready = DRPC_HandleReady;
	handlers.disconnected = DRPC_HandleDisconnect;
	handlers.errored = DRPC_HandleError;
	handlers.joinGame = DRPC_HandleJoin;
	handlers.joinRequest = DRPC_HandleJoinRequest;

	Discord_Initialize(DISCORD_APPID_STRING, &handlers, 1, NULL);
	DISC_HandleInitializing("Discord Rich Presence");
}

/*--------------------------------------------------
	void DISC_UpdatePresence(void)

		See header file for description.
--------------------------------------------------*/
void DISC_UpdatePresence(void)
{
	DiscordRichPresence discordPresence;
	char detailstr[DISC_STATUS_MAX_STRING_SIZE];
	char statestr[DISC_STATUS_MAX_STRING_SIZE];
	char imagestr[DISC_STATUS_MAX_IMAGE_STRING_SIZE], imagetxtstr[DISC_STATUS_MAX_STRING_SIZE];
	char s_imagestr[DISC_STATUS_MAX_IMAGE_STRING_SIZE], s_imagetxtstr[DISC_STATUS_MAX_STRING_SIZE];
	char *client_joinSecret = NULL;
	char *server_partyID = NULL;

	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.instance = 1;

	if (discordInfo.connectionStatus != DISC_CONNECTED || !cv_discordrp.value)
	{
		/* (Slightly modified 'TehRealSalt' comment) */
		// User either doesn't want to show their game information, or can't show presence.
		// So, update with empty presence.
		// This just shows that they're playing TSoURDt3rd. (If that's too much, then they should disable game activity :V)
		DISC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	DISC_SetActivityStatus(
		detailstr, statestr,
		imagestr, imagetxtstr,
		s_imagestr, s_imagetxtstr,
		(time_t *)&discordPresence.startTimestamp, (time_t *)&discordPresence.endTimestamp,
		&client_joinSecret,
		&server_partyID, &discordPresence.partySize, &discordPresence.partyMax
	);
	discordPresence.details = detailstr;
	discordPresence.state = statestr;
	discordPresence.largeImageKey = imagestr; discordPresence.largeImageText = imagetxtstr;
	discordPresence.smallImageKey = s_imagestr; discordPresence.smallImageText = s_imagetxtstr;
	discordPresence.joinSecret = client_joinSecret;
	discordPresence.partyId = server_partyID;
	Discord_UpdatePresence(&discordPresence);
}

/*--------------------------------------------------
	void DISC_RunCallbacks(void)

		See header file for description.

	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/
void DISC_RunCallbacks(void)
{
	Discord_RunCallbacks();
#ifdef DISCORD_DISABLE_IO_THREAD
	Discord_UpdateConnection();
#endif
}

/*--------------------------------------------------
	void DISC_Respond(const char *userID, DISC_RequestReply_t response)

		See header file for description.

	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/
void DISC_Respond(const char *userID, DISC_RequestReply_t response)
{
	Discord_Respond(userID, (UINT32)response);
}

/*--------------------------------------------------
	void DISC_RemoveRequest(DISC_Request_t *removeRequest)

		See header file for description.

	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/
void DISC_RemoveRequest(DISC_Request_t *removeRequest)
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
	void DISC_Quit(void)

		See header file for description.
--------------------------------------------------*/
void DISC_Quit(void)
{
	DISC_HandleQuitting();
	Discord_ClearPresence();
	Discord_Shutdown();
	I_OutputMsg("DISC_Quit(): Closing Discord Rich Presence...\n");
}

#endif // HAVE_DISCORDRPC
