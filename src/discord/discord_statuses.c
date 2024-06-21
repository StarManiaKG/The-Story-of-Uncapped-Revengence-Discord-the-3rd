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
/// \file  discord_statuses.c
/// \brief Discord Rich Presence statuses

#include "discord_cmds.h"
#include "../doomstat.h"
#include "../m_cond.h"
#include "../g_game.h"
#include "../z_zone.h"
#include "../p_tick.h"
#include "../r_skins.h"

#include "../STAR/star_vars.h" // TSoURDt3rd struct //

#ifdef HAVE_DISCORDSUPPORT

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct
{
	const char *name;
	const char *duoname;
	boolean super;
	boolean superduo;
	const char *superduoname;
} DRPC_Chars_t;

static DRPC_Chars_t supportedSkins[] = {
	//"custom",		// ...Does ghost sonic count as a vanilla char? Maybe.
	{"sonic",          "sonictails",   true,	true,     "supersonictails"}, // Vanilla skins
	{"tails",          NULL,  		  false,   false,                  NULL},
	{"knuckles",       NULL,          false,   false,                  NULL},
	{"amy",            NULL,          false,   false,                  NULL},
	{"fang",           NULL,          false,   false,                  NULL},
	{"metalsonic",     NULL,          false,   false,                  NULL},

	{"adventuresonic", NULL,          false,   false,                  NULL}, // Custom skins
	{"shadow",         NULL,          false,   false,                  NULL},
	{"skip",           NULL,          false,   false,                  NULL},
	{"jana",           NULL,          false,   false,                  NULL},
	{"surge",          NULL,          false,   false,                  NULL},
	{"cacee",          NULL,          false,   false,                  NULL},
	{"milne",          NULL,          false,   false,                  NULL},
	{"maimy",          NULL,          false,   false,                  NULL},
	{"mario",          NULL,          false,   false,                  NULL},
	{"luigi",          NULL,          false,   false,                  NULL},
	{"blaze",          NULL,          false,   false,                  NULL},
	{"marine",         NULL,          false,   false,                  NULL},
	{"tailsdoll",      NULL,          false,   false,                  NULL},
	{"metalknuckles",  NULL,          false,   false,                  NULL},
	{"whisper",        NULL,          false,   false,                  NULL},

	{"hexhog",         NULL,          false,   false,                  NULL}, // My skins

	{"smiles",         NULL,          false,   false,                  NULL}, // Friend's skins
	{"speccy",         NULL,          false,   false,                  NULL},

	{NULL,             NULL,          false,   false,                  NULL}, // DRPC CHARS END!
};

