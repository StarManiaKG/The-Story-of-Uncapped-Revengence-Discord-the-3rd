// SONIC ROBO BLAST 2 //WITH DISCORD RPC BROUGHT TO YOU BY THE KART KREW (And Star lol)
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
#include "m_menu.h" // gametype_cons_t and discord custom string pointers and jukebox stuff and things like that
#include "r_things.h" // skins
#include "mserv.h" // ms_RoomId
#include "m_cond.h" // queries about emblems
#include "z_zone.h"
#include "byteptr.h"
#include "stun.h"
#include "i_tcp.h" // current_port
#include "discord.h" // duh
#include "doomdef.h"
#include "w_wad.h" // numwadfiles
#include "d_netfil.h" // nameonly
#include "doomstat.h" // savemoddata
#include "dehacked.h" // titlechanged

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID "1013126566236135516"

// length of IP strings
#define IP_SIZE 21

static CV_PossibleValue_t statustype_cons_t[] = {
    {0, "All"},
    {1, "Only Characters"},
    {2, "Only Score"},
    {3, "Only Emeralds"},
    {4, "Only Emblems"},
    {5, "Only Levels"},
    {6, "Only Statuses"},
    {7, "Only Playtime"},
    {8, "Custom"},
    {0, NULL}};
static CV_PossibleValue_t characterimagetype_cons_t[] = {{0, "CS Portrait"}, {1, "Continue Sprite"}, {0, NULL}}; //{2, "Life Icon Sprite"},

// Custom Discord Status Image Type //
static CV_PossibleValue_t customimagetype_cons_t[] = {
	{0, "CS Portraits"},
	{1, "Continue Sprites"},
	{2, "Maps"},
	{3, "Miscellaneous"},
	{4, "None"},
	{0, NULL}};
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
static CV_PossibleValue_t custommapimage_cons_t[] = {{0, "MIN"}, {69, "MAX"}, {0, NULL}};
static CV_PossibleValue_t custommiscimage_cons_t[] = { // Miscellanious //
	{0, "Default"},
	// Intro Stuff
	{1, "Intro 1"},
	{2, "Intro 2"},
	{3, "Intro 3"},
	{4, "Intro 4"},
	{5, "Intro 5"},
	{6, "Intro 6"},
	{7, "Intro 7"},
	{8, "Intro 8"},
	// Alternate Images
	{9, "Alt. Sonic Image 1"},
	{10, "Alt. Sonic Image 2"},
	{11, "Alt. Sonic Image 3"},
	{12, "Alt. Sonic Image 4"},
	{13, "Alt. Tails Image 1"},
	{14, "Alt. Tails Image 2"},
	{15, "Alt. Knuckles Image 1"},
	{16, "Alt. Knuckles Image 2"},
	{17, "Alt. Amy Image 1"},
	{18, "Alt. Fang Image 1"},
	{19, "Alt. Metal Sonic Image 1"},
	{20, "Alt. Metal Sonic Image 2"},
	{21, "Alt. Eggman Image 1"},
	{0, NULL}};

                                                ////////////////////////////
                                                //    Discord Commands    //
                                                ////////////////////////////
consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, Discord_option_Onchange);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_OnOff, Discord_option_Onchange);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "All", CV_SAVE|CV_CALL, statustype_cons_t, Discord_option_Onchange);
consvar_t cv_discordcharacterimagetype = CVAR_INIT ("discordcharacterimagetype", "CS Portrait", CV_SAVE|CV_CALL, characterimagetype_cons_t, DRPC_UpdatePresence);
//// Custom Discord Status Things ////
consvar_t cv_customdiscorddetails = CVAR_INIT ("customdiscorddetails", "I'm Feeling Good!", CV_SAVE|CV_CALL, NULL, Discord_option_Onchange);
consvar_t cv_customdiscordstate = CVAR_INIT ("customdiscordstate", "I'm Playing Sonic Robo Blast 2!", CV_SAVE|CV_CALL, NULL, Discord_option_Onchange);
// Custom Discord Status Image Type
consvar_t cv_customdiscordlargeimagetype = CVAR_INIT ("customdiscordlargeimagetype", "CS Portraits", CV_SAVE|CV_CALL, customimagetype_cons_t, Discord_option_Onchange);
consvar_t cv_customdiscordsmallimagetype = CVAR_INIT ("customdiscordsmallimagetype", "Continue Sprites", CV_SAVE|CV_CALL, customimagetype_cons_t, Discord_option_Onchange);
// Custom Discord Status Images
    // Characters //
