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
#include "../m_menu.h" // gametype_cons_t and jukebox stuff
#include "../r_things.h" // skins
#include "../mserv.h" // msServerId
#include "../z_zone.h"
#include "../byteptr.h"
#include "../i_tcp.h" // current_port

#include "../p_local.h" // all7matchemeralds

#include "../m_cond.h" // queries about emblems
#include "../v_video.h" // hud stuff, mainly

#include "../w_wad.h" // numwadfiles
#include "../d_main.h" // extrawads
#include "../d_netfil.h" // nameonly
#include "../doomstat.h" // savemoddata
#include "../dehacked.h" // titlechanged

#include "../STAR/star_vars.h" // TSoURDt3rd structure

// ------------------------ //
//        Variables
// ------------------------ //

struct discordInfo_s discordInfo;

discordRequest_t *discordRequestList = NULL;

static char self_ip[IP_SIZE];

#ifndef DEVELOP
boolean devmode = false;
#else
boolean devmode = true;
#endif

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
	discordInfo.ConnectionStatus = DRPC_CONNECTED;
	snprintf(discord_username, 128, "%s#%s (%s)", user->username, user->discriminator, user->userId);
	CONS_Printf("Discord: connected to %s\n", DRPC_ReturnUsername());
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
	discordInfo.ConnectionStatus = DRPC_DISCONNECTED;
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

	if (discordInfo.joinsAllowed == true)
	{
		if (discordInfo.everyoneCanInvite == true)
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

	static char detailstr[128];
	static char statestr[128];

	//char gtname[128];

	char simagestr[30+26+8] = "";
	char simagetxtstr[22+26+16] = "";

	char imagestr[28+24+12] = "";
	char imagetxtstr[18+34+12] = "";


	char customSImage[32+18] = "";
	char customLImage[35+7+8] = "";

	// nerd emoji moment //
	char detailGrammar[1+2] = "";
	
	char stateGrammar[2+2] = "";
	char stateType[10+9+5] = "";

	char lifeType[9+10+2+7] = "";
	char lifeGrammar[9+10+2+3+4] = "";

	char spectatorType[9+10] = "";
	char spectatorGrammar[2+3] = "";

	char gametypeGrammar[2+3+1+9] = "";
	char gameType[2+3+8+9+25+12] = "";

	char addonsLoaded[3+2+9+8+7+5] = "";
	// end of the nerd emoji moment //

	static const char *supportedSkins[] = {
		// Vanilla Chars
		"custom",		// ...Does ghost sonic count as a vanilla char? Maybe.

		"sonic",

		"tails",
		"knuckles",
		"amy",
		"fang",
		"metalsonic",
		
		"sonictails",	// Bots, am I right?
		
		// Custom Chars
		"adventuresonic",
		"shadow",
		"skip",
		"jana",
		"surge",
		"cacee",
		"milne",
		"maimy",
		"mario",
		"luigi",
		"blaze",
		"marine",
		"tailsdoll",
		"metalknuckles",
		"smiles",
		"whisper",

		// My Chars
		"hexhog",

		// My Friendos' Chars
		"speccy",		// Spec Spec is my Friendo
		NULL
	};

	static const char *supportedSuperSkins[] = {
		"sonic", "supersonic",
		"sonictails",			// Bots, am I right?
		NULL
	};

	static const char *supportedMaps[] = {
		// Singleplayer/Co-op Maps
		"01",
		"02",
		"03",
		"04",
		"05",
		"06",
		"07",
		"08",
		"09",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"22",
		"23",
		"25",
		"26",
		"27",
		
		// Extra Maps
		"30",
		"31",
		"32",
		"33",
		
		// Advanced Maps
		"40",
		"41",
		"42",
		
		// Singleplayer Special Stages
		"50",
		"51",
		"52",
		"53",
		"54",
		"55",
		"56",
		"57",
		
		// Co-op Special Stages
		"60",
		"61",
		"62",
		"63",
		"64",
		"65",
		"66",
		
		// Bonus NiGHTS Stages
		"70",
		"71",
		"72",
		"73",
		
		// Match/Team Match/H&S/Tag Maps
		"f0",
		"f1",
		"f2",
		"f3",
		"f4",
		"f5",
		"f6",
		"f7",
		"f8",
		
		// CTF Maps
		"m0",
		"m1",
		"m2",
		"m3",
		"m4",
		"m5",
		"m6",
		"m7",
		"m8",
		"m9",
		"ma",
		"mb",
	
		// Tutorial Map
		"z0",
		
		// Custom Maps
		"custom",
		NULL
	};

	static const char *supportedMiscs[] = {
		"title",

		"intro1",
		"intro2",
		"intro3",
		"intro4",
		"intro5",
		"intro6",
		"intro7",
		"intro8",

		"altsonicimage1",
		"altsonicimage2",
		"altsonicimage3",
		"altsonicimage4",
		"altsonicimage5",

		"alttailsimage1",
		"alttailsimage2",

		"altknucklesimage1",
		"altknucklesimage2",

		"altamyimage1",

		"altfangimage1",

		"altmetalsonicimage1",
		"altmetalsonicimage2",

		"alteggmanimage1",

		NULL
	};

	static const char *customStringType[] = {
		"char",
		"cont",
		"life",

		"charsuper",
		"contsuper",
		"lifesuper",

		"map",
		"misc",

		NULL
	};

#if 0
	/* STAR NOTE: I Might Have a Better Idea for This, Actually */
	static const char *customStringLink[] = {
		"#s",
		"#j",
		"#t",
		"#e",
		"#m",
		NULL
	};
#endif

	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer]; // Obvious

	if (dedicated)
	{
		return;
	}

	// Statuses //
	if (!cv_discordrp.value)
	{
		DRPC_StringPrintf(detailstr, NULL, 128, "In-Game");

		if (paused)
			DRPC_StringPrintf(statestr, NULL, 128, "Paused");
		else if (menuactive || !Playing())
			DRPC_StringPrintf(statestr, NULL, 128, "In a Menu");
		else
			DRPC_StringPrintf(statestr, NULL, 128, "Active");

		DRPC_StringPrintf(imagestr, NULL, 128, "misctitle");
		DRPC_StringPrintf(imagetxtstr, NULL, 128, "Sonic Robo Blast 2");

		// User doesn't want to show their game information, so update with empty presence.
		// This just shows that they're playing SRB2Kart. (If that's too much, then they should disable game activity :V)
		DRPC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	// Netgames
	if (netgame)
	{
		if (DRPC_InvitesAreAllowed() == true)
		{
			const char *join;

			// Grab the host's IP for joining.
			if ((join = DRPC_GetServerIP()) != NULL)
			{
				if (!devmode)
				{
					discordPresence.joinSecret = DRPC_XORIPString(join);
					joinSecretSet = true;
				}
				else
				{
					clientJoinSecret = DRPC_XORIPString(join);
					discordPresence.joinSecret = clientJoinSecret;
					joinSecretSet = true;
				}
			}
			else
			{
				return;
			}
		}

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

		discordPresence.partyId	= server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
		discordPresence.partySize = D_NumPlayers(); // Players in server
		discordPresence.partyMax = discordInfo.maxPlayers; // Max players
	}
	else
	{
		memset(&discordInfo.maxPlayers, 0, sizeof(discordInfo.maxPlayers));
		memset(&discordInfo.joinsAllowed, 0, sizeof(discordInfo.joinsAllowed));
		memset(&discordInfo.everyoneCanInvite, 0, sizeof(discordInfo.everyoneCanInvite));
	}

	// Devmode
	if (devmode)
	{
		DRPC_StringPrintf(detailstr, " | ", 128, "Developing a Masterpiece");
		DRPC_StringPrintf(statestr, " | ", 128, "Keep your Eyes Peeled!");

		DRPC_ImagePrintf(imagestr, 128, "map", "custom");
		DRPC_StringPrintf(imagetxtstr, NULL, 128, "Hey! No Peeking!");

		goto pushPresence;
	}

	// Main
	switch (cv_discordshowonstatus.value)
	{
		case 1:
			DRPC_CharacterStatus(statestr, imagestr, simagestr, imagetxtstr, simagetxtstr);
			if (*imagetxtstr != '\0')
				DRPC_StringPrintf(detailstr, NULL, 128, imagetxtstr);
			if (*simagetxtstr != '\0')
				DRPC_StringPrintf(statestr, NULL, 128, simagetxtstr);
			goto pushPresence;

		case 2:
			DRPC_ScoreStatus(!netgame ? detailstr : statestr);
			goto pushPresence;

		case 3:
			DRPC_EmeraldStatus(!cv_discordshowonstatus.value ? detailstr : statestr);
			goto pushPresence;

		case 4:
			DRPC_EmblemStatus(!netgame ? detailstr : statestr);
			goto pushPresence;

		case 5:
			DRPC_GamestateStatus(statestr, imagestr, imagetxtstr);
			DRPC_StringPrintf(statestr, NULL, 128, imagestr);
			goto pushPresence;

		case 6:
			break;

		case 7:
			DRPC_PlaytimeStatus((Playing() && !netgame) ? detailstr : statestr);
			goto pushPresence;

		case 8:
			//DRPC_CustomStatus(detailstr, statestr);
			goto customStatus;

		default:
		{
			DRPC_EmblemStatus(detailstr);
			DRPC_EmeraldStatus(!cv_discordshowonstatus.value ? detailstr : statestr);


			DRPC_GamestateStatus(statestr, imagestr, imagetxtstr);

			break;
		}
	}

	// Main Statuses //
	// Image Text
	if ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK)
		|| ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK)
			&& (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5))
		|| (cv_discordshowonstatus.value >= 2 && cv_discordshowonstatus.value != 5))
	{
		snprintf(imagestr, 128, "misctitle");
		strcpy(imagetxtstr,
				// Allow Statuses
				((!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6) ?
					((gamestate == GS_TIMEATTACK) ? "Time Attack" : "Title Screen") :
				
				// Show No Statuses
				("Sonic Robo Blast 2")));
		
		(((!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6) && !Playing()) ?
			snprintf(statestr, 130,
					// Game States
					((gamestate == GS_TIMEATTACK) ? "In the Time Attack Menu" :

					// Demo States
					((!demoplayback && !titledemo) ? "Main Menu" :
					((demoplayback && !titledemo) ? "Watching Replays" :
					((demoplayback && titledemo) ? "Watching A Demo" : "???"))))) : 0);
	}

	// Misc. Status Text
	if ((playeringame[consoleplayer] && !demoplayback) || cv_discordshowonstatus.value == 7)
	{
		if (!splitscreen && !netgame)
		{
			if (Playing() && (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 3 && cv_discordshowonstatus.value != 5 && cv_discordshowonstatus.value != 6))
				strcpy(detailGrammar, ", ");
			
			if (gamecomplete) // You've Beaten the Game? You Get A Special Status Then!
				strlcat(detailstr, va("%sHas Beaten the Game", detailGrammar), 128);
		}
	}

	////// 	  STATUSES - ELECTRIC BOOGALO 	 //////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6)
	{
		if (((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing() && playeringame[consoleplayer]) || (paused || menuactive || TSoURDt3rd->jukebox.curtrack))
		{
			//// Statuses That Only Appear In-Game ////
			if (Playing())
			{
				// Modes //
				strcpy(gametypeGrammar, (!ultimatemode ? "Playing " : "Taking on "));
				snprintf(gameType, 64,
					// Main Gametypes/Ultimate Mode/Mode Attacking/Split-Screen
					(modeattacking ?
						((maptol != TOL_NIGHTS && maptol != TOL_XMAS) ? "Time Attack" : "NiGHTS Mode") :

						(!splitscreen ? ((gametype == GT_COOP && !netgame) ?
							(!ultimatemode ? "Single-Player" : "Ultimate Mode") : "%s") :
						("Split-Screen"))),
					
					// Custom Gametypes/Mode Attacking
					(modeattacking ?
						("") :
						(netgame ? gametype_cons_t[gametype].strvalue : "")));
				
				// Add-ons //
				if ((modifiedgame || autoloaded) && numwadfiles > (mainwads+extrawads))
					strcpy(addonsLoaded, ((numwadfiles - (mainwads+extrawads) > 1) ? va(" With %d Add-ons", (numwadfiles - (mainwads+extrawads))) : " With 1 Add-on"));
				
				// Lives //
				if (!players[consoleplayer].spectator && gametyperules & GTR_LIVES && !(ultimatemode || modeattacking))
					snprintf(lifeGrammar, 22, (!players[consoleplayer].lives ? ", Game Over..." : ((players[consoleplayer].lives == INFLIVES) || (!cv_cooplives.value && (netgame || multiplayer))) ? ", %lc Lives" : (players[consoleplayer].lives == 1 ? ", %d Life Left" : ", %d Lives Left")), (((players[consoleplayer].lives == INFLIVES) || (!cv_cooplives.value && (netgame || multiplayer))) ? 0x221E : players[consoleplayer].lives));
				else if (TSoURDt3rdPlayers[consoleplayer].timeOver)
					strcpy(lifeGrammar, ", Time Over...");
				
				// Spectators //
				if (!players[consoleplayer].spectator)
				{
					strcpy(spectatorGrammar, (((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer != consoleplayer))) ? "ing" : "er"));
					strcpy(spectatorType, va(", View%s", spectatorGrammar));
				}
				else
				{
					strcpy(lifeGrammar, ", Dead; ");
					strcpy(spectatorGrammar, (((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer == consoleplayer))) ? "ing" : "or"));
					strcpy(spectatorType, va("Spectat%s", spectatorGrammar));
					
					if (displayplayer == consoleplayer)
						strcpy(lifeType, va((!cv_discordstatusmemes.value ? "In %s Mode" : "%s Air"), spectatorType));
				}
				if (displayplayer != consoleplayer)
					strcpy(lifeType, va("%s %s", spectatorType, player_names[displayplayer]));
			}

			//// Statuses That Can Appear Whenever ////
			// Tiny States, Such as Pausing, Scrolling Through Menus, etc. //
			if (paused || menuactive || TSoURDt3rd->jukebox.curtrack)
			{
				if (!cv_discordshowonstatus.value || (cv_discordshowonstatus.value == 6 && Playing()) || !Playing())
					strcpy(stateGrammar, ", ");

				snprintf(stateType, 27, (paused ? "%sCurrently Paused" : (menuactive ? "%sScrolling Through Menus" : "")), stateGrammar);
				strlcat(stateType, (TSoURDt3rd->jukebox.curtrack ? va("%sPlaying '%s' in the Jukebox", stateGrammar, TSoURDt3rd->jukebox.curtrack->title) : ""), 95);
			}
			
			// Copy All Of Our Strings //
			strlcat(statestr, va("%s%s%s%s%s%s", gametypeGrammar, gameType, addonsLoaded, lifeGrammar, lifeType, stateType), 130);
		}
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

	////// 	  CHARACTERS 	 //////
	DRPC_CharacterStatus(statestr, imagestr, simagestr, imagetxtstr, simagetxtstr);

	goto pushPresence;

	////// 	  CUSTOM STATUSES 	 //////
	customStatus:
	{
		// Error Out if the String is Less Than Two Letters Long //
		// MAJOR STAR NOTE: please come back to this and flesh it out more lol //
		if (strlen(cv_customdiscorddetails.string) <= 2 || strlen(cv_customdiscordstate.string) <= 2 || strlen(cv_customdiscordsmallimagetext.string) <= 2 || strlen(cv_customdiscordlargeimagetext.string) <= 2)
		{
			M_StartMessage(va("%c%s\x80\nSorry, Discord RPC requires Strings to be longer than two characters.\n\nResetting strings with less than two letters back to defaults. \n\n(Press a key)\n", ('\x80' + (V_MENUCOLORMAP|V_CHARCOLORSHIFT)), "Custom Discord RPC String Too Short"),NULL,MM_NOTHING);
			S_StartSound(NULL, sfx_skid);

			if (strlen(cv_customdiscorddetails.string) <= 2)
				CV_Set(&cv_customdiscorddetails, cv_customdiscorddetails.defaultvalue);
			if (strlen(cv_customdiscordstate.string) <= 2)
				CV_Set(&cv_customdiscordstate, cv_customdiscordstate.defaultvalue);

			if (strlen(cv_customdiscordsmallimagetext.string) <= 2)
				CV_Set(&cv_customdiscordsmallimagetext, cv_customdiscordsmallimagetext.defaultvalue);
			if (strlen(cv_customdiscordlargeimagetext.string) <= 2)
				CV_Set(&cv_customdiscordlargeimagetext, cv_customdiscordlargeimagetext.defaultvalue);
		}

		// Write the Heading Strings to Discord
		if (strlen(cv_customdiscorddetails.string) > 2)
			strcpy(detailstr, cv_customdiscorddetails.string);
		if (strlen(cv_customdiscordstate.string) > 2)
			strcpy(statestr, cv_customdiscordstate.string);

		// Write The Images and Their Text to Discord //
		// Small Images
		if (cv_customdiscordsmallimagetype.value != 8)
		{
			strcpy(customSImage, va("%s%s", customStringType[cv_customdiscordsmallimagetype.value],
				(cv_customdiscordsmallimagetype.value <= 2 ? supportedSkins[cv_customdiscordsmallcharacterimage.value] :
				((cv_customdiscordsmallimagetype.value >= 3 && cv_customdiscordsmallimagetype.value <= 5) ? supportedSuperSkins[cv_customdiscordsmallsupercharacterimage.value] :
				(cv_customdiscordsmallimagetype.value == 6 ? supportedMaps[cv_customdiscordsmallmapimage.value] :
			supportedMiscs[cv_customdiscordsmallmiscimage.value])))));
		
			strcpy(simagestr, customSImage);
			(strlen(cv_customdiscordsmallimagetext.string) > 2 ? strcpy(simagetxtstr, cv_customdiscordsmallimagetext.string) : 0);
		}
		
		// Large Images
		if (cv_customdiscordlargeimagetype.value != 8)
		{
			strcpy(customLImage, va("%s%s", customStringType[cv_customdiscordlargeimagetype.value],
				(cv_customdiscordlargeimagetype.value <= 2 ? supportedSkins[cv_customdiscordlargecharacterimage.value] :
				((cv_customdiscordlargeimagetype.value >= 3 && cv_customdiscordlargeimagetype.value <= 5) ? supportedSuperSkins[cv_customdiscordlargesupercharacterimage.value] :
				(cv_customdiscordlargeimagetype.value == 6 ? supportedMaps[cv_customdiscordlargemapimage.value] :
			supportedMiscs[cv_customdiscordlargemiscimage.value])))));

			strcpy(imagestr, customLImage);
			(strlen(cv_customdiscordlargeimagetext.string) > 2 ? strcpy(imagetxtstr, cv_customdiscordlargeimagetext.string) : 0);
		}
	}

	// We can finally push our new status! :) //
	pushPresence:
	{
		discordPresence.details = detailstr;
		discordPresence.state = statestr;

		discordPresence.smallImageKey = simagestr;
		discordPresence.smallImageText = simagetxtstr;

		discordPresence.largeImageKey = imagestr;
		discordPresence.largeImageText = imagetxtstr;

		if (joinSecretSet == false)
		{
			// Not able to join? Flush the request list, if it exists.
			DRPC_EmptyRequests();
		}
		Discord_UpdatePresence(&discordPresence);

		memset(&detailstr, 0, sizeof(detailstr));
		memset(&statestr, 0, sizeof(statestr));

		free(clientJoinSecret);
	}
}

/*--------------------------------------------------
	void DRPC_Shutdown(void)

		Clears everything related to Discord Rich Presence.
		Only runs when the game closes or crashes.
--------------------------------------------------*/

void DRPC_Shutdown(void)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	memset(&discordInfo, 0, sizeof(discordInfo));

	discordPresence.details = "Currently Closing...";
	discordPresence.state = "Clearing SRB2 Discord Rich Presence...";

	DRPC_EmptyRequests();
	Discord_UpdatePresence(&discordPresence);

	Discord_RunCallbacks();
#ifdef DISCORD_DISABLE_IO_THREAD
	Discord_UpdateConnection();
#endif

	Discord_ClearPresence();
	Discord_Shutdown();

	I_OutputMsg("DRPC_Shutdown(): shut down\n");
}

#endif // HAVE_DISCORDRPC