static const char *supportedMaps[] = {
	[1] = "01", // Supported Singleplayer/Co-op Stages (GFZ-BCZ)
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
	[22] = "22",
	"23",
	[25] = "25",
	"26",
	"27",

	[30] = "30", // Unlockable Extra Stages
	"31",
	"32",
	"33",

	[40] = "40", // Unlockable Advanced Stages
	"41",
	"42",

	[50] = "50", // NiGHTS Special Stages
	"51",
	"52",
	"53",
	"54",
	"55",
	"56",
	"57",

	[60] = "60", // Co-op Special Stages
	"61",
	"62",
	"63",
	"64",
	"65",
	"66",

	[70] = "70", // Unlockable NiGHTS Stages
	"71",
	"72",
	"73",

	[280] = "f0", // Match, Team Match, H&S, & Tag Stages
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",

	[532] = "m0", // CTF Stages
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

	[1000] = "z0", // Tutorial Zone

	[10000] = "custom", // Custom Map

	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

// =====
// TOOLS
// =====

/*--------------------------------------------------
	void DRPC_StringPrintf(char *main, const char *sep, size_t size, const char *string, ...)

		Provides easier methods of concatenation when it
		comes to applying Discord Rich Presence statuses to the given string.
--------------------------------------------------*/
void DRPC_StringPrintf(char *main, const char *sep, size_t size, const char *string, ...)
{
	va_list argptr;
	char txt[size];

	if ((sep && *sep != '\0') && (main && *main != '\0' && main[strlen(main)-1] != '\0'))
		strlcat(main, sep, size); // Give up your children, seperate

	if (!string || *string == '\0')
		return;

	va_start(argptr, string);
	vsnprintf(txt, size, string, argptr);
	va_end(argptr);

	if (!main || *main == '\0' || (main && main[strlen(main)-1] == '\0'))
		snprintf(main, size, "%s", txt);
	else
		strlcat(main, txt, size);
}

/*--------------------------------------------------
	void DRPC_ImagePrintf(char *string, size_t size, const char *sep, const char *image, ...)

		Provides easier methods of applying images to
		Discord Rich Presence statuses.
--------------------------------------------------*/
void DRPC_ImagePrintf(char *string, size_t size, const char *sep, const char *image, ...)
{
	va_list argptr;
	char txt[size];

	if (!sep || *sep == '\0')
		return;
	if (!image || *image == '\0')
		return;

	va_start(argptr, image);
	vsnprintf(txt, size, image, argptr);
	va_end(argptr);

	snprintf(string, size, "%s%s", sep, txt);
}

// ========
// STATUSES
// ========

/*--------------------------------------------------
	void DRPC_ScoreStatus(char *string)

		Applies a Discord Rich Presence status, related to score amounts, to
		the given string.
--------------------------------------------------*/
void DRPC_ScoreStatus(char *string)
{
	if (!(playeringame[consoleplayer] && !demoplayback))
	{
		DRPC_StringPrintf(string, " | ", 128, "Title Screen");
		return;
	}
	DRPC_StringPrintf(string, "; ", 128, "Current Score: %d", players[consoleplayer].score);
}

/*--------------------------------------------------
	void DRPC_EmblemStatus(char *string)

		Applies a Discord Rich Presence status, related to emblems, to
		the given string.
--------------------------------------------------*/
void DRPC_EmblemStatus(char *string)
{
	DRPC_StringPrintf(string, " | ", 128, "%d/%d Emblems",
		M_CountEmblems(serverGamedata),
		(numemblems + numextraemblems));
}

/*--------------------------------------------------
	void DRPC_EmeraldStatus(char *string)

		Applies a Discord Rich Presence status, related to how many
		emeralds the user has, to the given string.
--------------------------------------------------*/
void DRPC_EmeraldStatus(char *string)
{
	UINT16 emerald_type = (gametyperules & GTR_POWERSTONES ? players[consoleplayer].powers[pw_emeralds] : emeralds);
	UINT8 emerald_count = 0; // Help me find the emouralds!

	DRPC_StringPrintf(string, " | ", 128, NULL);
	if (modeattacking)
		DRPC_StringPrintf(string, NULL, 128, "Time-Attacking; ");
	else if (!Playing() && !titledemo && !demoplayback)
		DRPC_StringPrintf(string, NULL, 128, "Title Screen; ");
	else
	{	// Emerald math, provided by Monster Iestyn and Uncapped Plus' Fafabis :)
		for (UINT8 i = 0; i < 7; i++)
		{
			if ((gametyperules & GTR_POWERSTONES) && !all7matchemeralds)
				break;
			if (all7matchemeralds)
			{
				emerald_count = 7;
				break;
			}

			if (emerald_type & 1<<i)
				emerald_count++;
		}
	}

	// Apply our Emeralds //
	if (emerald_count == 7)
		DRPC_StringPrintf(string, NULL, 128, "All ");
	DRPC_StringPrintf(string, NULL, 128, va("%d Emerald", emerald_count));
	if (emerald_count != 1)
		DRPC_StringPrintf(string, "s", 128, NULL);

	// Apply extras //
	switch (emerald_count)	
	{
		case 0: // NO EMERALDS?
		{
			if (!cv_discordstatusmemes.value)
				return;
			DRPC_StringPrintf(string, " ", 128, "(Ha, NO EMERALDS?)");
			break;
		}

		case 3: // Fun Fact: The subtitles in Shadow the Hedgehog emphasized "fourth",
		case 4: //	even though Jason Griffith emphasized "damn" in this sentence :p
		{
			if (!cv_discordstatusmemes.value)
				return;

			if (emerald_count == 3)
				DRPC_StringPrintf(string, ", ", 128, "Where's");
			else
				DRPC_StringPrintf(string, ", ", 128, "Found");
			DRPC_StringPrintf(string, NULL, 128, " that DAMN FOURTH!");
			break;
		}

		case 7: // Goku Mode
		{
			if (!players[consoleplayer].powers[pw_super])
				return;

			if (!cv_discordstatusmemes.value)
				DRPC_StringPrintf(string, ", ", 128, "Currently Super");
			else
				DRPC_StringPrintf(string, ", ", 128, "In Goku Mode");
			break;
		}

		default:
		{
			if (!cv_discordstatusmemes.value)
				return;
			DRPC_StringPrintf(string, NULL, 128, "s?");
			strupr(string);
			break;
		}
	}
}

/*--------------------------------------------------
	void DRPC_GamestateStatus(char *string, char *image, char *imagestr)

		Applies a Discord Rich Presence status, related to gamestates, to
		the given string.
--------------------------------------------------*/
void DRPC_GamestateStatus(char *string, char *image, char *imagestr)
{
	switch (gamestate)
	{
		case GS_INTRO:
		{
			DRPC_ImagePrintf(image, 128, "misc", "intro1");
			DRPC_StringPrintf(imagestr, NULL, 128, "Intro");
			DRPC_StringPrintf(string, " | ", 128, "Watching the Intro");
			break;
		}

		case GS_CUTSCENE:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", 128, "Watching a Cutscene");
			break;
		}

		case GS_TITLESCREEN:
		case GS_LEVEL:
		case GS_INTERMISSION:
		{
			char *maptitle = NULL;

			if (!G_BuildMapName(gamemap) || mapheaderinfo[gamemap-1]->menuflags & LF2_HIDEINMENU)
			{
				DRPC_ImagePrintf(image, 128, "misc", "missing");
				DRPC_StringPrintf(imagestr, " | ", 128, "???");
				break;
			}

			if (gamestate == GS_TITLESCREEN || gamemap == titlemap)
			{
				DRPC_ImagePrintf(image, 128, "misc", "title");
				if (!(Playing() && playeringame[consoleplayer]))
					DRPC_StringPrintf(imagestr, NULL, 128, "Title Screen");
				else
					DRPC_StringPrintf(imagestr, NULL, 128, "Hey y'all, Star here! What is wrong with this user?");
			}
			else
			{
				if (supportedMaps[gamemap])
					DRPC_ImagePrintf(image, 128, "map", supportedMaps[gamemap]);					
				else
					DRPC_ImagePrintf(image, 128, "map", "custom");

				maptitle = G_BuildMapTitle(gamemap);
				DRPC_StringPrintf(imagestr, NULL, 128, maptitle);
				Z_Free(maptitle);
			}

			break;
		}

		case GS_ENDING:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", 128, "Watching the Ending");
			break;
		}

		case GS_CREDITS:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", 128, "Viewing the Credits");
			break;
		}

		case GS_EVALUATION:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", 128, "Evaluating Results");
			break;
		}

		case GS_GAMEEND:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			if (!cv_discordstatusmemes.value)
			{
				DRPC_StringPrintf(string, " | ", 128, "Returning to the Main Menu...");
				if (ultimatemode)
					DRPC_StringPrintf(string, ", ", 128, "I beat Ultimate Mode!");
			}
			else
			{
				DRPC_StringPrintf(string, " | ", 128, "Did you get all those Chaos Emeralds?");
				if (ultimatemode)
					DRPC_StringPrintf(string, ", ", 128, "Look guys, my greatest achievement: A 'I beat Ultimate Mode' RPC status!");
			}
			break;
		}

		case GS_CONTINUING:
		{
			DRPC_ImagePrintf(image, 128, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", 128, "Continue?");
			break;
		}

		default:
		{
			DRPC_ImagePrintf(image, 128, "misc", "missing");
			DRPC_StringPrintf(imagestr, NULL, 128, "Sonic Robo Blast 2....?");
			DRPC_StringPrintf(string, " | ", 128, "Loading... Loading... Loading???");
			break;
		}
	}
}

