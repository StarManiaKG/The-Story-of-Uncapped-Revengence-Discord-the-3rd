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

#ifdef HAVE_DISCORDSUPPORT

#include "discord.h"

#include "../STAR/star_vars.h" // TSoURDt3rd struct //
#include "../STAR/smkg-misc.h" // TSoURDt3rd_M_RemoveStringChars() //
#include "../STAR/smkg-jukebox.h" // TSoURDt3rd_Jukebox_IsPlaying() //
#include "../STAR/menus/smkg-m_sys.h" // queries about level list modes //

#include "../m_cond.h" // queries about emblems
#include "../g_game.h"
#include "../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct
{
	const char *name;
	const char *duoname;
	boolean super;
	boolean superduo;
} DRPC_Chars_t;

static const char *imageType[] = {
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

static DRPC_Chars_t supportedSkins[] = {
	{"sonic",          "sonictails",   true,	true}, // Vanilla skins
	{"tails",          NULL,  		  false,   false},
	{"knuckles",       NULL,          false,   false},
	{"amy",            NULL,          false,   false},
	{"fang",           NULL,          false,   false},
	{"metalsonic",     NULL,          false,   false},

	{"adventuresonic", NULL,          false,   false}, // Custom skins
	{"shadow",         NULL,          false,   false},
	{"skip",           NULL,          false,   false},
	{"jana",           NULL,          false,   false},
	{"surge",          NULL,          false,   false},
	{"cacee",          NULL,          false,   false},
	{"milne",          NULL,          false,   false},
	{"maimy",          NULL,          false,   false},
	{"mario",          NULL,          false,   false},
	{"luigi",          NULL,          false,   false},
	{"blaze",          NULL,          false,   false},
	{"marine",         NULL,          false,   false},
	{"tailsdoll",      NULL,          false,   false},
	{"metalknuckles",  NULL,          false,   false},
	{"whisper",        NULL,          false,   false},

	{"hexhog",         NULL,          false,   false}, // Skins I've made

	{"smiles",         NULL,          false,   false}, // Friend's skins
	{"speccy",         NULL,          false,   false},

	{NULL,             NULL,          false,   false}, // DRPC CHARS END!
	//"custom",		// ...Does ghost sonic count as a vanilla char? Maybe.
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

	[280] = "f0", // CTF Stages
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",

	[532] = "m0", // Match, Team Match, H&S, & Tag Stages
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

	[10000] = "custom",
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

// ------------------------ //
//        Functions
// ------------------------ //

// =====
// TOOLS
// =====

/*--------------------------------------------------
	void DRPC_StringPrintf(char *main, const char *sep, const char *string, ...)

		Provides easier methods of concatenation when it
		comes to applying Discord Rich Presence statuses to the given string.
--------------------------------------------------*/
void DRPC_StringPrintf(char *main, const char *sep, const char *string, ...)
{
	va_list argptr;
	char string_txt[DISCORD_PRESENCE_STRING_SIZE];

	if ((sep != NULL && *sep != '\0') && *main != '\0')
	{
		// Give up your children, seperate
		strcat(main, sep);
	}
	if (main == NULL || !string || *string == '\0')
		return;

	va_start(argptr, string);
	vsnprintf(string_txt, DISCORD_PRESENCE_STRING_SIZE, string, argptr);
	va_end(argptr);

	strlcat(main, string_txt, DISCORD_PRESENCE_STRING_SIZE);
}

/*--------------------------------------------------
	void DRPC_ImagePrintf(char *string, const char *sep, const char *image, ...)

		Provides easier methods of applying images to
		Discord Rich Presence statuses.
--------------------------------------------------*/
void DRPC_ImagePrintf(char *string, const char *sep, const char *image, ...)
{
	va_list argptr;
	char string_txt[DISCORD_PRESENCE_IMAGE_STRING_SIZE];

	if (!string)
		return;
	if (!sep || *sep == '\0')
		return;
	if (!image || *image == '\0')
		return;

	va_start(argptr, image);
	vsnprintf(string_txt, DISCORD_PRESENCE_IMAGE_STRING_SIZE, image, argptr);
	va_end(argptr);

	snprintf(string, DISCORD_PRESENCE_IMAGE_STRING_SIZE, "%s%s", sep, string_txt);
}

// ========
// STATUSES
// ========

/*--------------------------------------------------
	void DRPC_GeneralStatus(char *string, char *image, char *imagestr)

		Applies a Discord Rich Presence status, related to general details,
		to the given string.
--------------------------------------------------*/
void DRPC_GeneralStatus(char *string, char *image, char *imagestr)
{
	DRPC_ImagePrintf(image, "misc", "title");

	if (Playing())
		return;

	if (gamestate == GS_TIMEATTACK)
	{
		if (tsourdt3rd_levellistmode == TSOURDT3RD_LLM_NIGHTSATTACK)
		{
			DRPC_ImagePrintf(image, "misc", "nights");
			DRPC_StringPrintf(imagestr, NULL, "NiGHTs Attack");
			DRPC_StringPrintf(string, NULL, "Menu: NiGHTs Attack");
		}
		else if (marathonmode)
		{
			DRPC_ImagePrintf(image, "misc", "record");
			DRPC_StringPrintf(imagestr, NULL, "Marathon Mode");
			DRPC_StringPrintf(string, NULL, "Menu: Marathon Mode");
		}
		else
		{
			DRPC_ImagePrintf(image, "misc", "record");
			DRPC_StringPrintf(imagestr, NULL, "Time Attack");
			DRPC_StringPrintf(string, NULL, "Menu: Time Attack");
		}
	}
	else if (demoplayback || titledemo)
	{
		if (!titledemo)
			DRPC_StringPrintf(string, NULL, "Watching a Replay");
		else
			DRPC_StringPrintf(string, NULL, "Watching a Demo");
	}
	else if (gamestate == GS_TITLESCREEN)
	{
		DRPC_StringPrintf(imagestr, NULL, "Title Screen");
		DRPC_StringPrintf(string, NULL, "Title Screen");
		if (menuactive)
			DRPC_StringPrintf(string, " | ", "Main Menu");
	}
	else if (gamestate == GS_NULL)
	{
		DRPC_ImagePrintf(image, "misc", "missing");
		DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2....?");
		DRPC_StringPrintf(string, NULL, "Loading... Loading... Loading???");
	}
}

/*--------------------------------------------------
	void DRPC_ExtendedStatus(char *string)

		Applies a Discord Rich Presence status, related to extended status details,
		to the given string.
--------------------------------------------------*/
void DRPC_ExtendedStatus(char *string)
{
	if (Playing())
	{
		if (ultimatemode)
			DRPC_StringPrintf(string, NULL, "Ultimate");
		else if (splitscreen)
			DRPC_StringPrintf(string, NULL, "Splitscreen");

		if (modeattacking)
		{
			if (maptol == TOL_NIGHTS || maptol == TOL_XMAS)
				DRPC_StringPrintf(string, " ", "NiGHTS Mode");
			else if (marathonmode)
				DRPC_StringPrintf(string, " ", "Marathon Mode");
			else
				DRPC_StringPrintf(string, " ", "Time Attack");
		}
		else
		{
			if (gametype == GT_COOP && !netgame)
				DRPC_StringPrintf(string, " ", "Singleplayer");
			else
				DRPC_StringPrintf(string, " ", gametype_cons_t[gametype].strvalue);

			if (!players[consoleplayer].spectator && gametyperules & GTR_LIVES)
			{
				if (!players[consoleplayer].lives)
					DRPC_StringPrintf(string, " | ", "Game Over...");
				else if (players[consoleplayer].lives == INFLIVES || (!cv_cooplives.value && (netgame || multiplayer)))
					DRPC_StringPrintf(string, " | ", "∞ Lives");
				else
					DRPC_StringPrintf(string, " | ", (players[consoleplayer].lives == 1 ? "%d Life" : "%d Lives"), players[consoleplayer].lives);
			}
			else if (tsourdt3rd[consoleplayer].levels.time_over)
				DRPC_StringPrintf(string, " | ", "Time Over...");

			if (players[consoleplayer].spectator && displayplayer == consoleplayer)
			{
				DRPC_StringPrintf(string, " | ", "Spectat");
				if (cv_discordstatusmemes.value)
					DRPC_StringPrintf(string, "ing ", "Air");
				else
					DRPC_StringPrintf(string, "or ", "mode");
			}
			else if (displayplayer != consoleplayer)
			{
				if (cv_discordstatusmemes.value)
					DRPC_StringPrintf(string, " | ", "Stalking");
				else
					DRPC_StringPrintf(string, " | ", "Spectating");
				DRPC_StringPrintf(string, " ", "'%s'", player_names[displayplayer]);
			}
		}

		if (menuactive)
			DRPC_StringPrintf(string, " | ", "In Menu");
		else if (paused)
			DRPC_StringPrintf(string, " | ", "Paused");
		if (gamecomplete)
			DRPC_StringPrintf(string, " | ", "Game Complete!");
	}

	if (TSoURDt3rd_Jukebox_IsPlaying())
		DRPC_StringPrintf(string, " | ", "Jukebox: '%s'", tsourdt3rd_global_jukebox->curtrack->title);
}

/*--------------------------------------------------
	void DRPC_ScoreStatus(char *string)

		Applies a Discord Rich Presence status, related to score amounts, to
		the given string.
--------------------------------------------------*/
void DRPC_ScoreStatus(char *string)
{
	if (!(playeringame[consoleplayer] && !demoplayback))
		return;
	DRPC_StringPrintf(string, " | ", "Current Score: %d", players[consoleplayer].score);
}

/*--------------------------------------------------
	void DRPC_EmblemStatus(char *string)

		Applies a Discord Rich Presence status, related to emblems, to
		the given string.
--------------------------------------------------*/
void DRPC_EmblemStatus(char *string)
{
	if (!(numemblems + numextraemblems))
	{
		DRPC_StringPrintf(string, " | ", "No Emblems Available");
		if (cv_discordstatusmemes.value)
			DRPC_StringPrintf(string, " ", "(Ha, NO EMBLEMS?)");
		return;
	}

	DRPC_StringPrintf(string, " | ", "%d/%d Emblems",
		M_CountEmblems(clientGamedata),
		(numemblems + numextraemblems));
}

/*--------------------------------------------------
	void DRPC_EmeraldStatus(char *string)

		Applies a Discord Rich Presence status, related to how many
		emeralds the user has, to the given string.
--------------------------------------------------*/
void DRPC_EmeraldStatus(char *string)
{
	UINT8 i;

	UINT16 emerald_type = (gametyperules & GTR_POWERSTONES ? players[consoleplayer].powers[pw_emeralds] : emeralds);
	UINT8 emerald_count = 0; // Help me find the emouralds!

	UINT16 match_emeralds[] = { [1] = 0, 0 };
	static tic_t emerald_time;

	if (!Playing())
		return;

	// Emerald math //
	if (!modeattacking)
	{
		// Provided by Monster Iestyn and Uncapped Plus' Fafabis :)
		for (i = 0; i < 7; i++)
		{
			if (emerald_type & 1<<i)
				emerald_count++;
		}

		if (gametyperules & GTR_POWERSTONES)
		{
			if (G_GametypeHasTeams())
			{
				for (i = 0; i >= MAXPLAYERS; i++)
				{
					if (!playeringame[i] || players[i].spectator)
						continue;
					match_emeralds[players[consoleplayer].ctfteam] |= players[i].powers[pw_emeralds];
				}
			}

			if (ALL7EMERALDS(match_emeralds[players[consoleplayer].ctfteam]) || ALL7EMERALDS(emerald_type))
				emerald_time = 20*TICRATE;

			if (--emerald_time > 0)
				emerald_count = 7;
		}
	}

	// Apply our Emeralds to our status //
	DRPC_StringPrintf(string, " | ", NULL);
	if (emerald_count == 7)
		DRPC_StringPrintf(string, NULL, "All ");
	DRPC_StringPrintf(string, NULL, va("%d Emerald", emerald_count));
	if (emerald_count != 1)
		DRPC_StringPrintf(string, "s", NULL);

	// Apply any extras to our status as well //
	switch (emerald_count)
	{
		case 0: // NO EMERALDS?
		{
			if (!cv_discordstatusmemes.value)
				return;
			DRPC_StringPrintf(string, " ", "(Ha, NO EMERALDS?)");
			break;
		}

		case 3: // Fun Fact: The subtitles in Shadow the Hedgehog emphasized "fourth",
		case 4: //	even though Jason Griffith emphasized "damn" in this sentence :p
		{
			if (!cv_discordstatusmemes.value)
				return;

			if (emerald_count == 3)
				DRPC_StringPrintf(string, ", ", "Where's");
			else
				DRPC_StringPrintf(string, ", ", "Found");
			DRPC_StringPrintf(string, NULL, " that DAMN FOURTH!");
			break;
		}

		case 7: // Goku Mode
		{
			if (!players[consoleplayer].powers[pw_super])
				return;

			if (!cv_discordstatusmemes.value)
				DRPC_StringPrintf(string, ", ", "Currently Super");
			else
				DRPC_StringPrintf(string, ", ", "In Goku Mode");
			break;
		}

		default: // Normal
		{
			if (!cv_discordstatusmemes.value)
				return;
			DRPC_StringPrintf(string, NULL, "s?");
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
			DRPC_ImagePrintf(image, "misc", "intro1");
			DRPC_StringPrintf(imagestr, NULL, "Intro");
			DRPC_StringPrintf(string, " | ", "Watching the Intro");
			break;
		}

		case GS_CUTSCENE:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", "Watching a Cutscene");
			break;
		}

		case GS_TITLESCREEN:
		case GS_LEVEL:
		case GS_INTERMISSION:
		{
			char *maptitle = NULL;

			if (gamestate == GS_TITLESCREEN || gamemap == titlemap)
			{
				if (Playing() && playeringame[consoleplayer])
				{
					DRPC_ImagePrintf(image, "misc", "titlefake");
					DRPC_StringPrintf(imagestr, NULL, "Hey y'all, Star here! What is wrong with this user?");
				}
				else
					DRPC_ImagePrintf(image, "misc", "title");
			}
			else
			{
				if (!G_BuildMapName(gamemap) || mapheaderinfo[gamemap-1]->menuflags & LF2_HIDEINMENU)
				{
					DRPC_ImagePrintf(image, "misc", "missing");
					DRPC_StringPrintf(imagestr, " | ", "???");
					break;
				}

				if (supportedMaps[gamemap])
					DRPC_ImagePrintf(image, "map", supportedMaps[gamemap]);
				else
					DRPC_ImagePrintf(image, "map", "custom");

				maptitle = G_BuildMapTitle(gamemap);
				DRPC_StringPrintf(imagestr, " | ", maptitle);
				Z_Free(maptitle);
			}

			break;
		}

		case GS_ENDING:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", "Watching the Ending");
			break;
		}

		case GS_CREDITS:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", "Viewing the Credits");
			break;
		}

		case GS_EVALUATION:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", "Evaluating Results");
			break;
		}

		case GS_GAMEEND:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			if (!cv_discordstatusmemes.value)
			{
				DRPC_StringPrintf(string, " | ", "Returning to the Main Menu...");
				if (ultimatemode)
					DRPC_StringPrintf(string, ", ", "I beat Ultimate Mode!");
			}
			else
			{
				DRPC_StringPrintf(string, " | ", "Did you get all those Chaos Emeralds?");
				if (ultimatemode)
					DRPC_StringPrintf(string, ", ", "Look guys, my greatest achievement: A 'I beat Ultimate Mode' RPC status!");
			}
			break;
		}

		case GS_CONTINUING:
		{
			DRPC_ImagePrintf(image, "misc", "title");
			DRPC_StringPrintf(imagestr, NULL, "Sonic Robo Blast 2");
			DRPC_StringPrintf(string, " | ", "Continue?");
			break;
		}

		default:
			break;
	}
}

