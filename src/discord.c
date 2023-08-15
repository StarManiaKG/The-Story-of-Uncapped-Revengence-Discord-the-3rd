// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
// Copyright (C) 2020-2023 by Star "I Make My Own Copyrights" ManiaKG.
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
#include "m_menu.h" // gametype_cons_t and jukebox stuff
#include "r_things.h" // skins
#include "mserv.h" // msServerId
#include "z_zone.h"
#include "byteptr.h"
#include "stun.h"
#include "i_tcp.h" // current_port

#include "discord.h"
#include "doomdef.h"

#include "p_local.h" // all7matchemeralds
#include "m_cond.h" // queries about emblems
#include "v_video.h" // hud stuff, mainly

#include "w_wad.h" // numwadfiles
#include "d_main.h" // extrawads
#include "d_netfil.h" // nameonly
#include "doomstat.h" // savemoddata
#include "dehacked.h" // titlechanged

#include "fastcmp.h" // fastcmp, helps with super stuff

#include "STAR/star_vars.h" // STAR STUFF: PROVIDES UNIQUE STUFF

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID "1013126566236135516"

#define IP_SIZE 21 // length of IP strings

static CV_PossibleValue_t statustype_cons_t[] = {
    {0, "Default"},

    {1, "Only Characters"},
    {2, "Only Score"},
    {3, "Only Emeralds"},
    {4, "Only Emblems"},
    {5, "Only Levels"},
    {6, "Only Statuses"},
    {7, "Only Playtime"},
    {8, "Custom"},
    {0, NULL}};

//// Custom Status and Images ////
// Character Image Types //
static CV_PossibleValue_t characterimagetype_cons_t[] = {
	{0, "CS Portrait"},
	{1, "Continue Sprite"},
	{2, "Life Icon Sprite"},
	{0, NULL}};

// All Image Types //
static CV_PossibleValue_t customimagetype_cons_t[] = {
	{0, "CS Portraits"},
	{1, "Continue Sprites"},
	{2, "Life Icon Sprites"},

	{3, "S. CS Portraits"},
	{4, "S. Continue Sprites"},
	{5, "S. Life Icon Sprites"},

	{6, "Maps"},
	{7, "Miscellaneous"},
	{8, "None"},
	{0, NULL}};

// Characters //
static CV_PossibleValue_t customcharacterimage_cons_t[] = {
    // Vanilla Chars
    {0, "Default"},			// ...Does ghost sonic count as a vanilla char? Maybe.
    {1, "Sonic"},
    {2, "Tails"},
    {3, "Knuckles"},
    {4, "Amy"},
    {5, "Fang"},
    {6, "Metal Sonic"},
    {7, "Sonic & Tails"}, 	// Bots, am I right?
    
	// Custom Chars
    {8, "Adventure Sonic"},
    {9, "Shadow"},
    {10, "Skip"},
    {11, "Jana"},
    {12, "Surge"},
    {13, "Cacee"},
    {14, "Milne"},
    {15, "Maimy"},
    {16, "Mario"},
    {17, "Luigi"},
    {18, "Blaze"},
    {19, "Marine"},
    {20, "Tails Doll"},
    {21, "Metal Knuckles"},
    {22, "Smiles"},
    {23, "Whisper"},

	// My Chars
    {24, "Hexhog"},

	// My Friendos' Chars
	{25, "Speccy"},
    {0, NULL}};

static CV_PossibleValue_t customsupercharacterimage_cons_t[] = {
    // Vanilla Chars
    {0, "Sonic"},
	{1, "Sonic & Tails"}, // Bots, am I right?
	{0, NULL}};