/*--------------------------------------------------
	void DRPC_CharacterStatus(char *string, char *charimg, char *s_charimg, char *charname, char *s_charname)

		Applies a Discord Rich Presence status, related to levels, to
		the given string.
--------------------------------------------------*/
void DRPC_CharacterStatus(char *string, char *charimg, char *s_charimg, char *charname, char *s_charname)
{
	DRPC_Chars_t *g_discord_mainChar = NULL;
	static const char *imageType[] = { "char", "cont", "life" };

	const char *player_skin[] = { NULL, NULL, NULL, NULL };
	const char *player_skin_realname[] = { NULL, NULL, NULL, NULL };

	char duo_skin[64];

	boolean g_discord_fillstatus = (!charimg || *charimg == '\0');
	size_t g_discord_skins = 0;
	INT32 g_discord_players;
	INT32 g_discord_player_slot = 0;

	if (!Playing() || !playeringame[consoleplayer])
	{
		DRPC_StringPrintf(string, " | ", 128, "Not In-Game");
		return;
	}
	else if (players[consoleplayer].spectator)
	{
		DRPC_StringPrintf(string, " | ", 128, "Spectating");
		return;
	}

	// Find character names and supported character images //
	for (g_discord_players = consoleplayer; !player_skin[g_discord_player_slot]; g_discord_players++, g_discord_player_slot++)
	{
		const char *skin = skins[players[g_discord_players].skin].name;
		char *skin_copy = strstr(skin, "super");

		if (netgame && g_discord_player_slot)
		{
			g_discord_mainChar = NULL;
			break;
		}

		if (playeringame[g_discord_players] && !players[g_discord_players].spectator)
		{
			player_skin[g_discord_player_slot] = "custom"; // Use the custom character icon!
			if (!splitscreen)
				player_skin_realname[g_discord_player_slot] = skins[players[g_discord_players].skin].realname; // Use the skin's real name!
			else
				player_skin_realname[g_discord_player_slot] = player_names[g_discord_player_slot]; // Use the player's real name!

			for (g_discord_skins = 0; supportedSkins[g_discord_skins].name; g_discord_skins++)
			{
				if (skin_copy)
					skin = skin_copy + 5; // removes super from the name :)

				if (strcmp(skin, supportedSkins[g_discord_skins].name))
					continue; // skin wasn't found, so continue...

				if ((players[g_discord_players].powers[pw_super] || skin_copy) && supportedSkins[g_discord_skins].super)
					skin = va("super%s", skin); // Allow for super images!

				player_skin[g_discord_player_slot] = skin; // Use the supported icon!
				break;
			}
		}

		if (!g_discord_mainChar)
		{
			if (!g_discord_player_slot && supportedSkins[g_discord_skins].duoname)
				g_discord_mainChar = &supportedSkins[g_discord_skins]; // Allow for dynamic duos!
			continue;
		}
		DRPC_ImagePrintf(duo_skin, 64, player_skin[0], player_skin[1]);

		if (players[consoleplayer].powers[pw_super] || skin_copy)
		{
			if (g_discord_mainChar->super && !g_discord_mainChar->superduo)
				*duo_skin = *duo_skin + 5; // removes super
			else if (!strcmp(duo_skin, g_discord_mainChar->superduoname))
				continue; // Allow for super duos!
		}

		if (!strcmp(duo_skin, g_discord_mainChar->duoname))
			continue; // Apply our regular dynamic duo!

		g_discord_mainChar = NULL; // No duo found... 
	}

	// Apply character images and names //
	if (g_discord_mainChar && *duo_skin != '\0' && !splitscreen)
	{
		// Apply our duo image, plus others
		if (g_discord_fillstatus && player_skin[2])
			DRPC_ImagePrintf(s_charimg, 128, imageType[cv_discordcharacterimagetype.value], player_skin[2]);
		DRPC_ImagePrintf((g_discord_fillstatus ? charimg : s_charimg), 128, imageType[cv_discordcharacterimagetype.value], duo_skin);

		DRPC_StringPrintf((g_discord_fillstatus ? charname : s_charname), NULL, 128, "Playing as: %s & %s", player_skin_realname[0], player_skin_realname[1]);
	}
	else
	{
		// Apply our main images, plus others
		if (g_discord_fillstatus && player_skin[1])
			DRPC_ImagePrintf(s_charimg, 128, imageType[cv_discordcharacterimagetype.value], player_skin[1]);
		DRPC_ImagePrintf((g_discord_fillstatus ? charimg : s_charimg), 128, imageType[cv_discordcharacterimagetype.value], player_skin[0]);

		DRPC_StringPrintf((g_discord_fillstatus ? charname : s_charname), NULL, 128, "Playing as: %s", player_skin_realname[0]);
		if (player_skin_realname[1])
			DRPC_StringPrintf(s_charname, NULL, 128, " & %s", player_skin_realname[1]);
	}

	if (player_skin_realname[2])
		DRPC_StringPrintf(s_charname, NULL, 128, " & %s", player_skin_realname[2]);
	if (player_skin_realname[3])
		DRPC_StringPrintf(s_charname, " + ", 128, "Others");
}

