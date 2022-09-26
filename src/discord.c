// SONIC ROBO BLAST 2 //WITH DISCORD RPC BROUGHT TO YOU BY THE KART KREW
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord.h
/// \brief Discord Rich Presence handling

#ifdef HAVE_DISCORDRPC

#include <time.h>

#include "i_system.h"
#include "d_clisrv.h"
#include "d_netcmd.h"
#include "i_net.h"
#include "g_game.h"
#include "p_tick.h"
#include "m_menu.h" // gametype_cons_t
#include "r_things.h" // skins
#include "mserv.h" // ms_RoomId
#include "m_cond.h" // queries about emblems
#include "z_zone.h"
#include "byteptr.h"
#include "stun.h"
#include "i_tcp.h" // current_port

#include "discord.h"
#include "doomdef.h"

// Feel free to provide your own, if you care enough to create another Discord app for this :P
#define DISCORD_APPID "1013126566236135516"

// length of IP strings
#define IP_SIZE 21

static CV_PossibleValue_t discordstatustype_cons_t[] = {{0, "All"}, {1, "Characters"}, {2, "Continues"}, {3, "Emeralds"}, {4, "Emblems"}, {5, "Levels"}, {6, "Gametype"}, {7, "Custom"}, {8, "None"}, {0, NULL}};
consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DRPC_UpdatePresence);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "all", CV_SAVE|CV_CALL, discordstatustype_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordstatus = CVAR_INIT ("customdiscordstatus", "I'm Playing Sonic Robo Blast 2!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DRPC_UpdatePresence);
struct discordInfo_s discordInfo;

discordRequest_t *discordRequestList = NULL;

static char self_ip[IP_SIZE+1];

/*--------------------------------------------------
	static char *DRPC_XORIPString(const char *input)

		Simple XOR encryption/decryption. Not complex or
		very secretive because we aren't sending anything
		that isn't easily accessible via our Master Server anyway.
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
		{
			xorinput = input[i];
		}

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
	if (cv_discordstreamer.value)
	{
		CONS_Printf("Discord: connected to %s\n", user->username);
	}
	else
	{
		CONS_Printf("Discord: connected to %s#%s (%s)\n", user->username, user->discriminator, user->userId);
	}
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

	/*if (discordInfo.joinsAllowed == true)
	{
		if (discordInfo.everyoneCanInvite == true)
		{*/
			// Everyone's allowed!
			return true;
		/*}
		else if (consoleplayer == serverplayer || IsPlayerAdmin(consoleplayer))
		{
			// Only admins are allowed!
			return true;
		}
	}*/

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
		None
--------------------------------------------------*/
static void DRPC_HandleJoinRequest(const DiscordUser *requestUser)
{
	discordRequest_t *append = discordRequestList;
	discordRequest_t *newRequest;

	if (DRPC_InvitesAreAllowed() == false)
	{
		// Something weird happened if this occurred...
		Discord_Respond(requestUser->userId, DISCORD_REPLY_IGNORE);
		return;
	}

	newRequest = Z_Calloc(sizeof(discordRequest_t), PU_STATIC, NULL);

	newRequest->username = Z_Calloc(344, PU_STATIC, NULL);
	snprintf(newRequest->username, 344, "%s", requestUser->username);

	newRequest->discriminator = Z_Calloc(8, PU_STATIC, NULL);
	snprintf(newRequest->discriminator, 8, "%s", requestUser->discriminator);

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
		M_RefreshPauseMenu();
	}

	// Made it to the end, request was valid, so play the request sound :)
	S_StartSound(NULL, sfx_requst);
}

