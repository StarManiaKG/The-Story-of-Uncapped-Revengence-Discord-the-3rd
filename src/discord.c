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

#ifdef HAVE_DISCORDRPC // HAVE_DISCORDRPC

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
#include "d_player.h" // INFLIVES

#include "discord.h"
#include "doomdef.h"

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID "1013126566236135516"

// length of IP strings
#define IP_SIZE 21

static CV_PossibleValue_t discordstatustype_cons_t[] = {
    {0, "All"},
    {1, "Only Characters"},
    {2, "Only Score"},
    {3, "Only Emeralds"},
    {4, "Only Emblems"},
    {5, "Only Levels"},
    {6, "Only Gametype"},
    {7, "Only Playtime"},
    {8, "Custom"},
    {0, NULL}};

// Custom Discord Status Image Type //
static CV_PossibleValue_t customlargeimagetype_cons_t[] = {{0, "Characters"}, {1, "Maps"}, {2, "Miscellaneous"}, {3, "None"}, {0, NULL}};
static CV_PossibleValue_t customsmallimagetype_cons_t[] = {{0, "Characters"}, {1, "Maps"}, {2, "Miscellaneous"}, {3, "None"}, {0, NULL}};
static CV_PossibleValue_t customcharacterimage_cons_t[] = { // Characters //
    // Vanilla Chars
    {0, "Default"}, //does ghost sonic count as a vanilla char? maybe.
    {1, "Sonic"},
    {2, "Tails"},
    {3, "Knuckles"},
    {4, "Amy"},
    {5, "Fang"},
    {6, "Metal Sonic"},
    {7, "Sonic & Tails"}, //Bots, am i right?
    //Custom Chars
    {8, "Adventure Sonic"},
    {9, "Shadow"},
    {10, "Skip"},
    {11, "Jana"},
    {12, "Surge"},
    {13, "Cacee"},
    {14, "Milne"},
    {15, "Maiamy"},
    {16, "Mario"},
    {17, "Luigi"},
    {18, "Blaze"},
    {19, "Marine"},
    {20, "Tails Doll"},
    {21, "Metal Knuckles"},
    {22, "Smiles"},
    {23, "Whisper"},
    {24, "Hexhog"},
    {0, NULL}};

static CV_PossibleValue_t custommapimage_cons_t[] = { // Maps //
    // Singleplayer/Co-op Maps
    {0, "01"},
    {1, "02"},
    {2, "03"},
    {3, "04"},
    {4, "05"},
    {5, "06"},
    {6, "07"},
    {7, "08"},
    {8, "09"},
    {9, "10"},
    {10, "11"},
    {11, "12"},
    {12, "13"},
    {13, "14"},
    {14, "15"},
    {15, "16"},
    {16, "22"},
    {17, "23"},
    {18, "25"},
    {19, "26"},
    {20, "27"},
    // Extra Maps
    {21, "30"},
    {22, "31"},
    {23, "32"},
    {24, "33"},
    // Advanced Maps
    {25, "40"},
    {26, "41"},
    {27, "42"},
    // Singleplayer Special Stages
    {28, "50"},
    {29, "51"},
    {30, "52"},
    {31, "53"},
    {32, "54"},
    {33, "55"},
    {34, "56"},
    {35, "57"},
    // Co-op Special Stages
    {36, "60"},
    {37, "61"},
    {38, "62"},
    {39, "63"},
    {40, "64"},
    {41, "65"},
    {42, "66"},
    // Other Things I Probably Forgot Because I'm Smart lol
    {43, "70"},
    {44, "71"},
    {45, "72"},
    {46, "73"},
    // Match/Team Match/H&S/Tag Maps
    {47, "f0"},
    {48, "f1"},
    {49, "f2"},
    {50, "f3"},
    {51, "f4"},
    {52, "f5"},
    {53, "f6"},
    {54, "f7"},
    {55, "f8"},
    // CTF Maps
    {56, "m0"},
    {57, "m1"},
    {58, "m2"},
    {59, "m3"},
    {60, "m4"},
    {61, "m5"},
    {62, "m6"},
    {63, "m7"},
    {64, "m8"},
    {65, "m9"},
    {66, "ma"},
    {67, "mb"},
    // Tutorial Map
    {68, "z0"},
    // Custom Map
    {69, "Custom"},
    {0, NULL}};