/*--------------------------------------------------
	void DRPC_PlaytimeStatus(char *string)

		Applies a Discord Rich Presence status, related to SRB2 playtime, to
		the given string.
--------------------------------------------------*/
void DRPC_PlaytimeStatus(char *string)
{
	DRPC_StringPrintf(string, NULL, 128, "Total Playtime: %d Hours, %d Minutes, and %d Seconds",
		G_TicsToHours(serverGamedata->totalplaytime),
		G_TicsToMinutes(serverGamedata->totalplaytime, false),
		G_TicsToSeconds(serverGamedata->totalplaytime));
}

/*--------------------------------------------------
	void DRPC_CustomStatus(char *detailstr, char *statestr)

		Using the customizable custom discord status commands, this applies
		a Discord Rich Presence status to the given string.
--------------------------------------------------*/
#include "../m_menu.h"
void DRPC_CustomStatus(char *detailstr, char *statestr)
{
#if 1
	(void)detailstr;
	(void)statestr;
#else
	// Error Out if the String is Less Than Two Letters Long //
	// MAJOR STAR NOTE: please come back to this and flesh it out more lol //
	if (strlen(cv_customdiscorddetails.string) <= 2 || strlen(cv_customdiscordstate.string) <= 2 || strlen(cv_customdiscordsmallimagetext.string) <= 2 || strlen(cv_customdiscordlargeimagetext.string) <= 2)
	{
		STAR_M_StartMessage("Custom Discord RPC String Too Short", "Sorry, Discord RPC requires Strings to be longer than two characters.\n\nResetting strings with less than two letters back to defaults.\n\n(Press a key)\n",NULL,MM_NOTHING);
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
#endif
}

#endif // HAVE_DISCORDSUPPORT