/*--------------------------------------------------
	void DRPC_CharacterStatus(char *charimg, char *charname, char *s_charimg, char *s_charname)

		Applies a Discord Rich Presence status, related to levels, to
		the given string.
--------------------------------------------------*/
void DRPC_CharacterStatus(char *charimg, char *charname, char *s_charimg, char *s_charname)
{
	DRPC_Chars_t *g_discord_mainChar = NULL;

	char *g_discord_fill_image = ((!charimg || *charimg == '\0') ? charimg : s_charimg);
	char *g_discord_fill_string = ((!charname || *charname == '\0') ? charname : s_charname);

	const char *player_skin[] = { NULL, NULL, NULL, NULL };
	const char *player_skin_realname[] = { NULL, NULL, NULL, NULL };

	char duo_skin[64] = "";
	char super_duo_skin[64] = "";

	size_t g_discord_skins = 0;
	INT32 g_discord_players;
	INT32 g_discord_player_slot = 0;

	if ((!Playing() || !playeringame[consoleplayer] || players[consoleplayer].spectator) && !demoplayback)
		return;

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
		snprintf(duo_skin, DISCORD_PRESENCE_IMAGE_STRING_SIZE, "%s%s", player_skin[0], player_skin[1]);
		snprintf(duo_skin, DISCORD_PRESENCE_IMAGE_STRING_SIZE, "super%s", g_discord_mainChar->duoname);

		if (players[consoleplayer].powers[pw_super] || skin_copy)
		{
			if (g_discord_mainChar->super && !g_discord_mainChar->superduo)
				*duo_skin = *duo_skin + 5; // removes super
			else if (!strcmp(duo_skin, super_duo_skin))
			{
				// Allow for super duos!
				snprintf(duo_skin, DISCORD_PRESENCE_IMAGE_STRING_SIZE, "%s", super_duo_skin);
				continue;
			}
		}

		if (!strcmp(duo_skin, g_discord_mainChar->duoname))
			continue; // Apply our regular dynamic duo!

		g_discord_mainChar = NULL; // No duo found...
	}

	// Apply character images and names //
	if (g_discord_mainChar && !splitscreen)
	{
		// Apply our duo image, plus others
		if ((g_discord_fill_image && *g_discord_fill_image != '\0') && player_skin[2])
			DRPC_ImagePrintf(g_discord_fill_image, imageType[cv_discordcharacterimagetype.value], player_skin[2]);
		DRPC_ImagePrintf(g_discord_fill_image, imageType[cv_discordcharacterimagetype.value], duo_skin);

		if (!demoplayback)
			DRPC_StringPrintf(g_discord_fill_string, " | ", "Playing as: %s & %s", player_skin_realname[0], player_skin_realname[1]);
		else
			DRPC_StringPrintf(g_discord_fill_string, " | ", "Demo: %s & %s", player_skin_realname[0], player_skin_realname[1]);
	}
	else
	{
		// Apply our main images, plus others
		if ((g_discord_fill_image && *g_discord_fill_image != '\0') && player_skin[1])
			DRPC_ImagePrintf(g_discord_fill_image, imageType[cv_discordcharacterimagetype.value], player_skin[1]);
		DRPC_ImagePrintf(g_discord_fill_image, imageType[cv_discordcharacterimagetype.value], player_skin[0]);

		if (!demoplayback)
			DRPC_StringPrintf(g_discord_fill_string, " | ", "Playing as: %s", player_skin_realname[0]);
		else
			DRPC_StringPrintf(g_discord_fill_string, " | ", "Demo: %s", player_skin_realname[0]);

		if (player_skin_realname[1])
			DRPC_StringPrintf((demoplayback ? g_discord_fill_string : s_charname), NULL, " & %s", player_skin_realname[1]);
	}

	if (player_skin_realname[2])
		DRPC_StringPrintf((demoplayback ? g_discord_fill_string : s_charname), NULL, " & %s", player_skin_realname[2]);
	if (player_skin_realname[3])
		DRPC_StringPrintf((demoplayback ? g_discord_fill_string : s_charname), " + ", "Others");
}