consvar_t cv_customdiscordlargecharacterimage = CVAR_INIT ("customdiscordlargecharacterimage", "Sonic", CV_SAVE|CV_CALL, customcharacterimage_cons_t, Discord_option_Onchange);
consvar_t cv_customdiscordsmallcharacterimage = CVAR_INIT ("customdiscordsmallimage", "Tails", CV_SAVE|CV_CALL, customcharacterimage_cons_t, Discord_option_Onchange);
    // Maps //
consvar_t cv_customdiscordlargemapimage = CVAR_INIT ("customdiscordlargemapimage", "MIN", CV_SAVE|CV_CALL, custommapimage_cons_t, Discord_option_Onchange);
consvar_t cv_customdiscordsmallmapimage = CVAR_INIT ("customdiscordsmallmapimage", "MAX", CV_SAVE|CV_CALL, custommapimage_cons_t, Discord_option_Onchange);
    // Miscellanious //
consvar_t cv_customdiscordlargemiscimage = CVAR_INIT ("customdiscordlargemiscimage", "Default", CV_SAVE|CV_CALL, custommiscimage_cons_t, Discord_option_Onchange);
consvar_t cv_customdiscordsmallmiscimage = CVAR_INIT ("customdiscordsmallmiscimage", "Intro 1", CV_SAVE|CV_CALL, custommiscimage_cons_t, Discord_option_Onchange);
    // Captions //
