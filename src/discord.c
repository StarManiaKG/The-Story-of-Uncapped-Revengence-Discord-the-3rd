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

static CV_PossibleValue_t discordstatustype_cons_t[] = {{0, "All"}, {1, "Only Characters"}, {2, "Only Player Name"}, {3, "Only Emeralds"}, {4, "Only Emblems"}, {5, "Only Levels"}, {6, "Only Gametype"}, {7, "Custom"}, {8, "None"}, {0, NULL}};
consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DRPC_UpdatePresence);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "all", CV_SAVE|CV_CALL, discordstatustype_cons_t, Discordcustomstatus_option_Onchange);
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

	if (!cv_discordasks.value)
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

	//char playerimg[25+50+15+10];
	char playername[11+MAXPLAYERNAME+1];

	char servertype[15+10];
	char servertag[11+26+15+8];

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

	////////////////////////////////////////////
	////   Main Rich Presence Status Info   ////
	////////////////////////////////////////////
	// Server info
	if (dedicated || netgame)
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
	}
		
	if (netgame && serverrunning)
	{
		switch (ms_RoomId)
		{
			case -1: snprintf(servertype, 26, "Private"); break; // Private server
			case 33: snprintf(servertype, 26, "Standard"); break;
			case 28: snprintf(servertype, 26, "Casual"); break;
			case 38: snprintf(servertype, 26, "Custom Gametypes"); break;
			case 31: snprintf(servertype, 26, "OLDC"); break;
			default: snprintf(servertype, 26, "Private"); break; // HOW
		}

		if (server)
		{
			if (!dedicated)
				snprintf(servertag, 60, "Hosting a %s Server", servertype);
			else
				snprintf(servertag, 60, "Hosting a Dedicated %s Server", servertype);
		}
		else
			snprintf(servertag, 60, "In a %s Server", servertype);

		discordPresence.details = servertag;
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

		if (Playing())
		{
			//Tiny Emerald Counter
			UINT8 emeraldCount = 0;

			for (INT32 i = 0; i < 7; i++) // thanks Monster Iestyn for this math
				if (emeralds & (1<<i))
					emeraldCount += 1;

			//// Emblems ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 4)
				snprintf(detailstr, 20, "%d/%d Emblems", M_CountEmblems(), (numemblems + numextraemblems));
			
			//// Emeralds ////
			//i think you know what the joke here is
			if (!cv_discordstatusmemes.value)
			{
				//there's a special stage token right at the BEGINNING OF GFZ1 HOW DO YOU NOT HAVE A EMERALD YET
				if (!emeralds)
				{
					if (cv_discordshowonstatus.value == 0)
						strlcat(detailstr, ", No Emeralds", 64);
					else if (cv_discordshowonstatus.value == 3)
						strlcat(detailstr, "No Emeralds", 64);
				}
				//Mystic Power Gang
				else
				{
					if (!cv_discordshowonstatus.value)
					{
						if (emeraldCount != 7)
							strlcat(detailstr, va(", %d Emeralds", emeraldCount), 64);
						else
							strlcat(detailstr, ", All 7 Emeralds Obtained!", 64);
					}
					else if (cv_discordshowonstatus.value == 3)
					{
						if (emeraldCount != 7)
							strlcat(detailstr, va("%d Emeralds", emeraldCount), 64);
						else
							strlcat(detailstr, "All 7 Emeralds Obtained!", 64);
					}
				}
			}
			//Honestly relatable lol
			else
			{
				//Man, Special Stage Got Hands
				if (!emeralds)
				{
					if (!cv_discordshowonstatus.value)
						strlcat(detailstr, ", NO EMERALDS?", 64);
					else if (cv_discordshowonstatus.value == 3)
						strlcat(detailstr, "NO EMERALDS?", 64);
				}
				//Mystic Power Gang
				else
				{
					if (!cv_discordshowonstatus.value)
					{
						if (emeraldCount < 7 && emeraldCount != 3 && emeraldCount != 4)
							strlcat(detailstr, va(", %d Emeralds", emeraldCount), 64);
						else if (emeraldCount == 3)
							// Fun Fact: the subtitles in Shadow the Hedgehog emphasized "fourth", even though Jason Griffith emphasized "damn" in this sentence
							strlcat(detailstr, ", %d Emeralds; Where's That DAMN FOURTH?", 64);
						else if (emeraldCount == 4)
							strlcat(detailstr, ", %d Emeralds; Found that DAMN FOURTH", 64);
						else if (emeraldCount == 7)
							strlcat(detailstr, ", All 7 Emeralds Obtained!", 64);
					}
					else if (cv_discordshowonstatus.value == 3)
					{
						if (emeraldCount < 7 && emeraldCount != 3 && emeraldCount != 4)
							strlcat(detailstr, va("%d Emeralds", emeraldCount), 64);
						else if (emeraldCount == 3)
							// You Already Know the Fun Fact lol
							strlcat(detailstr, "%d Emeralds; Where's That DAMN FOURTH?", 64);
						else if (emeraldCount == 4)
							strlcat(detailstr, "%d Emeralds; Found that DAMN FOURTH", 64);
						else if (emeraldCount == 7)
							strlcat(detailstr, "All 7 Emeralds Obtained!", 64);
					}
				}
			}

			discordPresence.details = detailstr;
		}
		else if (demoplayback && !titledemo)
			discordPresence.state = "Watching Replays";
		else
		{
			discordPresence.largeImageKey = "misctitle";
				
			if (!cv_discordshowonstatus.value)
			{
				discordPresence.largeImageText = "Title Screen";
				discordPresence.state = "Main Menu";
			}
		}
	}

	//// Gametypes ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6)
	{
		if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing())
		{
			if (modeattacking)
				discordPresence.details = "Time Attacking";
			else
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
				else
					discordPresence.state = "Playing Split-Screen";
			}
		}
	}

	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 5)
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
			//Fixes the Null Map Issue When Loading Into the Title Screen
			else if ((gamemap == 99) || (gamestate == GS_TITLESCREEN) || (gamestate == GS_EVALUATION))
			{
				discordPresence.largeImageKey = "misctitle";
				if (gamestate == GS_EVALUATION)
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

	//// Characters ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 1)
	{
		if (Playing() && playeringame[consoleplayer])
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

			if (!splitscreen)
			{
				// No Bots
				if (!players[1].bot || netgame)
				{
					//// Character images
					// Supported
					if ((strcmp(skins[players[consoleplayer].skin].name, baseSkins[strlen(skins[players[consoleplayer].skin].name)])) || (strcmp(skins[players[consoleplayer].skin].name, customSkins[strlen(skins[players[consoleplayer].skin].name)])))
						snprintf(charimg, 28, "char%s", skins[players[consoleplayer].skin].name);
					// Unsupported
					else
						snprintf(charimg, 11, "charcustom");
					
					//// Player names
					if (!players[consoleplayer].spectator)
						// Character
						snprintf(playername, 28, "Playing As: %s", skins[players[consoleplayer].skin].realname);
					// Viewpoint
					else
					{
						if (playeringame[displayplayer])
							snprintf(playername, 28, "%s is Spectating %s", player_names[consoleplayer], player_names[displayplayer]); // Combine Player Names Together
						else
						{
							if (!cv_discordstatusmemes.value)
								snprintf(playername, 28, "%s is Spectating", player_names[consoleplayer]); // you're no fun, you know
							else
								snprintf(playername, 28, "%s is Spectating Air", player_names[consoleplayer]); // why are you spectating air
						}		
					}
					
					// render character variables
					discordPresence.smallImageKey = charimg; // Character image
					discordPresence.smallImageText = playername; // Player names
				}
				// Bots
				else if (players[1].bot)
				{
					////Only One Regular Bot?
					if (!players[2].bot)
					{
						// Character images
						//if ((strcmp(skins[players[consoleplayer].skin].name, "sonic")) && (strcmp(((skin_t *)players[1].bot)->name, "tails")))) ////Scrapped until we can get it to work
						snprintf(charimg, 15, "charsonictails");
						snprintf(charname, 28, "Playing As: %s & %s", skins[players[consoleplayer].skin].realname, skins[players[1].skin].realname);
					}
					////Multiple Bots?
					else
					{
						snprintf(charimg, 28, "char%s", skins[players[consoleplayer].skin].name);
						snprintf(charname, 50, "Playing As: %s & Multiple Bots", skins[players[consoleplayer].skin].name);
					}

					// render character variables
					discordPresence.smallImageKey = charimg; // Character image
					discordPresence.smallImageText = charname; // Character name, Bot name
				}
			}
			else
			{
				// render character image
				snprintf(charimg, 28, "charsonictails");
				discordPresence.smallImageKey = charimg;

				// Player names
				snprintf(playername, 50, "%s & %s", player_names[consoleplayer], player_names[secondarydisplayplayer]);
				discordPresence.smallImageText = playername;
			}
		}
	}
	
	//// Custom Statuses ////
	if (cv_discordshowonstatus.value == 7)
	{
		if (cv_customdiscordstatus.string)
		{
			snprintf(charimg, 11, "charcustom");
			discordPresence.details = cv_customdiscordstatus.string;
			discordPresence.largeImageKey = charimg;
			discordPresence.largeImageText = "Custom Image";
		}
	}

	//// NO STATUS? ////
	if (cv_discordshowonstatus.value == 8)
	{
		discordPresence.largeImageKey = "misctitle";
		discordPresence.largeImageText = "Sonic Robo Blast 2";
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