/*--------------------------------------------------
	void DRPC_PlaytimeStatus(char *string)

		Applies a Discord Rich Presence status, related to SRB2 playtime, to
		the given string.
--------------------------------------------------*/
void DRPC_PlaytimeStatus(char *string)
{
	DRPC_StringPrintf(string, " | ", "Total Playtime: %d Hours, %d Minutes, and %d Seconds",
		G_TicsToHours(clientGamedata->totalplaytime),
		G_TicsToMinutes(clientGamedata->totalplaytime, false),
		G_TicsToSeconds(clientGamedata->totalplaytime));
}

/*--------------------------------------------------
	void DRPC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr)

		Using the customizable custom discord status commands, this applies
		a Discord Rich Presence status to the given string.
--------------------------------------------------*/
void DRPC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr)
{
	const char *image_index[] = { NULL, NULL };

	INT32 custom_cvartype_index[] = { cv_discordcustom_imagetype_large.value, cv_discordcustom_imagetype_small.value };
	consvar_t *custom_cvar_index[2][4] = {
		[0] = {
			&cv_discordcustom_characterimage_large,
			&cv_discordcustom_supercharacterimage_large,
			&cv_discordcustom_mapimage_large,
			&cv_discordcustom_miscimage_large,
		},
		[1] = {
			&cv_discordcustom_characterimage_small,
			&cv_discordcustom_supercharacterimage_small,
			&cv_discordcustom_mapimage_small,
			&cv_discordcustom_miscimage_small,
		},
	};

	// Determine custom images //
	for (UINT16 i = 0; !image_index[i]; i++)
	{
		switch (custom_cvartype_index[i])
		{
			case 0: // Non-Super
			case 1:
			case 2:
			case 3: // Super
			case 4:
			case 5:
				INT32 char_index = (custom_cvartype_index[i] > 2);
				char discord_char_skin[128] = "";

				sprintf(discord_char_skin, "%s", custom_cvar_index[i][char_index]->string); // converts string to char pointer
				strlwr(TSoURDt3rd_M_RemoveStringChars(discord_char_skin, " &")); // removes cases, spaces, and '&' sign

				for (size_t s = 0; supportedSkins[s].name; s++)
				{
					if (supportedSkins[s].duoname && !strcmp(supportedSkins[s].duoname, discord_char_skin))
					{
						image_index[i] = supportedSkins[s].duoname;
						break;
					}
					else if (!strcmp(supportedSkins[s].name, discord_char_skin))
					{
						image_index[i] = supportedSkins[s].name;
						break;
					}
				}
				break;

			case 6:
				image_index[i] = supportedMaps[custom_cvar_index[i][2]->value];
				break;

			case 7:
				image_index[i] = supportedMiscs[custom_cvar_index[i][3]->value];
				break;
		}
	}

	// Push our custom status! :) //
	DRPC_StringPrintf(detailstr, NULL, cv_discordcustom_details.string);
	DRPC_StringPrintf(statestr, NULL, cv_discordcustom_state.string);

	DRPC_ImagePrintf(image, imageType[custom_cvartype_index[0]], image_index[0]);
	DRPC_StringPrintf(imagestr, NULL, cv_discordcustom_imagetext_large.string);

	DRPC_ImagePrintf(s_image, imageType[custom_cvartype_index[1]], image_index[1]);
	DRPC_StringPrintf(s_imagestr, NULL, cv_discordcustom_imagetext_small.string);
}

#endif // HAVE_DISCORDSUPPORT