// Maps //
static CV_PossibleValue_t custommapimage_cons_t[] = {
    // Singleplayer/Co-op Maps
    {0, "GFZ1"},
    {1, "GFZ2"},
    {2, "GFZ3"},

    {3, "THZ1"},
    {4, "THZ2"},
    {5, "THZ3"},

    {6, "DSZ1"},
    {7, "DSZ2"},
    {8, "DSZ3"},

    {9, "CEZ1"},
    {10, "CEZ2"},
    {11, "CEZ3"},

    {12, "ACZ1"},
    {13, "ACZ2"},
    {14, "ACZ3"},

    {15, "RVZ1"},

    {16, "ERZ1"},
    {17, "ERZ2"},

    {18, "BCZ1"},
    {19, "BCZ2"},
    {20, "BCZ3"},

    // Extra Maps
    {21, "BS - FHZ"},
    {22, "BS - PTZ"},
    {23, "BS - FFZ"},
    {24, "BS - TLZ"},

    // Advanced Maps
    {25, "CS - HHZ"},
    {26, "CS - AGZ"},
    {27, "CS - ATZ"},

    // Singleplayer Special Stages
    {28, "SSS - FFZ"},
    {29, "SSS - TPZ"},
    {30, "SSS - FCZ"},
    {31, "SSS - CFZ"},
    {32, "SSS - DWZ"},
    {33, "SSS - MCZ"},
    {34, "SSS - ESZ"},
    {35, "SSS - BHZ"},

    // Co-op Special Stages
    {36, "MSS - 1"},
    {37, "MSS - 2"},
    {38, "MSS - 3"},
    {39, "MSS - 4"},
    {40, "MSS - 5"},
    {41, "MSS - 6"},
    {42, "MSS - 7"},

    // Other Things I Probably Forgot Because I'm Smart lol
    {43, "NBS - CCZ"},
    {44, "NBS - DHZ"},
    {45, "NBS - APZ1"},
    {46, "NBS - APZ2"},

    // CTF Maps
    {47, "CTF - LFZ"},
    {48, "CTF - LPZ"},
    {49, "CTF - SCZ"},
    {50, "CTF - IFZ"},
    {51, "CTF - TTZ"},
    {52, "CTF - CTZ"},
    {53, "CTF - ITZ"},
    {54, "CTF - DFZ"},
    {55, "CTF - NRZ"},

    // Match/Team Match/H&S/Tag Maps
    {56, "MATCH - JVZ"},
    {57, "MATCH - NFZ"},
    {58, "MATCH - TPZ"},
    {59, "MATCH - TCZ"},
    {60, "MATCH - DTZ"},
    {61, "MATCH - ICZ"},
    {62, "MATCH - OHZ"},
    {63, "MATCH - SFZ"},
    {64, "MATCH - DBZ"},
    {65, "MATCH - CSZ"},
    {66, "MATCH - FCZ"},
    {67, "MATCH - MMZ"},

    // Tutorial Map
    {68, "Tutorial - TZ"},
    
    // Custom Map
    {69, "Custom"},
    {0, NULL}};

// Miscellanious //
static CV_PossibleValue_t custommiscimage_cons_t[] = {
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
	{13, "Alt. Sonic Image 5"},

	{14, "Alt. Tails Image 1"},
	{15, "Alt. Tails Image 2"},

	{16, "Alt. Knuckles Image 1"},
	{17, "Alt. Knuckles Image 2"},

	{18, "Alt. Amy Image 1"},

	{19, "Alt. Fang Image 1"},

	{20, "Alt. Metal Sonic Image 1"},
	{21, "Alt. Metal Sonic Image 2"},

	{22, "Alt. Eggman Image 1"},
	{0, NULL}};

                                                ////////////////////////////
                                                //    Discord Commands    //
                                                ////////////////////////////
consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, Discord_option_Onchange);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdateUsername);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_OnOff, Discord_option_Onchange);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "Default", CV_SAVE|CV_CALL, statustype_cons_t, Discord_option_Onchange);
consvar_t cv_discordcharacterimagetype = CVAR_INIT ("discordcharacterimagetype", "CS Portrait", CV_SAVE|CV_CALL, characterimagetype_cons_t, DRPC_UpdatePresence);