/*--------------------------------------------------
	void DRPC_RemoveRequest(discordRequest_t *removeRequest)

		See header file for description.
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
	handlers.ready = DRPC_HandleReady;
	handlers.disconnected = DRPC_HandleDisconnect;
	handlers.errored = DRPC_HandleError;
	handlers.joinGame = DRPC_HandleJoin;
	handlers.joinRequest = DRPC_HandleJoinRequest;

	Discord_Initialize(DISCORD_APPID, &handlers, 1, NULL);
	I_AddExitFunc(DRPC_ShutDown);
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
		connected to. Will attempt to use STUN for getting your
		own IP address.
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
			sprintf(self_ip, "%s:%u", address, current_port);
			return self_ip;
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
	char detailstr[64+1];
	char statestr[64+1];

	char mapimg[8+1];
	char mapname[5+21+21+2+1];

	char charimg[4+SKINNAMESIZE+1];
	char charname[11+SKINNAMESIZE+1];
	char secondcharname[11+SKINNAMESIZE+1];

	char playername[MAXPLAYERNAME+1];
	char secondplayername[MAXPLAYERNAME+1];

	char combiring[80];
	char combisring[80];

	boolean joinSecretSet = false;

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	if (!cv_discordrp.value)
	{
		// User doesn't want to show their game information, so update with empty presence.
		// This just shows that they're playing SRB2Kart. (If that's too much, then they should disable game activity :V)
		DRPC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

#ifdef DEVELOP
	// This way, we can use the invite feature in-dev, but not have snoopers seeing any potential secrets! :P
	discordPresence.largeImageKey = "miscdevelop";
	discordPresence.largeImageText = "No peeking!";
	discordPresence.state = "Work (is) In Progress!";

	DRPC_EmptyRequests();
	Discord_UpdatePresence(&discordPresence);
	return;
#endif // DEVELOP

	// Server info
	if (dedicated)
		discordPresence.details = "Hosting a Dedicated Server";
		
	if (netgame)
	{
		if (DRPC_InvitesAreAllowed() == true)
		{
			const char *join;

			// Grab the host's IP for joining.
			if ((join = DRPC_GetServerIP()) != NULL)
			{
				char *xorjoin = DRPC_XORIPString(join);
				discordPresence.joinSecret = xorjoin;
				free(xorjoin);

				joinSecretSet = true;
			}
			else
				return;
		}

		// unfortunally this only works when you are the server /////lol
		switch (ms_RoomId)
		{
			case -1: discordPresence.details = "Private"; break; // Private server
			case 33: discordPresence.details = "Standard"; break;
			case 28: discordPresence.details = "Casual"; break;
			case 38: discordPresence.details = "Custom Gametypes"; break;
			case 31: discordPresence.details = "OLDC"; break;
			default: discordPresence.details = "Unknown Room"; break; // HOW
		}
		if (server)
		{
			discordPresence.details = "Hosting a Netgame";
		}
		else
		{
			discordPresence.details = "In a Netgame";
		}

		discordPresence.partyId = server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
		discordPresence.partySize = D_NumPlayers(); // Players in server
		discordPresence.partyMax = cv_maxplayers.value; // Max players
	}
	else
	{
		// Reset discord info if you're not in a place that uses it!
		// Important for if you join a server that compiled without HAVE_DISCORDRPC,
		// so that you don't ever end up using bad information from another server.
		memset(&discordInfo, 0, sizeof(discordInfo));

		// Offline info
		if (Playing())
		{
			UINT8 emeraldCount = 0;
			
			if (cv_discordshowonstatus.value == 0 || cv_discordshowonstatus.value == 4)
			{
				snprintf(detailstr, 20, "%d/%d Emblems", M_CountEmblems(), (numemblems + numextraemblems));
			}

			if (cv_discordshowonstatus.value == 0 || cv_discordshowonstatus.value == 3)
			{
				if (emeralds != 0)
				{
					for (INT32 i = 0; i < 7; i++) // thanks Monster Iestyn for this math
						if (emeralds & (1<<i))
							emeraldCount += 1;
					if (!cv_discordstatusmemes.value)
						strlcat(detailstr, va(", %d Emeralds", emeraldCount), 64);
					else
					{
						if (emeraldCount < 7 && emeraldCount != 3 && emeraldCount != 4)
							strlcat(detailstr, va(", %d Emeralds", emeraldCount), 64);
						else if (emeraldCount == 3)
							// Trivia: the subtitles in Shadow the Hedgehog emphasized "fourth",
							// even though Jason Griffith emphasized "damn" in this sentence
							strlcat(detailstr, ", %d Emeralds; Where's That DAMN FOURTH?)", 64);
						else if (emeraldCount == 4)
							strlcat(detailstr, ", %d Emeralds; Found that DAMN FOURTH)", 64);
					}

					if (emeralds == 7)
						strlcat(detailstr, ", All 7 Emeralds Obtained!", 64);
				}
				else
				{
					if (cv_discordshowonstatus.value == 3)
					{
						if (!cv_discordstatusmemes.value)
							strlcat(detailstr, ", No Emeralds", 64);
						else
							strlcat(detailstr, ", NO EMERALDS?", 64);
					}
					else if (!cv_discordshowonstatus.value)
					{
						if (!cv_discordstatusmemes.value)
							strlcat(detailstr, ", No Emeralds", 64);
						else
							strlcat(detailstr, ", NO EMERALDS?", 64);
					}
				}
				
				discordPresence.details = detailstr;
			}
		}
		else if (demoplayback && !titledemo)
			discordPresence.state = "Watching Replays";
		else
		{
			discordPresence.largeImageKey = "misctitle";
			discordPresence.largeImageText = "Title Screen";
			discordPresence.state = "Main Menu";
		}
	}

	// Gametype info
	if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing())
	{
		if (modeattacking)
			discordPresence.details = "Time Attacking";
		else
		{
			if (cv_discordshowonstatus.value == 0 || cv_discordshowonstatus.value == 6)
			{
				if (!splitscreen) 
				{
					if (netgame)
					{
						snprintf(statestr, 65, "Playing %s", gametype_cons_t[gametype].strvalue);
						discordPresence.state = statestr;
					}
					else
					{
						if ((gametype == GT_COOP) && (!netgame))
							discordPresence.state = "Playing Single-Player";
					}
				}
			}
		}
	}
	if (cv_discordshowonstatus.value == 0 || cv_discordshowonstatus.value == 5)
	{
		if (gamestate == GS_INTRO)
		{
			discordPresence.largeImageKey = "misctitle";
			discordPresence.largeImageText = "Watching the Intro";
		}
		else if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) // Map info
			&& !(demoplayback && titledemo))
		{
			if ((gamemap >= 1 && gamemap <= 73) // Supported Co-op maps
			|| (gamemap >= 280 && gamemap <= 288) // Supported CTF maps
			|| (gamemap >= 532 && gamemap <= 543)) // Supported Match maps
			{
				snprintf(mapimg, 8, "%s", G_BuildMapName(gamemap));
				strlwr(mapimg);
				discordPresence.largeImageKey = mapimg; // Map image
			}
			else if ((gamemap == 99) || (gamestate == GS_TITLESCREEN))
				discordPresence.largeImageKey = "misctitle";
			else if (gamestate == GS_EVALUATION)
			{
				discordPresence.largeImageKey = "misctitle";
				discordPresence.largeImageText = "Evaluating Results";
			}
			else
				discordPresence.largeImageKey = "mapcustom";
			
			if (mapheaderinfo[gamemap - 1]->menuflags & LF2_HIDEINMENU)
			{
				// Hell map, hide the name
				discordPresence.largeImageText = "Map: ???";
			}
			else
			{
				// Map name on tool tip
				snprintf(mapname, 48, "On %s", G_BuildMapTitle(gamemap));
				discordPresence.largeImageText = mapname;
			}

			if (gamestate == GS_LEVEL && Playing())
			{
				const time_t currentTime = time(NULL);
				const time_t mapTimeStart = currentTime - (leveltime / TICRATE);

				discordPresence.startTimestamp = mapTimeStart;

				if (timelimitintics > 0)
				{
					const time_t mapTimeEnd = mapTimeStart + ((timelimitintics + 1) / TICRATE);
					discordPresence.endTimestamp = mapTimeEnd;
				}
			}
		}
	}

	// Character info
	if ((cv_discordshowonstatus.value == 0 || cv_discordshowonstatus.value == 1) && Playing() && playeringame[consoleplayer] && !players[consoleplayer].spectator)
	{
		// Supported Skin Pictures
		static const char *baseSkins[] = {
			"sonic",
			"tails",
			"knuckles",
			"amy",
			"fang",
			"metalsonic",
			NULL
		};

        // Supported Skin Pictures
		static const char *customSkins[] = {
			"adventuresonic",
			"shadow",
			"skip",
			"jana",
			"surge",
			"cacee",
			"milne",
			"maiamy",
			"mario",
			"luigi",
			"blaze",
			"marine",
			"tailsdoll",
			"metalknuckles",
			"smiles",
			"whisper",
			NULL
		};

		if ((!netgame && !splitscreen) && (playeringame[secondarydisplayplayer]))
		{
			// Character images
			if (strcmp(skins[players[consoleplayer].skin].name, "sonic") 
				&& strcmp(skins[players[secondarydisplayplayer].skin].name, "tails"))
				snprintf(charimg, 28, "charsonictails");
			else
				snprintf(charimg, 28, "char%s", skins[players[consoleplayer].skin].name);
			
			// Character names
			snprintf(charname, 28, "Playing As: %s ", skins[players[consoleplayer].skin].realname);
			snprintf(secondcharname, 28, "& %s", skins[players[secondarydisplayplayer].skin].realname);
			
			//Combine Character Name and Bot Name
			strncat(combiring, strncat(charname, secondcharname, 28), 80); //Combine Ring
			discordPresence.smallImageKey = charimg; // Character image
			discordPresence.smallImageText = combiring; // Character name, Bot name
		}
		else if ((!playeringame[secondarydisplayplayer]) || (netgame))
		{
			// Character images
			if ((strcmp(skins[players[consoleplayer].skin].name, baseSkins[0])) || (strcmp(skins[players[consoleplayer].skin].name, customSkins[0])))
			{
				snprintf(charimg, 28, "char%s", skins[players[consoleplayer].skin].name);
				discordPresence.smallImageKey = charimg;
			}
			// Unsupported Character images
			else
				discordPresence.smallImageKey = "charcustom";

			snprintf(charname, 28, "Playing As: %s", skins[players[consoleplayer].skin].realname);
			discordPresence.smallImageText = charname; // Character name
		}
		else if (splitscreen && playeringame[secondarydisplayplayer])
		{
			// Character images
			snprintf(charimg, 28, "charsonictails");
			discordPresence.smallImageKey = charimg; // Character image

			// Player names
			snprintf(playername, 21, "%s & ", cv_playername.string);
			snprintf(secondplayername, 21, "%s", cv_playername2.string);

			//Combine Player Names
			strncat(combiring, strncat(playername, secondplayername, 21), 21); //Combine Ring (multiplayer edition)

			//Combine Everything
			snprintf(combisring, 80, "%s Are In Split-Screen!", combiring);
			discordPresence.smallImageText = combisring; // Player names
		}
	}
	
	//Custom Status Info
	if (cv_discordshowonstatus.value == 7)
	{
		if (cv_customdiscordstatus.string)
			discordPresence.details = cv_customdiscordstatus.string;
	}

	if (!joinSecretSet)
	{
		// Not able to join? Flush the request list, if it exists.
		DRPC_EmptyRequests();
	}

	Discord_UpdatePresence(&discordPresence);
}

/*--------------------------------------------------
	void DRPC_ShutDown(void)

		Clears Everything Related to Discord
		Rich Presence. Only Runs On Game Close
		or Game Crash.
--------------------------------------------------*/
void DRPC_ShutDown(void)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	DRPC_UpdatePresence();
	Discord_Shutdown();
	Discord_ClearPresence();
}

#endif // HAVE_DISCORDRPC