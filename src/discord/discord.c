// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
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

#include "../doomdef.h"

#include "discord.h"
#include "discord_cmds.h"
#include "discord_statuses.h"
#include "stun.h"

#include "../i_system.h"
#include "../d_clisrv.h"
#include "../d_netcmd.h"
#include "../i_net.h"
#include "../g_game.h"
#include "../p_tick.h"
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

#include "../fastcmp.h" // fastcmp, helps with super stuff

#include "../STAR/star_vars.h" // TSoURDt3rd structure

// ------------------------ //
//        Variables
// ------------------------ //

#if 0
discordInfo_t *discordInfo = NULL;
#else
struct discordInfo_s discordInfo;
#endif

discordRequest_t *discordRequestList = NULL;

size_t g_discord_skins = 0;

static char self_ip[IP_SIZE];

#ifndef DEVELOP
boolean devmode = false;
#else
boolean devmode = true;
#endif

static char discord_username[256];

// ------------------------ //
//        Functions
// ------------------------ //

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
	const char *DRPC_ReturnUsername(const DiscordUser *user);

		Returns the Discord username of the user.
	
	Input Arguments:-
		None

	Return:-
		Discord username string
--------------------------------------------------*/

const char *DRPC_ReturnUsername(const DiscordUser *user)
{
	const char *extrainfo = 0;

	if (user != NULL)
	{
		if (!cv_discordstreamer.value)
			extrainfo = va("#%s (%s)", user->discriminator, user->userId);
		sprintf(discord_username, "%s %s", user->username, extrainfo);
	}
	return discord_username;
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
	CONS_Printf("Discord: connected to %s\n", DRPC_ReturnUsername(user));

	discordInfo.Disconnected = false;
	discordInfo.Initialized	= true;
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
	discordInfo.Disconnected = true;
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
		if (discordInfo.whoCanInvite == 2)
		{
			// Everyone's allowed!
			return true;
		}
		else if (discordInfo.whoCanInvite == 1 && (consoleplayer == serverplayer || IsPlayerAdmin(consoleplayer)))
		{
			// Only admins are allowed!
			return true;
		}
		else if (consoleplayer == serverplayer)
		{
			// Only the server is allowed!
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
		M_RefreshDiscordRequestsOption();
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
	void DRPC_EmptyRequests(void)

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

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	char detailstr[128];
	char statestr[128];

	char charimg[32];
	char charname[128];

	//char gtname[128];

	char simagestr[30+26+8] = "";
	char simagetxtstr[22+26+16] = "";

	char imagestr[28+24+12] = "";
	char imagetxtstr[18+34+12] = "";

	char mapimg[8+1] = "";
	char mapname[5+21+21+2+1] = "";

	char charimgS[4+SKINNAMESIZE+7] = "";
	//char charimg[7+SKINNAMESIZE+4] = "";

	char charnameS[11+SKINNAMESIZE+1] = "";
	//char charname[11+SKINNAMESIZE+1] = "";

	char customSImage[32+18] = "";
	char customLImage[35+7+8] = "";

	// nerd emoji moment //
	char detailGrammar[1+2] = "";
	
	char stateGrammar[2+2] = "";
	char stateType[10+9+5] = "";

	char allEmeralds[1+2+2] = "";
	char emeraldComma[1+2] = "";
	char emeraldGrammar[1+1+1] = "";
	char emeraldMeme[3+5+12+7] = "";

	char lifeType[9+10+2+7] = "";
	char lifeGrammar[9+10+2+3+4] = "";

	char spectatorType[9+10] = "";
	char spectatorGrammar[2+3] = "";

	char gametypeGrammar[2+3+1+9] = "";
	char gameType[2+3+8+9+25+12] = "";

	char addonsLoaded[3+2+9+8+7+5] = "";

	char charImageType[2+2+1] = "";
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

	INT32 i = 0;					// General Iterator

	INT32 checkSkin = 0; 			// Checks Through The Consoleplayer's Skin
	INT32 checkSuperSkin = 0;		// Checks Through The Consoleplayer's Super Skin
	INT32 checkSideSkin = 0; 		// Checks Through The Secondary Display Player's Skin
	INT32 checkSuperSideSkin = 0;	// Checks Through The Secondary Display Player's Super Skin

	gamedata_t *data = serverGamedata; // Proper Gamedata Pointer, Made by Bitten
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer]; // Obvious

	/* FALLBACK/BASIC RICH PRESENCE

		Since The User Doesn't Want To Show Their Status, or Since They're Using the DEVELOP Flag,
			This Just Shows That They're Playing SRB2, Along With a Few Extra Predetermined Strings
			( Thanks to Star :) ).

		(If that's too much, then they should just disable game activity :V)
	*/
	if (dedicated)
		return;
	else if (!cv_discordrp.value)
	{
		snprintf(imagestr, 128, "misctitle");
		snprintf(imagetxtstr, 128, "Sonic Robo Blast 2");

		snprintf(detailstr, 128, "In Game");
		if (paused)
			snprintf(statestr, 128, "Currently Paused");
		else if (menuactive || !Playing())
			snprintf(statestr, 128, "In The Menu");
		else
			snprintf(statestr, 128, "Actively Playing");

		goto pushPresence;
	}
	else if (devmode)
	{
		snprintf(imagestr, 128, "mapcustom");
		snprintf(imagetxtstr, 128, "Hey! No Peeking!");

		snprintf(detailstr, 128, "Developing a Masterpiece");
		snprintf(statestr, 128, "Keep your Eyes Peeled!");

		goto pushPresence;
	}

	// Servers //
	if (netgame)
	{
		if (DRPC_InvitesAreAllowed() == true)
		{
			const char *join;

			// Grab the host's IP for joining.
			if ((join = DRPC_GetServerIP()) != NULL)
			{
				discordPresence.joinSecret = DRPC_XORIPString(join);
				joinSecretSet = true;
			}
			else
				return;
		}

		if (server)
			detailstr = "Hosting ";
		else if (Playing() && !playeringame[consoleplayer])
			detailstr = "Looking for ";

		switch (discordInfo.serverRoom)
		{
			case 33: strlcat(detailstr, "Standard", 128); break;
			case 28: strlcat(detailstr, "Casual", 128); break;
			case 38: strlcat(detailstr, "Custom Gametype", 128); break;
			case 31: strlcat(detailstr, "OLDC", 128); break;

			case 0: strlcat(detailstr, "Public", 128); break;
			case -1: strlcat(detailstr, "Private", 128); break;

			default: strlcat(detailstr, "Unknown", 128); break;
		}

		discordPresence.partyId	= server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
		discordPresence.partySize = D_NumPlayers(); // Players in server
		discordPresence.partyMax = discordInfo.maxPlayers; // Max players
		discordPresence.instance = 1; // Net instance
	}
	else
	{
		memset(&discordInfo.maxPlayers, 0, sizeof(discordInfo.maxPlayers));
		memset(&discordInfo.joinsAllowed, 0, sizeof(discordInfo.joinsAllowed));
		memset(&discordInfo.whoCanInvite, 0, sizeof(discordInfo.whoCanInvite));
	}

	switch (cv_discordshowonstatus.value)
	{
		case 1:
		case 2:
		case 3:
			DRPC_EmeraldStatus(!cv_discordshowonstatus.value ? detailstr : statestr);
			break;

		case 4:
			DRPC_EmblemStatus(!netgame ? detailstr : statestr);
			break;

		case 5:
		case 6:

		case 7:
			DRPC_PlaytimeStatus((Playing() && !netgame) ? detailstr : statestr);
			break;

		case 8:
			goto customStatus;

		default:
		{
			if (!(netgame || splitscreen))
				DRPC_EmblemStatus(!netgame ? detailstr : statestr);
			DRPC_EmeraldStatus(!cv_discordshowonstatus.value ? detailstr : statestr);
		}
	}

	// Main Statuses //
	// Image Text
	if ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK) || ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK) && (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5)) || (cv_discordshowonstatus.value >= 2 && cv_discordshowonstatus.value != 5))
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

	// Status Text
	if ((playeringame[consoleplayer] && !demoplayback) || cv_discordshowonstatus.value == 7)
	{
		// Score
		if (cv_discordshowonstatus.value == 2)
			strlcat((!netgame ? detailstr : statestr), va("Current Score: %d", players[consoleplayer].score), 130);
			
		// SRB2 Playtime
		if (cv_discordshowonstatus.value == 7)
			
		
		// Misc.
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
		if (((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing() && playeringame[consoleplayer]) || (paused || menuactive || TSoURDt3rd->jukebox.musicPlaying))
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
					snprintf(lifeGrammar, 22, (!players[consoleplayer].lives ? ", Game Over..." : ((players[consoleplayer].lives == INFLIVES) || (!cv_cooplives.value && (netgame || multiplayer))) ? ", Has Infinite Lives" : (players[consoleplayer].lives == 1 ? ", %d Life Left" : ", %d Lives Left")), players[consoleplayer].lives);
				else if (timeover)
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
			if (paused || menuactive || TSoURDt3rd->jukebox.musicPlaying)
			{
				if (!cv_discordshowonstatus.value || (cv_discordshowonstatus.value == 6 && Playing()) || !Playing())
					strcpy(stateGrammar, ", ");

				snprintf(stateType, 27, (paused ? "%sCurrently Paused" : (menuactive ? "%sScrolling Through Menus" : "")), stateGrammar);
				strlcat(stateType, (TSoURDt3rd->jukebox.musicPlaying ? va("%sPlaying '%s' in the Jukebox", stateGrammar, TSoURDt3rd->jukebox.musicName) : ""), 95);
			}
			
			// Copy All Of Our Strings //
			strlcat(statestr, va("%s%s%s%s%s%s", gametypeGrammar, gameType, addonsLoaded, lifeGrammar, lifeType, stateType), 130);
		}
	}

	////// 	  MAPS 	 //////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 5)
	{
		// Scene Info //
		if (gamestate == GS_EVALUATION || gamestate == GS_GAMEEND || gamestate == GS_INTRO || gamestate == GS_CUTSCENE || gamestate == GS_CREDITS || gamestate == GS_ENDING || gamestate == GS_CONTINUING)
		{
			strcpy(imagestr, (gamestate == GS_INTRO ? "miscintro1" : "misctitle"));
			strcpy(imagetxtstr, (gamestate == GS_INTRO ? "Intro" : "Sonic Robo Blast 2"));
			
			snprintf(statestr, 130,
							// No Ultimate Mode
							(!ultimatemode ?
								(gamestate == GS_EVALUATION ? "Evaluating Results" :
								(gamestate == GS_CONTINUING ? "On the Continue Screen" :
								(gamestate == GS_CREDITS ? "Viewing the Credits" :
								(gamestate == GS_ENDING ? "Watching the Ending" :
								(gamestate == GS_GAMEEND ? (!cv_discordstatusmemes.value ? "Returning to the Main Menu..." : "Did You Get All Those Chaos Emeralds?") :
								(gamestate == GS_INTRO ? "Watching the Intro" :
								(gamestate == GS_CUTSCENE ? "Watching a Cutscene" : "???"))))))) :
								
							// Ultimate Mode
							(!cv_discordstatusmemes.value ? "Just Beat Ultimate Mode!" : "Look Guys, It's my Greatest Achievement: An SRB2 Discord RPC Status Saying I Beat Ultimate Mode!")));
		}

		// Map Info //
		else if (gamestate == GS_LEVEL || gamestate == GS_INTERMISSION || (gamestate == GS_TITLESCREEN || titlemapinaction))
		{
			// Map Images (Now With More Specificness)
			if ((gamemap >= 1 && gamemap <= 16) 		// Supported Co-op Maps (GFZ-RVZ1)
				|| (gamemap >= 22 && gamemap <= 23) 	// Supported Co-op Maps (ERZ1-ERZ2)
				|| (gamemap >= 25 && gamemap <= 27) 	// Supported Co-op Maps (BCZ1-BCZ3)

				|| (gamemap >= 30 && gamemap <= 33) 	// Supported Extra Maps
				|| (gamemap >= 40 && gamemap <= 42) 	// Supported Advanced Maps

				|| (gamemap >= 50 && gamemap <= 57) 	// Supported Singleplayer NiGHTS Stages
				|| (gamemap >= 60 && gamemap <= 66) 	// Supported Co-op Special Stages
				|| (gamemap >= 70 && gamemap <= 73) 	// Supported Bonus NiGHTS Stages

				|| (gamemap >= 280 && gamemap <= 288) 	// Supported CTF Maps
				|| (gamemap >= 532 && gamemap <= 543) 	// Supported Match Maps

				|| (tutorialmode))						// Tutorial Zone
			{
				strcpy(mapimg, va("%s", G_BuildMapName(gamemap)));
				strlwr(mapimg);
				strcpy(imagestr, mapimg);
			}
			else if ((gamemap == titlemap) && (gamestate != GS_TITLESCREEN && !titlemapinaction) && Playing())
			{
				strcpy(imagestr, "misctitle");
				strcpy(imagetxtstr, "What is Wrong With You.");
			}
			else
				strcpy(imagestr, "mapcustom");
			
			// Map Names
			if (mapheaderinfo[gamemap-1]->menuflags & LF2_HIDEINMENU)
				strcpy(imagetxtstr, "???");
			else
			{
				// List the Name
				char *maptitle = G_BuildMapTitle(gamemap);
				snprintf(mapname, 48, ((gamestate != GS_TITLESCREEN && !titlemapinaction) ? "%s" : "Title Screen"), ((gamestate != GS_TITLESCREEN && !titlemapinaction) ? maptitle : 0));
				Z_Free(maptitle);

				// Display Some Text Under The Map's Name
				strcpy(imagetxtstr, mapname);
				
				// Display the Map's Name on our Status, Since That's What We Set
				if (cv_discordshowonstatus.value == 5)
					strcpy(statestr, mapname);

				// Display The Title Screen Images, If We're on That
				if (gamestate == GS_TITLESCREEN || titlemapinaction)
					strcpy(imagestr, "misctitle");
			}

			// Time //
#if 0
			if (Playing() && (playeringame[consoleplayer] || paused || menuactive))
#else
			if (gamestate == GS_LEVEL && Playing())
#endif
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
		}
	}

	////// 	  CHARACTERS 	 //////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 1)
	{
		///// Character Types /////
		strcpy(charImageType, (!cv_discordcharacterimagetype.value ? "char" : (cv_discordcharacterimagetype.value == 1 ? "cont" : "life")));
		
		strcpy(charimg, va("%scustom", charImageType));
		((cv_discordshowonstatus.value && ((playeringame[1] && players[1].bot) || splitscreen)) ? strcpy(charimgS, va("%scustom", charImageType)) : 0);

		///// Renderers /////
		if (Playing())
		{
			//// Supported Character Images ////
			/// Main Player ///
			while (supportedSkins[checkSkin] != NULL)
			{
				/// Supers ///
				if (players[consoleplayer].powers[pw_super])
				{
					while (supportedSuperSkins[checkSuperSkin] != NULL)
					{
						// Dynamic Duos //
						// Sonic & Tails!
						if (((strcmp(skins[players[consoleplayer].skin].name, "sonic") == 0) || (strcmp(skins[players[consoleplayer].skin].name, "supersonic") == 0))
							&& ((playeringame[1] && players[1].bot) && strcmp(skins[players[1].skin].name, "tails") == 0))
						{
							(!cv_discordshowonstatus.value ?
								// With the Default Show Status Option
								strcpy(charimg, va("%ssupersonictails", charImageType)) :
								
								// Withe the Only Characters Show Status Option
								(strcpy(charimg, va("%ssupersonic", charImageType)), strcpy(charimgS, va("%stails", charImageType))));
							break;
						}
						
						// Others //
						if (strcmp(skins[players[consoleplayer].skin].name, supportedSuperSkins[checkSuperSkin]) == 0)
						{
							snprintf(charimg, 27, "%s%s", charImageType, (fastncmp(supportedSuperSkins[checkSuperSkin], "super", 5) ? supportedSuperSkins[checkSuperSkin] : va("super%s", supportedSuperSkins[checkSuperSkin])));
							break;
						}

						checkSuperSkin++;
					}
					if (supportedSuperSkins[checkSuperSkin] != NULL)
						break;
				}

				// Dynamic Duos //
				// Sonic & Tails!
				if ((strcmp(skins[players[consoleplayer].skin].name, "sonic") == 0) &&
					((playeringame[1] && players[1].bot) && strcmp(skins[players[1].skin].name, "tails") == 0))
				{
					(!cv_discordshowonstatus.value ?
						// With the Default Show Status Option
						strcpy(charimg, va("%ssonictails", charImageType)) :
						
						// Withe the Only Characters Show Status Option
						(strcpy(charimg, va("%ssonic", charImageType)), strcpy(charimgS, va("%stails", charImageType))));
					break;
				}

				// Others! //
				if (strcmp(skins[players[consoleplayer].skin].name, supportedSkins[checkSkin]) == 0)
				{
					strcpy(charimg, va("%s%s", charImageType, supportedSkins[checkSkin]));	
					break;
				}

				checkSkin++;
			}
			
			/// Side Player ///
			if (cv_discordshowonstatus.value && ((playeringame[1] && players[1].bot) || (splitscreen)))
			{
				while (supportedSkins[checkSideSkin] != NULL)
				{
					/// Supers ///
					if (players[1].powers[pw_super])
					{
						while (supportedSuperSkins[checkSuperSideSkin] != NULL)
						{
							// Others //
							if (strcmp(skins[players[1].skin].name, supportedSuperSkins[checkSuperSideSkin]) == 0)
							{
								snprintf(charimgS, 27, "%s%s", charImageType, (fastncmp(supportedSuperSkins[checkSuperSideSkin], "super", 5) ? supportedSuperSkins[checkSuperSideSkin] : va("super%s", supportedSuperSkins[checkSuperSideSkin])));
								break;
							}

							checkSuperSideSkin++;
						}
						if (supportedSuperSkins[checkSuperSideSkin] != NULL)
							break;
					}

					/// Others ///
					if (strcmp(skins[players[1].skin].name, supportedSkins[checkSideSkin]) == 0)
					{	
						strcpy(charimgS, va("%s%s", charImageType, supportedSkins[checkSideSkin]));
						break;
					}

					checkSideSkin++;
				}
			}
			
			//// Strings ////
			if (playeringame[consoleplayer])
			{
				// Display Character Names //
				if (!splitscreen)
					(!(playeringame[1] && players[1].bot) ?
						// No Bots; Default Character Status String
						(snprintf(charname, 75, "Playing As: %s",
							(players[consoleplayer].powers[pw_super] ? (fastncmp(skins[players[consoleplayer].skin].realname, "Super ", 6) ? skins[players[consoleplayer].skin].realname : va("Super %s", skins[players[consoleplayer].skin].realname)) : skins[players[consoleplayer].skin].realname))) :
						
						// One Bot, Default Status Option
						(!cv_discordshowonstatus.value ?
							snprintf(charname, 75, "Playing As: %s & %s",
								(players[consoleplayer].powers[pw_super] ? (fastncmp(skins[players[consoleplayer].skin].realname, "Super ", 6) ? skins[players[consoleplayer].skin].realname : va("Super %s", skins[players[consoleplayer].skin].realname)) : skins[players[consoleplayer].skin].realname),
									(players[1].powers[pw_super] ? (fastncmp(skins[players[1].skin].realname, "Super ", 6) ? skins[players[1].skin].realname : va("Super %s", skins[players[1].skin].realname)) : skins[players[1].skin].realname)) :
						
						// One Bot, Only Characters Status Option
						(snprintf(charname, 75, "Playing As: %s",
							((players[consoleplayer].powers[pw_super] ? (fastncmp(skins[players[consoleplayer].skin].realname, "Super ", 6) ? skins[players[consoleplayer].skin].realname : va("Super %s", skins[players[consoleplayer].skin].realname)) : skins[players[consoleplayer].skin].realname))),
						
						snprintf(charnameS, 75, "& %s",
							((players[1].powers[pw_super] ? (fastncmp(skins[players[1].skin].realname, "Super ", 6) ? skins[players[1].skin].realname : va("Super %s", skins[players[1].skin].realname)) : skins[players[1].skin].realname))))));
				else
					// The Secondary Display Player, Default Status Option
					(!cv_discordshowonstatus.value ?
						snprintf(charname, 75, "%s & %s", player_names[consoleplayer], player_names[1]) :
						
						// The Secondary Display Player, Default Status Option
						(snprintf(charname, 75, "%s", player_names[consoleplayer]), snprintf(charnameS, 75, "%s", player_names[1])));
				
				// Apply Character Images and Names //
				(!cv_discordshowonstatus.value ? strcpy(simagetxtstr, charname) : (strcpy(imagetxtstr, charname), strcpy(simagetxtstr, charnameS))); // Character Names, And Bot Names, If They Exist
				(!cv_discordshowonstatus.value ? strcpy(simagestr, charimg) : (strcpy(imagestr, charimg), strcpy(simagestr, charimgS))); // Character images			
				
				// Also Set it On Their Status, Since They Set it To Be That Way //
				if (cv_discordshowonstatus.value)
					strcpy(statestr,
							// Split-Screen //
							(strcmp(charnameS, "") != 0 ? (splitscreen ? va("%s & %s", charname, charnameS) :
							
							// No Split-Screen //
							// Bots
							((playeringame[2] && players[2].bot) ?
								(!(playeringame[3] && players[3].bot) ?
									// Three Bots
									va("%s %s & %s", charname, charnameS, 
										(players[2].powers[pw_super] ? (fastncmp(skins[players[2].skin].realname, "Super ", 6) ? skins[players[2].skin].realname : va("Super %s", skins[players[2].skin].realname)) : skins[players[2].skin].realname)) :
							
									// More Than Three Bots
									va("%s %s & %s With Multiple Bots", charname, charnameS,
										(players[2].powers[pw_super] ? (fastncmp(skins[players[2].skin].realname, "Super ", 6) ? skins[players[2].skin].realname : va("Super %s", skins[players[2].skin].realname)) : skins[players[2].skin].realname))) :
								
								// Two Bots
								va("%s %s", charname, charnameS))) : 
							
							// No Bots
							charname));
			}
		}
	}
	
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
		discordPresence.details			= detailstr;
		discordPresence.state			= statestr;

		discordPresence.smallImageKey	= simagestr;
		discordPresence.smallImageText	= simagetxtstr;

		discordPresence.largeImageKey	= imagestr;
		discordPresence.largeImageText	= imagetxtstr;

		if (joinSecretSet == false)
		{
			// Not able to join? Flush the request list, if it exists.
			DRPC_EmptyRequests();
		}
		Discord_UpdatePresence(&discordPresence);
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

	DRPC_UpdatePresence();
	DRPC_EmptyRequests();

#ifdef DISCORD_DISABLE_IO_THREAD
	Discord_UpdateConnection();
#endif
	Discord_RunCallbacks();

	Discord_ClearPresence();
	Discord_Shutdown();

	I_OutputMsg("DRPC_Shutdown(): shut down\n");
}

#endif // HAVE_DISCORDRPC