//// Custom Discord Status Things ////
consvar_t cv_customdiscorddetails = CVAR_INIT ("customdiscorddetails", "I'm Feeling Good!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
consvar_t cv_customdiscordstate = CVAR_INIT ("customdiscordstate", "I'm Playing Sonic Robo Blast 2!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);

// Custom Discord Status Image Type //
consvar_t cv_customdiscordlargeimagetype = CVAR_INIT ("customdiscordlargeimagetype", "CS Portraits", CV_SAVE|CV_CALL, customimagetype_cons_t, Discord_option_Onchange);
consvar_t cv_customdiscordsmallimagetype = CVAR_INIT ("customdiscordsmallimagetype", "Continue Sprites", CV_SAVE|CV_CALL, customimagetype_cons_t, Discord_option_Onchange);

// Custom Discord Status Images //
// Characters
consvar_t cv_customdiscordlargecharacterimage = CVAR_INIT ("customdiscordlargecharacterimage", "Sonic", CV_SAVE|CV_CALL, customcharacterimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallcharacterimage = CVAR_INIT ("customdiscordsmallcharacterimage", "Tails", CV_SAVE|CV_CALL, customcharacterimage_cons_t, DRPC_UpdatePresence);

consvar_t cv_customdiscordlargesupercharacterimage = CVAR_INIT ("customdiscordlargesupercharacterimage", "Sonic", CV_SAVE|CV_CALL, customsupercharacterimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallsupercharacterimage = CVAR_INIT ("customdiscordsmallsupercharacterimage", "Sonic", CV_SAVE|CV_CALL, customsupercharacterimage_cons_t, DRPC_UpdatePresence);

// Maps
consvar_t cv_customdiscordlargemapimage = CVAR_INIT ("customdiscordlargemapimage", "GFZ1", CV_SAVE|CV_CALL, custommapimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallmapimage = CVAR_INIT ("customdiscordsmallmapimage", "Custom", CV_SAVE|CV_CALL, custommapimage_cons_t, DRPC_UpdatePresence);
    
// Miscellanious
consvar_t cv_customdiscordlargemiscimage = CVAR_INIT ("customdiscordlargemiscimage", "Default", CV_SAVE|CV_CALL, custommiscimage_cons_t, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallmiscimage = CVAR_INIT ("customdiscordsmallmiscimage", "Intro 1", CV_SAVE|CV_CALL, custommiscimage_cons_t, DRPC_UpdatePresence);
   
// Captions
consvar_t cv_customdiscordlargeimagetext = CVAR_INIT ("customdiscordlargeimagetext", "My Favorite Character!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);
consvar_t cv_customdiscordsmallimagetext = CVAR_INIT ("customdiscordsmallimagetext", "My Other Favorite Character!", CV_SAVE|CV_CALL, NULL, DRPC_UpdatePresence);

struct discordInfo_s discordInfo; // Dedicated Server Safety Struct
discordRequest_t *discordRequestList = NULL;

#ifndef DEVELOP
boolean devmode = false;
#else
boolean devmode = true;
#endif

static char self_ip[IP_SIZE];

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
	void DRPC_UpdateUsername(void);

		Updates the current Discord Rich Presence username.
	
	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/

void DRPC_UpdateUsername(void)
{
	if (!discordInfo.Initialized)
		return;
	memset(&discordInfo.sessionUsername, 0, sizeof(discordInfo.sessionUsername));

	strcpy(discordInfo.sessionUsername, discordInfo.grabbedUsername);
	if (!cv_discordstreamer.value)
		strlcat(discordInfo.sessionUsername, va("#%s", discordInfo.discriminator), 256);
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
	// Print Name
	CONS_Printf("Discord: connected to %s%s\n", user->username,
				(cv_discordstreamer.value ? "" : va("#%s (%s)", user->discriminator, user->userId)));
	
	// Store Things
	discordInfo.Disconnected = false;

	strcpy(discordInfo.grabbedUsername, user->username);
	strcpy(discordInfo.discriminator, user->discriminator);
	strcpy(discordInfo.userID, user->userId);

	discordInfo.Initialized	= true;

	// Run Functions, and We're Done :)
	DRPC_UpdateUsername();
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

	M_ClearMenus(true); 			// Don't have menus open during connection screen
	if (demoplayback && titledemo)
		G_CheckDemoStatus(); 		// Stop the title demo, so that the connect command doesn't error if a demo is playing

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
		true if invites are allowed, false otherwise
--------------------------------------------------*/
static boolean DRPC_InvitesAreAllowed(void)
{
	if ((!Playing())					// We're Not Playing, So No Invites Should Be Sent.
		|| (!cv_discordasks.value)		// The Client Doesn't Allow Invites, so Don't Send Any in the First Place.
		|| (!discordInfo.joinsAllowed))	// Player's Aren't Allowed to Join the Server, so Don't Send

		return false;

	if ((!discordInfo.whoCanInvite && consoleplayer == serverplayer) 											// Only the Server Player is Allowed to Invite!
		|| (discordInfo.whoCanInvite == 1 && (consoleplayer == serverplayer || IsPlayerAdmin(consoleplayer)))	// Only Admins and the Server are Allowed to Invite!
		|| (discordInfo.whoCanInvite == 2)) 														   			// Everyone's Allowed to Invite!

		return true;

	return false; // Did Not Pass Any of the Checks, so Still Don't Send Any Invites.
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

	// Something weird happened if this occurred...
	if (DRPC_InvitesAreAllowed() == false)
	{
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
		removeRequest->prev->next = removeRequest->next;

	if (removeRequest->next != NULL)
	{
		removeRequest->next->prev = removeRequest->prev;

		if (removeRequest == discordRequestList)
			discordRequestList = removeRequest->next;
	}
	else
	{
		if (removeRequest == discordRequestList)
			discordRequestList = NULL;
	}

	Z_Free(removeRequest->username);
	Z_Free(removeRequest->userID);
	Z_Free(removeRequest);
}

/*--------------------------------------------------
	void DRPC_Init(void)

		See header file for description.

	Input Arguments:-
		None
	
	Return:-
		None
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
}

/*--------------------------------------------------
	static const char *DRPC_GetServerIP(void)

		Retrieves the IP address of the server that you're
		connected to. Will attempt to use STUN for getting your
		own IP address.
	
	Input Arguments:-
		None
	
	Return:-
		the server IP if it was found, none if otherwise
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

	Input Arguments:-
		None
	
	Return:-
		None
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

	Input Arguments:-
		None
	
	Return:-
		None
--------------------------------------------------*/
//////// 	  DEPENDANCIES 	 	////////
boolean customStringTooLow;
boolean alreadyWarned;

static void DRPC_StringError(void) // Prints a Message if Your Discord RPC Strings Are Two Small (...Get It?)
{
	if (cv_discordrp.value && cv_discordshowonstatus.value == 8)
	{
		if (!discordMenuOpen && !Playing())
			alreadyWarned = customStringTooLow = false;
		else
		{
			if (!(alreadyWarned && customStringTooLow) && (strlen(cv_customdiscorddetails.string) <= 2 || strlen(cv_customdiscordstate.string) <= 2 || strlen(cv_customdiscordsmallimagetext.string) <= 2 || strlen(cv_customdiscordlargeimagetext.string) <= 2))
				customStringTooLow = true;
			
			if (!alreadyWarned && customStringTooLow)
			{
                M_StartMessage(va("%c%s\x80\nSorry, Discord RPC requires Strings to be longer than two characters. \n\n(Press a key)\n", ('\x80' + (menuColor[cv_menucolor.value]|V_CHARCOLORSHIFT)), "Discord RPC"),NULL,MM_NOTHING);
				S_StartSound(NULL, sfx_skid);
				
				alreadyWarned = true;
			}
		}
	}
}

//////// 	  MAIN 	 	////////
void DRPC_UpdatePresence(void)
{
	////// 	  DECLARE VARS 	 //////
	char detailstr[64+26+17+23] = "";
	char statestr[64+26+15+25] = "";

	char simagestr[30+26+8] = "";
	char simagetxtstr[22+26+16] = "";

	char imagestr[28+24+12] = "";
	char imagetxtstr[18+34+12] = "";

	char mapimg[8+1] = "";
	char mapname[5+21+21+2+1] = "";

	char charimgS[4+SKINNAMESIZE+7] = "";
	char charimg[7+SKINNAMESIZE+4] = "";

	char charnameS[11+SKINNAMESIZE+1] = "";
	char charname[11+SKINNAMESIZE+1] = "";

	char customSImage[32+18] = "";
	char customLImage[35+7+8] = "";

	char servertype[15+10] = "";

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
		// Vanilla Chars
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
		// Title Screen
		"title",

		// Intro
		"intro1",
		"intro2",
		"intro3",
		"intro4",
		"intro5",
		"intro6",
		"intro7",
		"intro8",

		// Sonic
		"altsonicimage1",
		"altsonicimage2",
		"altsonicimage3",
		"altsonicimage4",
		"altsonicimage5",
		
		// Tails
		"alttailsimage1",
		"alttailsimage2",
		
		// Knuckles
		"altknucklesimage1",
		"altknucklesimage2",
		
		// Amy
		"altamyimage1",
		
		// Fang
		"altfangimage1",
		
		// Metal Sonic
		"altmetalsonicimage1",
		"altmetalsonicimage2",
		
		// Eggman
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

	/*
	// STAR NOTE: I Might Have a Better Idea for This, Actually
	static const char *customStringLink[] = {
		// Statuses
		"#s",
		"#j",
		"#t",
		"#e",
		"#m",
		NULL
	};*/

	// Iterators
	INT32 i = 0;					// General Iterator
	UINT8 emeraldCount = 0;			// Helps Me To Find The Emeralds

	INT32 checkSkin = 0; 			// Checks Through The Consoleplayer's Skin
	INT32 checkSuperSkin = 0;		// Checks Through The Consoleplayer's Super Skin
	INT32 checkSideSkin = 0; 		// Checks Through The Secondary Display Player's Skin
	INT32 checkSuperSideSkin = 0;	// Checks Through The Secondary Display Player's Super Skin

	// Booleans
	boolean joinSecretSet = false;

	////// 	  INITIALIZE 	 //////
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	////// 	  NO RICH PRESENCE ALLOWED/DEVMODE/DEDICATED MODE	 //////
	if (!cv_discordrp.value || devmode)
	{
		// Since The User Doesn't Want To Show Their Status, or Since They're Using the DEVELOP Flag, This Just Shows That They're Playing SRB2. (If that's too much, then they should just disable game activity :V)
		// However, Now it also shows a few predetermined states, based on whether you have Discord RPC off or have enabled the DEVELOP flag, thanks to Star :)

		discordPresence.largeImageKey = (devmode ? "mapcustom" : "misctitle");
		discordPresence.largeImageText = (devmode ? "Hey! No Peeking!" : "Sonic Robo Blast 2");
		
		discordPresence.details = (devmode ? "Developing a Masterpiece" : "In Game");
		discordPresence.state = (devmode ? "Keep your Eyes Peeled!" : (paused ? "Currently Paused" : ((menuactive || !Playing() ? "In The Menu" : "Actively Playing"))));

		DRPC_EmptyRequests();
		Discord_UpdatePresence(&discordPresence);
		return;
	}
	else if (dedicated)
		return;
	
	////////////////////////////////////////////
	////   Main Rich Presence Status Info   ////
	////////////////////////////////////////////

	//////// ALL GAME INFO ////////
	////// 	  SERVER INFO 	 //////
	if (netgame)
	{
		switch (discordInfo.serverRoom)
		{
			case 33: strcpy(servertype, "Standard"); break;
			case 28: strcpy(servertype, "Casual"); break;
			case 38: strcpy(servertype, "Custom Gametype"); break;
			case 31: strcpy(servertype, "OLDC"); break;

			case 0: strcpy(servertype, "Public"); break;
			default: strcpy(servertype, "Unknown/Private"); break;
		}

		if (cv_discordshowonstatus.value != 8)
			snprintf(detailstr, 60, (Playing() ? (server ? "Hosting a %s Server" : "In a %s Server") : "Looking for a Server"), servertype);

		discordPresence.partyId = server_context; 		   // Thanks, whoever gave us Mumble support, for implementing the EXACT thing Discord wanted for this field!
		discordPresence.partySize = D_NumPlayers(); 	   // Current Amount of Players in the Server
		discordPresence.partyMax = discordInfo.maxPlayers; // Max Players
		discordPresence.instance = 1;					   // Initialize Discord Net Instance, Just In Case

		if (DRPC_InvitesAreAllowed() == true)
		{
			const char *join;

			// Grab the host's IP for joining.
			if ((join = DRPC_GetServerIP()) != NULL)
			{
				discordPresence.joinSecret = DRPC_XORIPString(join);
				joinSecretSet = true;
			}
		}
	}
	else
	{
		memset(&discordInfo.maxPlayers, 0, sizeof(discordInfo.maxPlayers));
		memset(&discordInfo.joinsAllowed, 0, sizeof(discordInfo.joinsAllowed));
		memset(&discordInfo.whoCanInvite, 0, sizeof(discordInfo.whoCanInvite));
	}

	//// 	  STATUSES 		////
	if (cv_discordshowonstatus.value != 8)
	{
		//// Status Pictures ////
		if ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK) || ((!Playing() || gamestate == GS_NULL || gamestate == GS_TIMEATTACK) && (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 5)) || (cv_discordshowonstatus.value >= 2 && cv_discordshowonstatus.value != 5))
		{
			strcpy(imagestr, "misctitle");
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

		//// Status Text ////
		if (((Playing() && playeringame[consoleplayer] && !demoplayback) || cv_discordshowonstatus.value == 7))
		{
			//// Emblems ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 4)
			{
				if ((!(netgame || splitscreen)) || (cv_discordshowonstatus.value))
					snprintf((!netgame ? detailstr : statestr), 130, "%d/%d Emblems", M_CountEmblems(), (numemblems + numextraemblems));
			}
				
			//// Emeralds ////
			if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 3)
			{
				if (!modeattacking)
				{
					// Emerald Math, Provided By Monster Iestyn and Uncapped Plus Fafabis
					for (i = 0; i < 7; i++) {
						if ((gametyperules & GTR_POWERSTONES && (players[consoleplayer].powers[pw_emeralds] & (1<<i))) || (!(gametyperules & GTR_POWERSTONES) && emeralds & (1<<i)))
							emeraldCount += 1;
					}

					// Grammar
					if (!cv_discordshowonstatus.value && !splitscreen)
						strcpy(emeraldComma, ", ");
					if (emeraldCount > 1)
						strcpy(emeraldGrammar, "s");
					if (emeraldCount == 7)
						strcpy(allEmeralds, "All ");
					
					// Memes
					if (cv_discordstatusmemes.value)
					{
						// Puncuation
						if (!emeraldCount || emeraldCount == 3 || emeraldCount == 4)
							strcpy(emeraldGrammar, (emeraldCount == 3 || emeraldCount == 4) ? "s;" : "s?");
						
						// Fun Fact: the subtitles in Shadow the Hedgehog emphasized "fourth", even though Jason Griffith emphasized "damn" in this sentence
						if (emeraldCount == 3 || emeraldCount == 4)
							strcpy(emeraldMeme, ((emeraldCount == 3) ? " Where's That DAMN FOURTH?" : " Found That DAMN FOURTH!"));
						
						// Goku Mode
						if (emeraldCount == 7)
							strcpy(emeraldMeme, " Currently In Goku Mode");
					}
						
					// Apply Strings
					strlcat((!cv_discordshowonstatus.value ? detailstr : statestr),
							// No Emeralds
							((cv_discordstatusmemes.value && ((!(gametyperules & GTR_POWERSTONES) && !emeraldCount) || (gametyperules & GTR_POWERSTONES && !all7matchemeralds && !emeraldCount))) ? 
								va("%s%s%d EMERALDS?", emeraldComma, allEmeralds, emeraldCount) :
									
							// Seven Emeralds
							(all7matchemeralds ? va("%s All 7 Emeralds", emeraldComma) :
							((players[consoleplayer].powers[pw_super] ? (va("%s%s", emeraldComma, (cv_discordstatusmemes.value ? emeraldMeme : " Currently Super"))) :

							// Some Emeralds
							va("%s%s%d Emerald%s", emeraldComma, allEmeralds, emeraldCount, emeraldGrammar))))), 130);
				}
			}

			//// Score ////
			if (cv_discordshowonstatus.value == 2)
				strlcat((!netgame ? detailstr : statestr), va("Current Score: %d", players[consoleplayer].score), 130);
				
			//// SRB2 Playtime ////
			if (cv_discordshowonstatus.value == 7)
				strlcat(((Playing() && !netgame) ? detailstr : statestr), va("Total Playtime: %d Hours, %d Minutes, and %d Seconds", G_TicsToHours(totalplaytime), G_TicsToMinutes(totalplaytime, false), G_TicsToSeconds(totalplaytime)), 130);
			
			//// Tiny Details, Such as Complete Games, etc. ////
			if (!splitscreen && !netgame)
			{
				if (Playing() && (cv_discordshowonstatus.value != 1 && cv_discordshowonstatus.value != 3 && cv_discordshowonstatus.value != 5 && cv_discordshowonstatus.value != 6))
					strcpy(detailGrammar, ", ");
				
				if (gamecomplete) // You've Beaten the Game? You Get A Special Status Then!
					strlcat(detailstr, va("%sHas Beaten the Game", detailGrammar), 128);
			}
		}
	}

	////// 	  STATUSES - ELECTRIC BOOGALO 	 //////
	if (!cv_discordshowonstatus.value || cv_discordshowonstatus.value == 6)
	{
		if (((gamestate == GS_LEVEL || gamestate == GS_INTERMISSION) && Playing() && playeringame[consoleplayer]) || (paused || menuactive || jukeboxMusicPlaying))
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
			if (paused || menuactive || jukeboxMusicPlaying)
			{
				if (!cv_discordshowonstatus.value || (cv_discordshowonstatus.value == 6 && Playing()) || !Playing())
					strcpy(stateGrammar, ", ");

				snprintf(stateType, 27, (paused ? "%sCurrently Paused" : (menuactive ? "%sScrolling Through Menus" : "")), stateGrammar);
				strlcat(stateType, (jukeboxMusicPlaying ? va("%sPlaying %s in the Jukebox", stateGrammar, jukeboxMusicName) : ""), 95);
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
			if (Playing() && (playeringame[consoleplayer] || paused || menuactive))
			{
				const time_t currentTime = time(NULL);
				const time_t mapTimeStart = currentTime - (leveltime / TICRATE);

				discordPresence.startTimestamp = mapTimeStart;

				if ((cv_timelimit.value && timelimitintics) && gametyperules & GTR_TIMELIMIT)
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
	if (cv_discordshowonstatus.value == 8)
	{
		DRPC_StringError();

		// Write the Heading Strings to Discord //
		(strlen(cv_customdiscorddetails.string) > 2 ? strcpy(detailstr, cv_customdiscorddetails.string) : 0);
		(strlen(cv_customdiscordstate.string) > 2 ? strcpy(statestr, cv_customdiscordstate.string) : 0);

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

	////// 	  APPLY ALL INFO 	 //////
	discordPresence.details = detailstr;
	discordPresence.state = statestr;

	discordPresence.smallImageKey = simagestr;
	discordPresence.smallImageText = simagetxtstr;

	discordPresence.largeImageKey = imagestr;
	discordPresence.largeImageText = imagetxtstr;

	// Flush the Request List, if it Exists and We Can't Join.
	if (joinSecretSet == false)
		DRPC_EmptyRequests();

	// Finally Push Our Status and Finish Everything! //
	Discord_UpdatePresence(&discordPresence);
}

/*--------------------------------------------------
	void DRPC_Shutdown(void)

		Clears Everything Related to Discord
		Rich Presence. Only Runs When the
		Game Closes or Crashes.

	Input Arguments:-
		None
	
	Return:-
		None
--------------------------------------------------*/
void DRPC_Shutdown(void)
{
	// Initialize Discord Once More
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	memset(&discordInfo, 0, sizeof(discordInfo));
	
	// Assign a Custom Status Because We Can
	discordPresence.details = "Currently Closing...";
	discordPresence.state = "Clearing SRB2 Discord Rich Presence...";
	Discord_UpdatePresence(&discordPresence);
	
	// Empty Requests
	DRPC_EmptyRequests();

	// Close Everything Down
	I_OutputMsg("DRPC_Shutdown(): ");
	Discord_ClearPresence();
	Discord_Shutdown();
	I_OutputMsg((dedicated || devmode || !cv_discordrp.value) ? "barely started, but shut down anyways\n" : "shut down\n");
}

#endif // HAVE_DISCORDRPC