// Miscellanious //
static CV_PossibleValue_t custommiscimage_cons_t[] = {{0, "Default"}, {0, NULL}};

                                                ////////////////////////////
                                                //    Discord Commands    //
                                                ////////////////////////////
consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, Discordcustomstatus_option_Onchange);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DRPC_UpdatePresence);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DRPC_UpdatePresence);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "All", CV_SAVE|CV_CALL, discordstatustype_cons_t, Discordcustomstatus_option_Onchange);
//// Custom Discord Status Things ////
consvar_t cv_customdiscorddetails = CVAR_INIT ("customdiscorddetails", "I'm Feeling Good!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
consvar_t cv_customdiscordstate = CVAR_INIT ("customdiscordstate", "I'm Playing Sonic Robo Blast 2!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
// Custom Discord Status Image Type
consvar_t cv_customdiscordlargeimagetype = CVAR_INIT ("customdiscordlargeimagetype", "Characters", CV_SAVE|CV_CALL, customlargeimagetype_cons_t, Discordcustomstatus_option_Onchange);
consvar_t cv_customdiscordsmallimagetype = CVAR_INIT ("customdiscordsmallimagetype", "Maps", CV_SAVE|CV_CALL, customsmallimagetype_cons_t, Discordcustomstatus_option_Onchange);
// Custom Discord Status Images
    // Characters //
consvar_t cv_customdiscordlargecharacterimage = CVAR_INIT ("customdiscordlargecharacterimage", "Sonic", CV_SAVE|CV_CALL, customcharacterimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallcharacterimage = CVAR_INIT ("customdiscordsmallimage", "Tails", CV_SAVE|CV_CALL, customcharacterimage_cons_t, DRPC_UpdatePresence);
    // Maps //
consvar_t cv_customdiscordlargemapimage = CVAR_INIT ("customdiscordlargemapimage", "01", CV_SAVE|CV_CALL, custommapimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallmapimage = CVAR_INIT ("customdiscordsmallmapimage", "02", CV_SAVE|CV_CALL, custommapimage_cons_t, DRPC_UpdatePresence);
    // Miscellanious //
consvar_t cv_customdiscordlargemiscimage = CVAR_INIT ("customdiscordlargemiscimage", "Default", CV_SAVE|CV_CALL, custommiscimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallmiscimage = CVAR_INIT ("customdiscordsmallmiscimage", "Default", CV_SAVE|CV_CALL, custommiscimage_cons_t, DRPC_UpdatePresence);
    // Captions //
consvar_t cv_customdiscordlargeimagetext = CVAR_INIT ("customdiscordlargeimagetext", "This is my Favorite Character!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallimagetext = CVAR_INIT ("customdiscordsmallimagetext", "This is also my Favorite Character!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
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
		CONS_Printf("Discord: connected to %s\n", user->username);
	else
		CONS_Printf("Discord: connected to %s#%s (%s)\n", user->username, user->discriminator, user->userId);
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

	char servertype[15+10];
	char servertag[11+26+15+8];

	//nerd emoji moment
	char allEmeralds[1+2+2] = "";
	char emeraldComma[1+2] = "";
	char emeraldGrammar[1+1+1] = "";
	char emeraldMeme[2+5+12+7] = "";

	char lifetype[9+10+2+7] = "";
	char lifeplural[9+10+2+7] = "";

	char spectatortype[9+10] = "";
	char spectatorgrammar[2+3] = "";

	INT32 i;

	//Tiny Emeralds Counter
	UINT8 emeraldCount = 0;

	boolean joinSecretSet = false;

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	//// NO STATUS? ////
	if (!cv_discordrp.value)
	{
		// User doesn't want to show their game information, so update with empty presence.
		// This just shows that they're playing SRB2. (If that's too much, then they should disable game activity :V)
		discordPresence.largeImageKey = "misctitle";
		discordPresence.largeImageText = "Sonic Robo Blast 2";
		if (paused) //look, if you wanna be a whiny baby about it, just let me know
			discordPresence.details = "Currently Paused";
		DRPC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

#ifdef DEVELOP
	// This way, we can use the invite feature in-dev, but not have snoopers seeing any potential secrets! :P
	discordPresence.largeImageKey = "miscdevelop";
	discordPresence.largeImageText = "No peeking!";
	discordPresence.state = "Developing a Masterpiece!";

	DRPC_EmptyRequests();
	Discord_UpdatePresence(&discordPresence);
	return;
#endif // DEVELOP

	////////////////////////////////////////////
	////   Main Rich Presence Status Info   ////
	////////////////////////////////////////////

	//// Server Info ////
	if (dedicated || netgame)
	{
	    // Reset discord info if you're not in a place that uses it!
        // Important for if you join a server that compiled without HAVE_DISCORDRPC,
        // so that you don't ever end up using bad information from another server.
        memset(&discordInfo, 0, sizeof(discordInfo));

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

		switch (ms_RoomId)
		{
			case 33: snprintf(servertype, 26, "Standard"); break;
			case 28: snprintf(servertype, 26, "Casual"); break;
			case 38: snprintf(servertype, 26, "Custom Gametypes"); break;
			case 31: snprintf(servertype, 26, "OLDC"); break;
			case -1: default: snprintf(servertype, 26, "Private"); break; // Private server
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

		//// Set Status Picture (Just in Case) ////
		if (cv_discordshowonstatus.value != 0 && cv_discordshowonstatus.value != 8)
		{
			if (((cv_discordshowonstatus.value == 1 || cv_discordshowonstatus.value == 5) && !Playing()) || (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5))
			{
				discordPresence.largeImageKey = "misctitle";
				discordPresence.largeImageText = "Sonic Robo Blast 2";
			}
		}

		if (playeringame[consoleplayer])
		{
			//// Emblems ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 4)
				snprintf(detailstr, 20, "%d/%d Emblems", M_CountEmblems(), (numemblems + numextraemblems));
			
			//// Emeralds ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 3)
			{
				for (i = 0; i < 7; i++) // thanks Monster Iestyn for this math
					if (emeralds & (1<<i))
						emeraldCount += 1;
				
				if (emeraldCount == 7)
					snprintf(allEmeralds, 5, "All ");

				if (emeraldCount > 1 || emeraldCount == 0) //Mystic Power Gang
				{
					snprintf(emeraldGrammar, 2, "s"); //i think you know what the joke here is
					if (cv_discordstatusmemes.value) //Honestly relatable lol
					{
						if (emeraldCount == 3 || emeraldCount == 4)
							snprintf(emeraldGrammar, 3, "s;");
						else if (!emeralds) //Man, Special Stage Got Hands
							snprintf(emeraldGrammar, 3, "s?"); //there's a special stage token right at the BEGINNING OF GFZ1 HOW DO YOU NOT HAVE A EMERALD YET
						
						// Fun Fact: the subtitles in Shadow the Hedgehog emphasized "fourth", even though Jason Griffith emphasized "damn" in this sentence
						if (emeraldCount == 3)
							snprintf(emeraldMeme, 26, " Where's That DAMN FOURTH");
						else if (emeraldCount == 4)
							snprintf(emeraldMeme, 25, " Found That DAMN FOURTH");
					}
				}

				if (!cv_discordshowonstatus.value)
					snprintf(emeraldComma, 3, ", ");

				strlcat(detailstr, va("%s%s%d Emerald%s%s", emeraldComma, allEmeralds, emeraldCount, emeraldGrammar, emeraldMeme), 64);
			}

			//// Score ////
			if (cv_discordshowonstatus.value == 2)
				snprintf(detailstr, 25, "Current Score: %d", players[consoleplayer].score);
			
			//// SRB2 Playtime ////
			if (cv_discordshowonstatus.value == 7)
				snprintf(detailstr, 60, "Total Playtime: %d hours, %d minutes, %d seconds", G_TicsToHours(totalplaytime), G_TicsToMinutes(totalplaytime, false), G_TicsToSeconds(totalplaytime));

			//// Apply our Status, And We're Done :) ////
			discordPresence.details = detailstr;
		}
		
		if (demoplayback && !titledemo)
		{
			snprintf(statestr, 25, "Watching Replays");
			discordPresence.state = statestr;
		}
		else
		{
			if (!Playing())
			{
				discordPresence.largeImageKey = "misctitle";
				if (!cv_discordshowonstatus.value)
				{
					discordPresence.largeImageText = "Title Screen";
					snprintf(statestr, 18, "Main Menu");
					discordPresence.state = statestr;
				}
				else
					discordPresence.largeImageText = "Sonic Robo Blast 2";
			}
		}
	}

	//// Gametypes ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6)
	{
		// You Already Know The Deal.
		memset(&discordInfo, 0, sizeof(discordInfo));

		if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing())
		{
			if (modeattacking)
			{
				if ((maptol != TOL_NIGHTS && maptol != TOL_XMAS))
					snprintf(statestr, 65, "Playing Time Attack");
				else
					snprintf(statestr, 65, "Playing NiGHTS Mode");
			}
			else
			{	
				if (!splitscreen) 
				{
					if ((gametype == GT_COOP) && (!netgame))
					{
						if (!ultimatemode)
							snprintf(statestr, 65, "Playing Single-Player");
						else
							snprintf(statestr, 65, "Taking on Ultimate Mode");
					}
					else
						snprintf(statestr, 65, "Playing %s", gametype_cons_t[gametype].strvalue);
				}
				else
					snprintf(statestr, 65, "Playing Split-Screen");
				
				if (players[consoleplayer].lives && !players[consoleplayer].spectator && gametyperules & GTR_LIVES && !ultimatemode)
				{
					if (players[consoleplayer].lives == 1)
						snprintf(lifeplural, 9, ", %d Life", players[consoleplayer].lives);
					else if (players[consoleplayer].lives > 1)
						snprintf(lifeplural, 12, ", %d Lives", players[consoleplayer].lives);
					
					if (players[consoleplayer].lives >= 1)
						snprintf(lifetype, 7, " Left");
					else if (players[consoleplayer].lives == INFLIVES)
						snprintf(lifeplural, 22, "Has Infinite Lives");
				}
				else if (!players[consoleplayer].spectator && !players[consoleplayer].lives)
					snprintf(lifeplural, 15, ", Game Over...");
				else if (playeringame[consoleplayer])
				{
					if (!players[consoleplayer].spectator)
					{
						if ((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer != consoleplayer)))
							snprintf(spectatorgrammar, 4, "ing");
						else
							snprintf(spectatorgrammar, 3, "er");
						
						snprintf(spectatortype, 21, ", View%s", spectatorgrammar);
					}
					else
					{
						if ((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer == consoleplayer)))
							snprintf(spectatorgrammar, 4, "ing");
						else
							snprintf(spectatorgrammar, 3, "or");
						
						snprintf(spectatortype, 21, "Spectat%s", spectatorgrammar);
					}

					if (!players[consoleplayer].spectator || players[consoleplayer].spectator)
					{
						if (players[consoleplayer].spectator)
							snprintf(lifeplural, 12, ", Dead & ");

						if (displayplayer != consoleplayer)
							snprintf(lifetype, 30, "%s %s", spectatortype, player_names[displayplayer]);
						else if ((displayplayer == consoleplayer) && players[consoleplayer].spectator)
						{
							if (!cv_discordstatusmemes.value)
								snprintf(lifetype, 27, "In %s Mode", spectatortype);
							else
								snprintf(lifetype, 23, "%s Air", spectatortype);
						}
					}
				}
				strlcat(statestr, va("%s%s", lifeplural, lifetype), 65);
			}
			discordPresence.state = statestr;
		}
	}
	
	// You Should Be Able To Set Whether You're Paused Or Not, In My Opinion, No Matter What.
	if (paused)
	{
		strlcat(statestr, ", Currently Paused", 17);
		discordPresence.state = statestr;
	}

	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 5)
	{
		// You've Got It Down-Pact Already.
		memset(&discordInfo, 0, sizeof(discordInfo));

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
			else
			{
				if (gamestate != GS_TITLESCREEN && !titlemapinaction) //tiny null fix
					discordPresence.largeImageKey = "mapcustom";
			}
			
			if (mapheaderinfo[gamemap - 1]->menuflags & LF2_HIDEINMENU)
				// Hell map, hide the name
				discordPresence.largeImageText = "Map: ???";
			else
			{
				// Map name on tool tip
				if (gamemap != 99 && gamestate != GS_TITLESCREEN && !titlemapinaction)
					snprintf(mapname, 48, "%s", G_BuildMapTitle(gamemap));
				//fully fixes null map issue
				else
				{
					snprintf(mapname, 48, "Title Screen");
					discordPresence.largeImageKey = "misctitle";
				}

				discordPresence.largeImageText = mapname;
			}

			if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing())
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
		else if (gamestate == GS_EVALUATION || gamestate == GS_GAMEEND || gamestate == GS_CREDITS || gamestate == GS_ENDING|| gamestate == GS_CONTINUING)
		{	
			discordPresence.largeImageKey = "misctitle";
				
			if (gamestate == GS_EVALUATION && !ultimatemode)
				discordPresence.largeImageText = "Evaluating Results";
			else if (gamestate == GS_CONTINUING) 
				discordPresence.largeImageText = "On the Continue Screen";

			if (ultimatemode)
			{	
				if (!cv_discordstatusmemes.value)
					discordPresence.details = "Just Beat Ultimate Mode!";
				else
					discordPresence.details = "Look, It's my Greatest Achievement: An Ultimate Mode Complete Discord RPC Status";
			}
		}
	}

	//// Characters ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 1)
	{
		//just in case
		memset(&discordInfo, 0, sizeof(discordInfo));

		if (Playing() && playeringame[consoleplayer])
		{
			// Why Would You Split My Screen
			if (!splitscreen)
			{
				// Supported Skin Pictures
				static const char *supportedSkins[] = {
					"sonic",
					"tails",
					"knuckles",
					"amy",
					"fang",
					"metalsonic",
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
					"hexhog",
					NULL
				};

				INT32 MAXCUSTOMCHARS = 23;

				//// No Bots ////
				if (!players[1].bot || netgame)
				{
					for (i = 0; i < MAXCUSTOMCHARS; i++)
					{
						// Character Images
						snprintf(charimg, 11, "charcustom"); // Unsupported

						if (strcmp(skins[players[consoleplayer].skin].name, supportedSkins[i]) == 0)
						{
							snprintf(charimg, 32, "char%s", skins[players[consoleplayer].skin].name); // Supported
							break;
						}    
					}
					
					// Player Names
					if (!players[consoleplayer].spectator)
						snprintf(charname, 32, "Playing As: %s", skins[players[consoleplayer].skin].realname); // Character
				}
				//// Bots ////
				else if (players[1].bot)
				{
					// Let's set this at the top!
					for (i = 0; i < MAXCUSTOMCHARS; i++)
					{
						// Character Images
						snprintf(charimg, 11, "charcustom"); // Unsupported

						if ((strcmp(skins[players[consoleplayer].skin].name, "sonic") == 1) && (strcmp(skins[players[secondarydisplayplayer].skin].name, "tails") == 1)) // Let's make sure they aren't sonic and tails, just in case
						{
							if (strcmp(skins[players[consoleplayer].skin].name, supportedSkins[i]) == 0)
							{
								snprintf(charimg, 32, "char%s", skins[players[consoleplayer].skin].name); // Supported
								break;
							}
						}
					}

					// Only One Regular Bot?
					if (!players[2].bot)
					{
						// Character Tags
						if ((strcmp(skins[players[consoleplayer].skin].name, "sonic") == 0) && (strcmp(skins[players[secondarydisplayplayer].skin].name, "tails") == 0)) // Are They Sonic and Tails?
							snprintf(charimg, 15, "charsonictails"); // Put that Image on Then!

						snprintf(charname, 50, "Playing As: %s & %s", skins[players[consoleplayer].skin].realname, skins[players[secondarydisplayplayer].skin].realname);
					}
					// Multiple Bots?
					else
						snprintf(charname, 75, "Playing As: %s, %s, & Multiple Bots", skins[players[consoleplayer].skin].realname, skins[players[secondarydisplayplayer].skin].realname);
				}
			}
			//I Split my Screen
			else
			{
				// render player names and the character image
				snprintf(charname, 50, "%s & %s", player_names[consoleplayer], player_names[secondarydisplayplayer]);
				snprintf(charimg, 15, "charsonictails");
			}
			
			// Apply Character Images and Names
			discordPresence.smallImageKey = charimg; // Character image
			discordPresence.smallImageText = charname; // Character name, Bot name (if they exist)
		}
	}
	
	//// Custom Statuses ////
	if (cv_discordshowonstatus.value == 8)
    {
		//still just in case lol
		memset(&discordInfo, 0, sizeof(discordInfo));

	    char customlargeimagestring[32];
        char customsmallimagestring[32];

		discordPresence.details = cv_customdiscorddetails.string;
		discordPresence.state = cv_customdiscordstate.string;

        // Large Images
		if ((!cv_customdiscordlargeimagetype.value) || (cv_customdiscordlargeimagetype.value == 1) || (cv_customdiscordlargeimagetype.value == 2))
		{
            if (!cv_customdiscordlargeimagetype.value)
                snprintf(customlargeimagestring, 36, "char%s", cv_customdiscordlargecharacterimage.string);
            else if (cv_customdiscordlargeimagetype.value == 1)
                snprintf(customlargeimagestring, 36, "map%s", cv_customdiscordlargemapimage.string);
            else if (cv_customdiscordlargeimagetype.value == 2)
                snprintf(customlargeimagestring, 36, "misc%s", cv_customdiscordlargemiscimage.string);

            int nospaces = 0; //this helps us remove spaces from our string, if we have any
            for (i = 0; customlargeimagestring[i] != '\0'; i++) { //string writing, now capiable of removing spaces and forcing lowercases on letters, in limited small image edition
                if ((customlargeimagestring[i] != ' ') && (customlargeimagestring[i] != '&')) // do we not have any spaces?
                {
                    //continue with our normal behavior then!
                    customlargeimagestring[i] = tolower(customlargeimagestring[i]);
                    customlargeimagestring[nospaces] = customlargeimagestring[i];
                    nospaces++;
                }
            }
            customlargeimagestring[nospaces] = '\0';

            if (!cv_customdiscordlargeimagetype.value)
                discordPresence.largeImageKey = (cv_customdiscordlargecharacterimage.value > 0 ? customlargeimagestring : "charcustom");
            else if (cv_customdiscordlargeimagetype.value == 1)
                discordPresence.largeImageKey = (cv_customdiscordlargemapimage.value > 0 ? customlargeimagestring : "map01");
            else
                discordPresence.largeImageKey = (cv_customdiscordlargemiscimage.value > 0 ? customlargeimagestring : "misctitle");
            discordPresence.largeImageText = cv_customdiscordlargeimagetext.string;
        }

        // Small Images
		if ((!cv_customdiscordsmallimagetype.value) || (cv_customdiscordsmallimagetype.value == 1) || (cv_customdiscordsmallimagetype.value == 2))
		{
            if (!cv_customdiscordsmallimagetype.value)
                snprintf(customsmallimagestring, 32, "char%s", cv_customdiscordsmallcharacterimage.string);
            else if (cv_customdiscordsmallimagetype.value == 1)
                snprintf(customsmallimagestring, 32, "map%s", cv_customdiscordsmallmapimage.string);
            else if (cv_customdiscordsmallimagetype.value == 2)
                snprintf(customsmallimagestring, 32, "misc%s", cv_customdiscordsmallmiscimage.string);

            int nospaces = 0; //this helps us remove spaces from our string, if we have any
            for (i = 0; customsmallimagestring[i] != '\0'; i++) { //string writing, now capiable of removing spaces and forcing lowercases on letters, in limited small image edition
                if ((customsmallimagestring[i] != ' ') && (customsmallimagestring[i] != '&')) // do we not have any spaces?
                {
                    //continue with our normal behavior then!
                    customsmallimagestring[i] = tolower(customsmallimagestring[i]);
                    customsmallimagestring[nospaces] = customsmallimagestring[i];
                    nospaces++;
                }
            }
            customsmallimagestring[nospaces] = '\0';

            if (!cv_customdiscordsmallimagetype.value)
                discordPresence.smallImageKey = (cv_customdiscordsmallcharacterimage.value > 0 ? customsmallimagestring : "charcustom");
            else if (cv_customdiscordsmallimagetype.value == 1)
                discordPresence.smallImageKey = (cv_customdiscordsmallmapimage.value > 0 ? customsmallimagestring : "map01");
            else
                discordPresence.smallImageKey = (cv_customdiscordsmallmiscimage.value > 0 ? customsmallimagestring : "misctitle");
            discordPresence.smallImageText = cv_customdiscordsmallimagetext.string;
        }
	}

	if (!joinSecretSet)
		DRPC_EmptyRequests(); // Not able to join? Flush the request list, if it exists.

	Discord_UpdatePresence(&discordPresence);
}

/*--------------------------------------------------
	void DRPC_ShutDown(void)

		Clears Everything Related to Discord
		Rich Presence. Only Runs On Game Close
		or Crash.
--------------------------------------------------*/
void DRPC_ShutDown(void)
{
	DRPC_EmptyRequests();
	Discord_ClearPresence();
	Discord_Shutdown();
}

#endif // HAVE_DISCORDRPC