consvar_t cv_customdiscordlargeimagetext = CVAR_INIT ("customdiscordlargeimagetext", "My Favorite Character!", CV_SAVE|CV_CALL, NULL, Discord_option_Onchange);
consvar_t cv_customdiscordsmallimagetext = CVAR_INIT ("customdiscordsmallimagetext", "My Other Favorite Character!", CV_SAVE|CV_CALL, NULL, Discord_option_Onchange);
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
	UINT8 i;
	char *output = malloc(sizeof(char) * (IP_SIZE+1));
	const UINT8 xor[IP_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
		
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
	COM_BufAddText(va("connect \"%s\"\n", ip));

	M_ClearMenus(true); //Don't have menus open during connection screen
	if (demoplayback && titledemo)
		G_CheckDemoStatus(); //Stop the title demo, so that the connect command doesn't error if a demo is playing

	CONS_Printf("Connecting to %s via Discord\n", ip);
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
	if (discordInfo.whoCanInvite > 1)
		return false; // Client has the CVar set to off, so never allow invites from this client.

	if (!Playing())
		return false; // We're not playing, so we should not be getting invites.
	
	if (discordInfo.joinsAllowed || cv_allownewplayer.value) //hack, since discordInfo.joinsAllowed doesn't work
	{
		if (!discordInfo.whoCanInvite && (consoleplayer == serverplayer || IsPlayerAdmin(consoleplayer)))
			return true; // Only admins are allowed!
		else if (discordInfo.whoCanInvite)
			return true; // Everyone's allowed!
	}

	return false; // Did not pass any of the checks
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

	if (!DRPC_InvitesAreAllowed())
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
		//return NULL;
		return self_ip;
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
	char detailstr[64+26+15] = "";
	char statestr[64+26+15] = "";

	char mapimg[8+1] = "";
	char mapname[5+21+21+2+1] = "";

	char charimg[4+SKINNAMESIZE+1] = "";
	char charname[11+SKINNAMESIZE+1] = "";

	char servertype[15+10] = "";
	char servertag[11+26+15+8] = "";

	static const char *supportedSkins[] = {// Supported Skin Pictures
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

	//nerd emoji moment
	char detailGrammar[1+2] = "";
	
	char stateGrammar[2+2] = "";
	char stateType[10+9+5] = "";

	char allEmeralds[1+2+2] = "";
	char emeraldComma[1+2] = "";
	char emeraldGrammar[2+1] = "";
	char emeraldMeme[3+5+12+7] = "";

	char lifeType[9+10+2+7] = "";
	char lifeGrammar[9+10+2+3+4] = "";

	char spectatorType[9+10] = "";
	char spectatorGrammar[2+3] = "";

	char gametypeGrammar[2+3+1+9] = "";
	char gameType[2+3+8+9] = "";

	char characterPlaying[2+6+4+10] = "";

	char charImageType[2+2+1] = "";

	// custom discord things from menu.c that i had to redeclare here because i do not know much about c
	static const char *customStringType[] = {
		"char",
		"cont",
		"map",
		"misc",
		NULL
	};

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
		// Now it also shows a few predetermined states, thanks to Star :)
		discordPresence.largeImageKey = "misctitle";
		discordPresence.largeImageText = "Sonic Robo Blast 2";
		discordPresence.details = "In Game";

		if (paused) // You Should Be Able To Set Whether You're Paused Or Not, In My Opinion, No Matter What. And That's Why I Fight For Yo-
			discordPresence.state = "Currently Paused";
		else if (menuactive || !Playing()) // Scrolling Through the Menus or Not Playing? Set the following as Your Status Then.
			discordPresence.state = "In The Menu";
		else // Not Doing Any Of The Above? You're Technically Playing the Game Then.
			discordPresence.state = "Actively Playing";

		DRPC_EmptyRequests();
		Discord_RunCallbacks();
		Discord_UpdatePresence(&discordPresence);
		return;
	}

#ifdef DEVELOP
	// This way, we can use the invite feature in-dev, but not have snoopers seeing any potential secrets! :P
	discordPresence.largeImageKey = "miscdevelop";
	discordPresence.largeImageText = "No peeking!";
	discordPresence.details = "Developing a Masterpiece!";
	discordPresence.state = "Keep your Eyes Peeled!";
	DRPC_EmptyRequests();
	Discord_RunCallbacks();
	Discord_UpdatePresence(&discordPresence);
	return;
#endif // DEVELOP

	////////////////////////////////////////////
	////   Main Rich Presence Status Info   ////
	////////////////////////////////////////////

	// Reset discord info and presence if you're not in a place that uses it!
    // Important for if you join a server that compiled without HAVE_DISCORDRPC,
	// so that you don't ever end up using bad information from another server.
    memset(&discordInfo, 0, sizeof(discordInfo));

	//// Server Info ////
	if (dedicated || netgame || multiplayer)
	{
		if (DRPC_InvitesAreAllowed())
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
		}

		switch (ms_RoomId)
		{
			case 33: snprintf(servertype, 26, "Standard"); break;
			case 28: snprintf(servertype, 26, "Casual"); break;
			case 38: snprintf(servertype, 26, "Custom Gametype"); break;
			case 31: snprintf(servertype, 26, "OLDC"); break;
			case -1: default: snprintf(servertype, 26, "Private"); break; // Private server
		}

		if (cv_discordshowonstatus.value != 8)
		{
			snprintf(servertag, 60, (server ? (!dedicated ? "Hosting a %s Server" : "Hosting a Dedicated %s Server") : "In a %s Server"), servertype);
			discordPresence.details = servertag;
		}
		discordPresence.partyId = server_context; // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
		discordPresence.partySize = D_NumPlayers(); // Players in server
		discordPresence.partyMax = cv_maxplayers.value; // Max players
		discordPresence.instance = 1;
	}
	else if (dedicated || netgame || multiplayer || !multiplayer)
	{
		//// Set Status Picture (Just in Case) ////
		if (((cv_discordshowonstatus.value == 1 || cv_discordshowonstatus.value == 5) && !Playing()) || (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5))
		{
			discordPresence.largeImageKey = "misctitle";
			discordPresence.largeImageText = "Sonic Robo Blast 2";
		}

		if (playeringame[consoleplayer] && !demoplayback)
		{
			//// Emblems ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 4)
				snprintf(detailstr, 105, "%d/%d Emblems", M_CountEmblems(), (numemblems + numextraemblems));
			
			//// Emeralds ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 3)
			{
				for (INT32 i = 0; i < 7; i++) // thanks Monster Iestyn for this math (and thank you uncapped plus fababis for providing me with this math)
					if (emeralds & (1<<i))
						emeraldCount += 1;

				if (!cv_discordshowonstatus.value) //banrey the dinosaur taught eme garammar
					snprintf(emeraldComma, 3, ", ");
				if (emeraldCount != 1) //guess what the joke is
					snprintf(emeraldGrammar, 2, "s");
				if (emeraldCount == 7) //Mystic Power Gang
					snprintf(allEmeralds, 5, "All ");

				if (cv_discordstatusmemes.value) //Honestly relatable lol
				{
					if (emeraldCount == 3 || emeraldCount == 4) //Funny meme lol
						snprintf(emeraldGrammar, 3, "s;");
					else if (!emeralds) //there's a special stage token right at the BEGINNING OF GFZ1 HOW DO YOU NOT HAVE A EMERALD YET
						snprintf(emeraldGrammar, 3, "s?");
						
					// Fun Fact: the subtitles in Shadow the Hedgehog emphasized "fourth", even though Jason Griffith emphasized "damn" in this sentence
					if (emeraldCount == 3)
						snprintf(emeraldMeme, 27, " Where's That DAMN FOURTH?");
					else if (emeraldCount == 4)
						snprintf(emeraldMeme, 27, " Found That DAMN FOURTH!");
				}
				strlcat(detailstr, va("%s%s%d Emerald%s%s", emeraldComma, allEmeralds, emeraldCount, emeraldGrammar, emeraldMeme), 105);
			}

			//// Score ////
			if (cv_discordshowonstatus.value == 2)
				snprintf(detailstr, 105, "Current Score: %d", players[consoleplayer].score);
			
			//// SRB2 Playtime ////
			if (cv_discordshowonstatus.value == 7)
				snprintf(detailstr, 105, "Total Playtime: %d hours, %d minutes, %d seconds", G_TicsToHours(totalplaytime), G_TicsToMinutes(totalplaytime, false), G_TicsToSeconds(totalplaytime));

			//// Tiny Detail Things; Complete Games, etc. ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value != 8)
			{
				if (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5 && cv_discordshowonstatus.value != 6)
					snprintf(detailGrammar, 3, ", ");

				if (gamecomplete) //You've beat the game? You Get A Special Status Then!
					strlcat(detailstr, va("%sHas Beaten the Game" , detailGrammar), 105);
			}

			//// Apply our Status, And We're Done :) ////
			discordPresence.details = detailstr;
		}
		
		if (cv_discordshowonstatus.value != 8)
		{
			if (demoplayback && !titledemo)
				snprintf(statestr, 25, "Watching Replays");
			else if (demoplayback && titledemo)
				snprintf(statestr, 25, "Watching A Demo");
			else
			{
				if (!Playing())
				{
					discordPresence.largeImageKey = "misctitle";
					discordPresence.largeImageText = (!cv_discordshowonstatus.value ? "Title Screen" : "Sonic Robo Blast 2");
					snprintf(statestr, 18, "Main Menu");
				}
			}
			discordPresence.state = statestr;
		}
	}

	//// Statuses ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6)
	{
		if (((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing()) || (paused || menuactive || jukeboxMusicPlaying))
		{
			if (Playing())
			{
				// Modes //
				snprintf(gametypeGrammar, 20, (!ultimatemode ? "Playing " : "Taking on "));
				if (modeattacking)
					snprintf(gameType, 12, ((maptol != TOL_NIGHTS && maptol != TOL_XMAS) ? "Time Attack" : "NiGHTS Mode"));
				else
				{
					if (!splitscreen)
					{
						if ((gametype == GT_COOP) && (!netgame))
							snprintf(gameType, 15, (!ultimatemode ? "Single-Player" : "Ultimate Mode"));
						else
							snprintf(gameType, 24, "%s", gametype_cons_t[gametype].strvalue);
					}
					else
						snprintf(gameType, 14, "Split-Screen");
				}
				if (modifiedgame)
					strlcat(gameType, " With Mods", 12);
				
				// Lives //
				if ((!players[consoleplayer].spectator && players[consoleplayer].lives) && gametyperules & GTR_LIVES && !ultimatemode)
				{
					if ((players[consoleplayer].lives == INFLIVES) || (!cv_cooplives.value && (netgame || multiplayer)))
						snprintf(lifeGrammar, 22, ", Has Infinite Lives");
					else
						snprintf(lifeGrammar, 17, (players[consoleplayer].lives == 1 ? ", %d Life Left" : ", %d Lives Left"), players[consoleplayer].lives);				
				}
				else if (!players[consoleplayer].spectator && !players[consoleplayer].lives)
					snprintf(lifeGrammar, 15, ", Game Over...");
				
				// Spectators
				if (playeringame[consoleplayer])
				{
					if (!players[consoleplayer].spectator)
					{
						snprintf(spectatorGrammar, 4, (((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer != consoleplayer))) ? "ing" : "er"));
						snprintf(spectatorType, 21, "View%s", spectatorGrammar);
					}
					else
					{
						snprintf(lifeGrammar, 12, ", Dead & ");
						snprintf(spectatorGrammar, 4, (((displayplayer != consoleplayer) || (cv_discordstatusmemes.value && (displayplayer == consoleplayer))) ? "ing" : "or"));
						snprintf(spectatorType, 21, "Spectat%s", spectatorGrammar);
					}

					if (displayplayer != consoleplayer)
						snprintf(lifeType, 30, "%s %s", spectatorType, player_names[displayplayer]);
					else
					{
						if (players[consoleplayer].spectator)
							snprintf(lifeType, 27, (!cv_discordstatusmemes.value ? "In %s Mode" : "%s Air"), spectatorType);
					}
				}
				snprintf(statestr, 105, "%s%s%s%s", gametypeGrammar, gameType, lifeGrammar, lifeType);
			}
			
			//// Tiny State Things; Pausing, Active Menues, etc. ////
			if ((!cv_discordshowonstatus.value || cv_discordshowonstatus.value != 8) && (paused || menuactive || jukeboxMusicPlaying))
			{
				snprintf(stateGrammar, 3, ", ");

				if (paused) // You Should Be Able To Set Whether You're Paused Or Not, In My Opinion, No Matter What.
					snprintf(stateType, 20, "%sCurrently Paused", stateGrammar);
				if (menuactive) // Scrolling Through the Menus? Set it as Your Status Then.
					snprintf(stateType, 27, "%sScrolling Through Menus", stateGrammar);
				if (jukeboxMusicPlaying) // Playing Jukebox Music? Copy and Display It On Your Status Then
					strlcat(stateType, va("%sPlaying %s in the Jukebox", stateGrammar, jukeboxMusicName), 95);
				
				strlcat(statestr, va("%s", stateType), 105);
			}
			discordPresence.state = statestr;
		}
	}

	//// Maps ////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 5)
	{
		if (gamestate == GS_INTRO)
		{
			discordPresence.largeImageKey = "miscintro1";
			discordPresence.largeImageText = "Intro";
			discordPresence.state = "Watching the Intro";
		}
		else if (gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) // Map info
		{
			if ((gamemap >= 1 && gamemap <= 73) // Supported Co-op maps
			|| (gamemap >= 532 && gamemap <= 543) // Supported Match maps
			|| (gamemap >= 280 && gamemap <= 288)) // Supported CTF maps
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
				discordPresence.largeImageText = "Map: ???"; // Hell map, hide the name
			else
			{
				// Map name on tool tip
				if (gamemap != 99 && gamestate != GS_TITLESCREEN && !titlemapinaction)
					snprintf(mapname, 48, "%s", G_BuildMapTitle(gamemap));
				//fully fixes null map issue
				else
				{
					snprintf(mapname, 13, "Title Screen");
					discordPresence.largeImageKey = "misctitle";
				}

				discordPresence.largeImageText = mapname;
				if (cv_discordshowonstatus.value) // Display the Map's Name on our Status, Since That's What We Set
					discordPresence.state = (!cv_discordshowonstatus.value ? mapname : ((gamemap != 99 && gamestate != GS_TITLESCREEN && !titlemapinaction) ? va("On Map: %s", mapname) : mapname));
			}

			if ((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && (Playing() || paused))
			{
				const time_t currentTime = time(NULL);
				const time_t mapTimeStart = currentTime - (leveltime / TICRATE);

				discordPresence.startTimestamp = mapTimeStart;

				if (timelimitintics > 0 && gametyperules == GTR_TIMELIMIT)
				{
					const time_t mapTimeEnd = mapTimeStart + ((timelimitintics + 1) / TICRATE);
					discordPresence.endTimestamp = mapTimeEnd;
				}
			}
		}
		else if (gamestate == GS_EVALUATION || gamestate == GS_GAMEEND || gamestate == GS_CREDITS || gamestate == GS_ENDING|| gamestate == GS_CONTINUING)
		{	
			discordPresence.largeImageKey = "misctitle";
			discordPresence.largeImageText = "Sonic Robo Blast 2";
				
			if (gamestate == GS_EVALUATION && !ultimatemode)
				discordPresence.details = "Evaluating Results";
			else if (gamestate == GS_CONTINUING) 
				discordPresence.details = "On the Continue Screen";

			if (ultimatemode)
				discordPresence.details = (!cv_discordstatusmemes.value ? "Just Beat Ultimate Mode!" : "Look, It's my Greatest Achievement: An Ultimate Mode Complete Discord RPC Status");
		}
	}

	//// Characters ////
	snprintf(charImageType, 5, (!cv_discordcharacterimagetype.value ? "char" : "cont"));
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 1)
	{
		if (!cv_discordshowonstatus.value)
			snprintf(characterPlaying, 22, "Playing As: ");

		// Character Images/Tags
		snprintf(charimg, 11, "%scustom", charImageType); // Unsupported
		for (INT32 i = 0; i < 23; i++) //23 is the current amount of custom characters that are supported lol
		{
			if ((strcmp(skins[players[consoleplayer].skin].name, "sonic") == 0) && (strcmp(skins[players[secondarydisplayplayer].skin].name, "tails") == 0)) // Let's make sure they sonic and tails, just in case
			{
				snprintf(charimg, 15, "%ssonictails", charImageType); // Put that Image on Then!
				break;
			}
			else if (strcmp(skins[players[consoleplayer].skin].name, supportedSkins[i]) == 0)
			{
				snprintf(charimg, 36, "%s%s", charImageType, skins[players[consoleplayer].skin].name); // Supported
				break;
			}
		}

		if (playeringame[consoleplayer])
		{
			// Why Would You Split My Screen
			if (!splitscreen)
			{
				//// No Bots ////
				if (!players[1].bot)
					snprintf(charname, 32, "%s%s", characterPlaying, skins[players[consoleplayer].skin].realname);
				//// Bots ////
				else if (players[1].bot)
				{
					// Only One Regular Bot?
					if (!players[2].bot)
						snprintf(charname, 50, "%s%s & %s", characterPlaying, skins[players[consoleplayer].skin].realname, skins[players[secondarydisplayplayer].skin].realname);
					// Multiple Bots?
					else
						snprintf(charname, 75, "%s%s & %s, With Multiple Bots", characterPlaying, skins[players[consoleplayer].skin].realname, skins[players[secondarydisplayplayer].skin].realname);
				}
			}
			// I Split my Screen
			else
			{
				// render player names and the character image
				snprintf(charname, 50, "%s & %s", player_names[consoleplayer], player_names[secondarydisplayplayer]);
				snprintf(charimg, 15, "%ssonictails", charImageType);
			}
			
			// Apply Character Images and Names
			discordPresence.smallImageKey = charimg; // Character image
			discordPresence.smallImageText = charname; // Character name, and even Bot name (if they exist)
			if (cv_discordshowonstatus.value) // Display it loud and proud on their status too, if they set it to be this way
				discordPresence.state = va("Playing As: %s", charname);
		}
	}
	
	//// NOTE: The Main Custom Status Functions can be Found in m_menu.c! The following is just backported from there.
	if (cv_discordshowonstatus.value == 8)
	{
		discordPresence.details = cv_customdiscorddetails.string;
		discordPresence.state = cv_customdiscordstate.string;

		if (cv_customdiscordsmallimagetype.value < 2)
			discordPresence.smallImageKey = (cv_customdiscordsmallcharacterimage.value > 0 ? customSImageString : va("%scustom", customStringType[cv_customdiscordsmallimagetype.value]));
		else if (cv_customdiscordsmallimagetype.value == 2)
			discordPresence.smallImageKey = (cv_customdiscordsmallmapimage.value > 0 ? customSImageString : "map01");
		else
			discordPresence.smallImageKey = (cv_customdiscordsmallmiscimage.value > 0 ? customSImageString : "misctitle");

		if (cv_customdiscordlargeimagetype.value < 2)
			discordPresence.largeImageKey = (cv_customdiscordlargecharacterimage.value > 0 ? customLImageString : va("%scustom", customStringType[cv_customdiscordlargeimagetype.value]));
		else if (cv_customdiscordlargeimagetype.value == 2)
			discordPresence.largeImageKey = (cv_customdiscordlargemapimage.value > 0 ? customLImageString : "map01");
		else
			discordPresence.largeImageKey = (cv_customdiscordlargemiscimage.value > 0 ? customLImageString : "misctitle");

		discordPresence.smallImageText = cv_customdiscordsmallimagetext.string;
		discordPresence.largeImageText = cv_customdiscordlargeimagetext.string;
	}

	if (!joinSecretSet)
		DRPC_EmptyRequests(); // Not able to join? Flush the request list, if it exists.

	Discord_RunCallbacks();
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