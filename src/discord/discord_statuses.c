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
/// \file  discord_statuses.c
/// \brief Discord integration - statuses

#ifdef HAVE_DISCORDSUPPORT

#include "discord.h"

#include "../m_cond.h" // queries about emblems
#include "../g_game.h"
#include "../z_zone.h"

#include "../STAR/star_vars.h" // TSoURDt3rd struct //
#include "../STAR/core/smkg-s_jukebox.h" // TSoURDt3rd_Jukebox_IsPlaying() //
#include "../STAR/menus/smkg-m_sys.h" // queries about level list modes //
#include "../STAR/misc/smkg-m_misc.h" // TSoURDt3rd_M_RemoveStringChars() //

// ------------------------ //
//        Variables
// ------------------------ //

#define DISC_MAX_SKINS_PER_STATUS_TYPE 2
#define DISC_MAX_STATUS_SKINS 4

typedef struct
{
	const char *name;
	const char *duoname;
	boolean super;
	boolean superduo;
} DISC_Chars_t;

static const char *DISC_imageType[] = {
	// Character sprites
	"char",
	"cont",
	"life",
	// Super character sprites
	"charsuper",
	"contsuper",
	"lifesuper",
	// Various world data
	"map",
	"misc",
	NULL
};

static DISC_Chars_t DISC_supportedSkins[] = {
	// Vanilla skins
	{"sonic",          "sonictails",   true,	true},
	{"tails",          NULL,  		  false,   false},
	{"knuckles",       NULL,          false,   false},
	{"amy",            NULL,          false,   false},
	{"fang",           NULL,          false,   false},
	{"metalsonic",     NULL,          false,   false},
	// Custom skins
	{"adventuresonic", NULL,          false,   false},
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
	// Skins I've made
	{"hexhog",         NULL,          false,   false},
	// Skins my friends have made
	{"smiles",         NULL,          false,   false},
	{"speccy",         NULL,          false,   false},
	// DRPC CHARS END!
	{NULL,             NULL,          false,   false},
	//"custom",		// ...Does ghost sonic count as a vanilla char? Maybe, but probably not.
};

static const char *DISC_supportedMaps[] = {
	// Supported Singleplayer/Co-op Stages (GFZ-BCZ)
	[1] = "01", "02", "03",
	"04", "05", "06",
	"07", "08", "09",
	"10", "11", "12",
	"13", "14", "15",
	"16",
	[22] = "22", "23",
	[25] = "25", "26", "27",
	// Unlockable Extra Stages
	[30] = "30",
	"31",
	"32",
	"33",
	// Unlockable Advanced Stages
	[40] = "40",
	"41",
	"42",
	// NiGHTS Special Stages
	[50] = "50",
	"51",
	"52",
	"53",
	"54",
	"55",
	"56",
	"57",
	// Co-op Special Stages
	[60] = "60",
	"61",
	"62",
	"63",
	"64",
	"65",
	"66",
	// Unlockable NiGHTS Stages
	[70] = "70",
	"71",
	"72",
	"73",
	// CTF Stages
	[280] = "f0",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	// Match, Team Match, H&S, & Tag Stages
	[532] = "m0",
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
	// Tutorial Zone
	[1000] = "z0",
	// NULL Zone
	[10000] = "custom",
	NULL
};

static const char *DISC_supportedMiscs[] = {
	"title",
	// Intro scenes
	"intro1",
	"intro2",
	"intro3",
	"intro4",
	"intro5",
	"intro6",
	"intro7",
	"intro8",
	// Characers
	"altsonicimage1", "altsonicimage2",
		"altsonicimage3", "altsonicimage4",
		"altsonicimage5",
	"alttailsimage1", "alttailsimage2",
	"altknucklesimage1", "altknucklesimage2",
	"altamyimage1",
	"altfangimage1",
	"altmetalsonicimage1", "altmetalsonicimage2",
	"alteggmanimage1",
	NULL
};

static consvar_t *custom_cvartype_index[] = {
	&cv_discordcustom_imagetype_large,
	&cv_discordcustom_imagetype_small
};
static consvar_t *custom_cvar_index[2][8] = {
	[0] = {
		&cv_discordcustom_characterimage_large, &cv_discordcustom_characterimage_large, &cv_discordcustom_characterimage_large,
		&cv_discordcustom_supercharacterimage_large, &cv_discordcustom_supercharacterimage_large, &cv_discordcustom_supercharacterimage_large,
		&cv_discordcustom_mapimage_large,
		&cv_discordcustom_miscimage_large,
	},
	[1] = {
		&cv_discordcustom_characterimage_small, &cv_discordcustom_characterimage_small, &cv_discordcustom_characterimage_small,
		&cv_discordcustom_supercharacterimage_small, &cv_discordcustom_supercharacterimage_small, &cv_discordcustom_supercharacterimage_small,
		&cv_discordcustom_mapimage_small,
		&cv_discordcustom_miscimage_small,
	},
};

// ------------------------ //
//        Functions
// ------------------------ //

// =====
// TOOLS
// =====

/*--------------------------------------------------
	void DISC_StatusPrintf(boolean is_image, char *string, const char *sep, const char *fmt, ...)

		See header file for description.
--------------------------------------------------*/
void DISC_StatusPrintf(boolean is_image, char *string, const char *sep, const char *fmt, ...)
{
	va_list argptr;
	char *status_txt = malloc(8192);
	size_t size = (is_image ? DISC_STATUS_MAX_IMAGE_STRING_SIZE : DISC_STATUS_MAX_STRING_SIZE);

	if (status_txt == NULL)
		return;

	if (string == NULL) goto conclude;
	if (is_image == false)
	{
		if (sep != NULL && *string != '\0')
		{
			// Give up your children, seperate
			strlcat(string, sep, size);
		}
	}
	else
	{
		if (sep == NULL || *sep == '\0') goto conclude;
	}
	if (fmt == NULL || *fmt == '\0') goto conclude;

	va_start(argptr, fmt);
	vsnprintf(status_txt, size, fmt, argptr);
	va_end(argptr);

	if (is_image)
		snprintf(string, size, "%s%s", sep, status_txt);
	else
		strlcat(string, status_txt, size);
	goto conclude;

	conclude:
	{
		free(status_txt);
		return;
	}
}

// ========
// STATUSES
// ========

/*--------------------------------------------------
	void DISC_BasicStatus(char *string, char *image, char *imagestr)

		See header file for description.
--------------------------------------------------*/
void DISC_BasicStatus(char *string, char *image, char *imagestr)
{
	if (demoplayback || titledemo)
		DISC_StatusPrintf(false, string, " | ", (titledemo ? "Watching a Demo" : "Watching Replays"));
	else if (Playing())
	{
		if (gamecomplete && !netgame)
			DISC_StatusPrintf(false, string, " | ", "Game Complete!");
		DISC_StatusPrintf(false, string, " | ", "Active");
	}
	else
		DISC_StatusPrintf(false, string, " | ", "Inactive");

	if (paused)
		DISC_StatusPrintf(false, string, " | ", "Paused");
	if (menuactive)
		DISC_StatusPrintf(false, string, " | ", "Menu");

	switch (gamestate)
	{
		case GS_TIMEATTACK:
			if (tsourdt3rd_levellistmode == TSOURDT3RD_LLM_NIGHTSATTACK)
			{
				DISC_StatusPrintf(true, image, "misc", "nights");
				DISC_StatusPrintf(false, imagestr, NULL, "NiGHTs Attack");
				DISC_StatusPrintf(false, string, ": ", "NiGHTs Attack");
			}
			else if (marathonmode)
			{
				DISC_StatusPrintf(true, image, "misc", "record");
				DISC_StatusPrintf(false, imagestr, NULL, "Marathon Mode");
				DISC_StatusPrintf(false, string, ": ", "Marathon Mode");
			}
			else
			{
				DISC_StatusPrintf(true, image, "misc", "record");
				DISC_StatusPrintf(false, imagestr, NULL, "Time Attack");
				DISC_StatusPrintf(false, string, ": ", "Time Attack");
			}
			break;
		case GS_TITLESCREEN:
			DISC_StatusPrintf(false, imagestr, NULL, "Title Screen");
			DISC_StatusPrintf(false, string, " | ", "Title Screen");
			break;
		case GS_NULL:
			DISC_StatusPrintf(false, string, " | ", "Loading... Loading... Loading?");
			break;
		default:
			break;
	}

	if (TSoURDt3rd_Jukebox_IsPlaying())
		DISC_StatusPrintf(false, string, " | ", "Jukebox: '%s'", tsourdt3rd_global_jukebox->curtrack->title);

	if (*image == '\0') DISC_StatusPrintf(true, image, "misc", "title");
	if (*imagestr == '\0') DISC_StatusPrintf(false, imagestr, NULL, TSOURDT3RD_SRB2_APP_FULL);
}

/*--------------------------------------------------
	void DISC_PlayerStatus(char *string)

		See header file for description.
--------------------------------------------------*/
void DISC_PlayerStatus(char *string)
{
	player_t *player = &players[consoleplayer];

	if (!(Playing() && playeringame[consoleplayer] && player))
		return;

	DISC_StatusPrintf(false, string, " |", NULL);

	if (ultimatemode)
		DISC_StatusPrintf(false, string, " ", "Ultimate");
	else if (splitscreen)
		DISC_StatusPrintf(false, string, " ", "Splitscreen");

	if (modeattacking)
	{
		if (maptol == TOL_NIGHTS || maptol == TOL_XMAS)
			DISC_StatusPrintf(false, string, " ", "NiGHTS Mode");
		else if (marathonmode)
			DISC_StatusPrintf(false, string, " ", "Marathon Mode");
		else
			DISC_StatusPrintf(false, string, " ", "Time Attack");
	}
	else
	{
		if (gametype == GT_COOP && !netgame)
			DISC_StatusPrintf(false, string, " ", "Singleplayer");
		else
			DISC_StatusPrintf(false, string, " ", gametype_cons_t[gametype].strvalue);

		if (!player->spectator && gametyperules & GTR_LIVES)
		{
			if (!player->lives)
				DISC_StatusPrintf(false, string, " | ", "Game Over...");
			else if (player->lives == INFLIVES || (!cv_cooplives.value && (netgame || multiplayer)))
				DISC_StatusPrintf(false, string, " | ", "∞ Lives");
			else
				DISC_StatusPrintf(false, string, " | ", (player->lives == 1 ? "%d Life" : "%d Lives"), player->lives);
		}
		else if (tsourdt3rd[consoleplayer].game.time_over)
			DISC_StatusPrintf(false, string, " | ", "Time Over...");

		if (player->spectator && displayplayer == consoleplayer)
		{
			const char *spectate_str = (cv_discordstatusmemes.value ? "Spectating Air" : "In Spectator Mode");
			DISC_StatusPrintf(false, string, " | ", spectate_str);
		}
		else if (displayplayer != consoleplayer)
		{
			const char *spectating_str = (cv_discordstatusmemes.value ? "Stalking" : "Spectating");
			DISC_StatusPrintf(false, string, " | ", "%s: '%s'", spectating_str, player_names[displayplayer]);
		}
	}
}

/*--------------------------------------------------
	void DISC_ScoreStatus(char *string)

		See header file for description.
--------------------------------------------------*/
void DISC_ScoreStatus(char *string)
{
	if (!(Playing() && playeringame[consoleplayer]) || demoplayback) return;
	DISC_StatusPrintf(false, string, " | ", "Score: %d", players[consoleplayer].score);
}

/*--------------------------------------------------
	void DISC_EmblemStatus(char *string)

		See header file for description.
--------------------------------------------------*/
void DISC_EmblemStatus(char *string)
{
	INT32 user_emblems = M_CountEmblems(clientGamedata);
	INT32 total_emblems = (numemblems + numextraemblems);
	if (total_emblems <= 0)
		DISC_StatusPrintf(false, string, " | ", "No Emblems Available...");
	else
		DISC_StatusPrintf(false, string, " | ", "%d/%d Emblems", user_emblems, total_emblems);
	if (cv_discordstatusmemes.value && (user_emblems <= 0))
		DISC_StatusPrintf(false, string, " ", "(Ha, NO EMBLEMS?)");
}

/*--------------------------------------------------
	void DISC_EmeraldStatus(char *string)

		See header file for description.
--------------------------------------------------*/
void DISC_EmeraldStatus(char *string)
{
	UINT16 emerald_bitflags = 0, individual_emeralds = 0; // Help me find the emouralds!
	UINT8 i;

	if (!(Playing() && playeringame[consoleplayer]) || modeattacking)
		return;

	// Get the current amount of emerald bits we have... //
	if (gametyperules & GTR_POWERSTONES)
	{
		static tic_t emerald_time = 20*TICRATE;
		static boolean multiplayer_super_active = false;

		if (G_GametypeHasTeams())
		{
			for (i = 0; i < MAXPLAYERS; i++)
				if (players[i].ctfteam == players[consoleplayer].ctfteam)
					emerald_bitflags |= players[i].powers[pw_emeralds];
		}
		else
			emerald_bitflags |= players[consoleplayer].powers[pw_emeralds];

		if (ALL7EMERALDS(emerald_bitflags) && !multiplayer_super_active)
		{
			emerald_time = 20*TICRATE;
			multiplayer_super_active = true;
		}

		emerald_time--;
		if (emerald_time <= 0)
		{
			multiplayer_super_active = false;
			emerald_bitflags = 0;
		}
	}
	else
		emerald_bitflags = emeralds;

	// Calculate how many individual emeralds we have! //
	// -- Math provided by Monster Iestyn and the creator of Uncapped Plus, Fafabis :)
	for (i = 0; i < 7; i++)
		if (emerald_bitflags & 1<<i)
			individual_emeralds++;

	// Apply our Emeralds to our status //
	DISC_StatusPrintf(false, string, " | ", "%s %d Emerald%s",
		(individual_emeralds == 7 ? "All" : ""), individual_emeralds,
		(individual_emeralds != 1 ? "s" : ""));

	// Apply any extra emerald junk to our status as well! //
	switch (individual_emeralds)
	{
		case 0: // NO EMERALDS?
			if (!cv_discordstatusmemes.value) return;
			DISC_StatusPrintf(false, string, " ", "(Ha, NO EMERALDS?)");
			break;
		case 3: // Fun Fact: The subtitles in Shadow the Hedgehog emphasized "fourth",
		case 4: //	even though Jason Griffith emphasized "damn" in this sentence :p
			if (!cv_discordstatusmemes.value) return;
			DISC_StatusPrintf(false, string, ", ", (individual_emeralds == 3 ? "Where's" : "Found"));
			DISC_StatusPrintf(false, string, NULL, " that DAMN FOURTH!");
			break;
		case 7: // Goku Mode
			if (!players[consoleplayer].powers[pw_super]) return;
			DISC_StatusPrintf(false, string, ", ", (!cv_discordstatusmemes.value ? "Currently Super" : "In Goku Mode"));
			break;
		default:
			break;
	}
}

/*--------------------------------------------------
	void DISC_GamestateStatus(char *string, char *image, char *imagestr)

		See header file for description.
--------------------------------------------------*/
void DISC_GamestateStatus(char *string, char *image, char *imagestr)
{
	switch (gamestate)
	{
		case GS_INTRO:
			DISC_StatusPrintf(true, image, "misc", "intro1");
			DISC_StatusPrintf(false, imagestr, NULL, "Intro");
			DISC_StatusPrintf(false, string, " | ", "Watching the Intro");
			break;
		case GS_CUTSCENE:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", "Watching a Cutscene");
			break;
		case GS_TITLESCREEN:
		case GS_LEVEL:
		case GS_INTERMISSION:
			if (gamestate == GS_TITLESCREEN || gamemap == titlemap)
			{
				if (Playing() && playeringame[consoleplayer])
				{
					DISC_StatusPrintf(true, image, "misc", "titlefake");
					DISC_StatusPrintf(false, imagestr, NULL, "Hey y'all, Star here! What is wrong with this user?");
				}
				else
					DISC_StatusPrintf(true, image, "misc", "title");
			}
			else
			{
				if (!G_BuildMapName(gamemap) || mapheaderinfo[gamemap-1]->menuflags & LF2_HIDEINMENU)
				{
					DISC_StatusPrintf(true, image, "misc", "missing");
					DISC_StatusPrintf(false, imagestr, " | ", "???");
					break;
				}
				const char *mapimage = (DISC_supportedMaps[gamemap] ? DISC_supportedMaps[gamemap] : "custom");
				char *maptitle = G_BuildMapTitle(gamemap);
				DISC_StatusPrintf(true, image, "map", mapimage);
				DISC_StatusPrintf(false, imagestr, " | ", maptitle);
				Z_Free(maptitle);
			}
			break;
		case GS_ENDING:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", "Watching the Ending");
			break;
		case GS_CREDITS:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", "Watching the Credits");
			break;
		case GS_EVALUATION:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", "Evaluating Results");
			break;
		case GS_GAMEEND:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", NULL);
			if (!cv_discordstatusmemes.value)
			{
				DISC_StatusPrintf(false, string, NULL,
					(ultimatemode ? "I beat Ultimate Mode!" : "Returning to the Main Menu..."));
			}
			else
			{
				DISC_StatusPrintf(false, string, NULL,
					(ultimatemode ? "Look guys, my greatest achievement: An 'I beat Ultimate Mode' Discord status!" : "Did you get all those Chaos Emeralds?"));
			}
			break;
		case GS_CONTINUING:
			DISC_StatusPrintf(true, image, "misc", "title");
			DISC_StatusPrintf(false, imagestr, NULL, "Sonic Robo Blast 2; TSoURDt3rd");
			DISC_StatusPrintf(false, string, " | ", "Continue?");
			break;
		default:
			break;
	}
}

/*--------------------------------------------------
	void DISC_CharacterStatus(char *image, char *imagestr, char *s_image, char *s_imagestr)

		See header file for description.
--------------------------------------------------*/
void DISC_CharacterStatus(char *image, char *imagestr, char *s_image, char *s_imagestr)
{
	DISC_Chars_t *g_discord_mainChar = NULL;

	boolean use_large_char_image = (!image || *image == '\0');
	boolean use_large_char_string = (!imagestr || *imagestr == '\0');
	char *g_discord_fill_image = (use_large_char_image ? image : s_image);
	char *g_discord_fill_string = (use_large_char_string ? imagestr : s_imagestr);

	const char *player_skin[DISC_MAX_STATUS_SKINS];
	const char *player_skin_realname[DISC_MAX_STATUS_SKINS];

	char *skin_name = NULL;
	char *skin_name_isSuper = NULL;

	char *duo_skin = NULL;
	char *super_duo_skin = NULL;
	boolean duo_found = false;
	boolean apply_duo = false;
	INT32 duo_skin_num;

	size_t g_discord_skins = 0;
	INT32 g_discord_players = consoleplayer;
	INT32 g_discord_player_slot = 0;

	const char *playString = (demoplayback ? "Demo:" : "Playing as:");
	const char *fill_image_skin = NULL;

	if (!Playing())
		return;
	memset(player_skin, 0, sizeof(player_skin));
	memset(player_skin_realname, 0, sizeof(player_skin_realname));

	// Find character names and supported character images //
	for (; player_skin[g_discord_player_slot] == NULL; g_discord_players++, g_discord_player_slot++)
	{
		player_t *player = &players[g_discord_players];

		if (netgame && g_discord_player_slot)
		{
			g_discord_mainChar = NULL;
			break;
		}
		if (!playeringame[g_discord_players] || player == NULL || player->spectator)
		{
			// Invalid player!
			if (g_discord_players == consoleplayer)
				break;
			else
				continue;
		}

		// Initialize this skin slot!
		skin_name = strdup(skins[player->skin]->name);
		if (skin_name == NULL)
			continue;
		skin_name_isSuper = strstr(skin_name, "super");
		player_skin[g_discord_player_slot] = "custom";
		if (!splitscreen)
			player_skin_realname[g_discord_player_slot] = skins[player->skin]->realname; // Use the skin's real name!
		else
			player_skin_realname[g_discord_player_slot] = player_names[g_discord_player_slot]; // Use the player's real name!
		if (skin_name_isSuper)
		{
			// removes super from the name :)
			skin_name = (skin_name_isSuper + 5);
		}

		for (g_discord_skins = 0; DISC_supportedSkins[g_discord_skins].name; g_discord_skins++)
		{
			if (strcmp(skin_name, DISC_supportedSkins[g_discord_skins].name))
				continue; // skin wasn't found, so continue...

			if ((player->powers[pw_super] || skin_name_isSuper) && DISC_supportedSkins[g_discord_skins].super)
				skin_name = strdup(va("super%s", skin_name)); // Allow for super images!

			player_skin[g_discord_player_slot] = skin_name; // Use the supported icon!
			break;
		}

		if (!g_discord_mainChar)
		{
			if (!g_discord_player_slot && DISC_supportedSkins[g_discord_skins].duoname)
				g_discord_mainChar = &DISC_supportedSkins[g_discord_skins]; // Allow for dynamic duos!
			continue;
		}
		duo_skin = strdup(va("%s%s", player_skin[0], player_skin[1]));
		super_duo_skin = strdup(va("super%s", g_discord_mainChar->duoname));

		if (players[consoleplayer].powers[pw_super] || skin_name_isSuper)
		{
			if (g_discord_mainChar->super && !g_discord_mainChar->superduo)
			{
				// No super duo image for us...
				duo_skin = (duo_skin + 5);
			}
			else if (!strcmp(duo_skin, super_duo_skin))
			{
				// Allow for super duos!
				snprintf(duo_skin, DISC_STATUS_MAX_IMAGE_STRING_SIZE, "%s", super_duo_skin);
				continue;
			}
		}

		if (!strcmp(duo_skin, g_discord_mainChar->duoname))
		{
			// Apply our regular dynamic duo!
			duo_found = true;
			continue;
		}
		g_discord_mainChar = NULL; // -- No duo found...
	}
	if (player_skin[0] == NULL || player_skin_realname[0] == NULL)
		goto free;

	// Get our proper skin duo, if we can... //
	apply_duo = (g_discord_mainChar && !splitscreen);
	if (apply_duo)
	{
		fill_image_skin = duo_skin;
		duo_skin_num = (duo_found ? 2 : 1);
	}
	else
	{
		fill_image_skin = player_skin[0];
		duo_skin_num = 1;
	}

	// Apply character images... //
	if (player_skin[duo_skin_num])
	{
		// This is always set to the small image
		// but gets overrided by the below if the status requires it.
		DISC_StatusPrintf(true, s_image, DISC_imageType[cv_discordcharacterimagetype.value], player_skin[duo_skin_num]);
	}
	DISC_StatusPrintf(true, g_discord_fill_image, DISC_imageType[cv_discordcharacterimagetype.value], fill_image_skin);

	// Apply status strings... //
	DISC_StatusPrintf(false, g_discord_fill_string, " | ", "%s", playString);
	for (INT32 status_chars = 0; status_chars < DISC_MAX_SKINS_PER_STATUS_TYPE; status_chars++)
	{
		if (player_skin[status_chars] == NULL || player_skin_realname[status_chars] == NULL)
			continue;
		if (status_chars)
		{
			// -- More than one status character...
			if (!apply_duo) break;
			DISC_StatusPrintf(false, g_discord_fill_string, " ", "&");
		}
		DISC_StatusPrintf(false, g_discord_fill_string, " ", "%s", player_skin_realname[status_chars]);
	}

	// Apply additional status strings... //
	if (!apply_duo && player_skin_realname[1])
		DISC_StatusPrintf(false, (demoplayback ? g_discord_fill_string : s_imagestr), NULL, " & %s", player_skin_realname[1]);
	if (player_skin_realname[2])
		DISC_StatusPrintf(false, (demoplayback ? g_discord_fill_string : s_imagestr), NULL, " & %s", player_skin_realname[2]);
	if ((g_discord_player_slot >= 3) && player_skin_realname[g_discord_player_slot])
		DISC_StatusPrintf(false, (demoplayback ? g_discord_fill_string : s_imagestr), NULL, " + Others");
	goto free;

	free:
	{
		// Free any misc. data, and we're done! //
		free(skin_name);
		free(skin_name_isSuper);
		free(duo_skin);
		free(super_duo_skin);
		return;
	}
}

/*--------------------------------------------------
	void DISC_PlaytimeStatus(char *string)

		See header file for description.
--------------------------------------------------*/
void DISC_PlaytimeStatus(char *string)
{
	INT32 hours = G_TicsToHours(clientGamedata->totalplaytime);
	INT32 minutes = G_TicsToMinutes(clientGamedata->totalplaytime, false);
	INT32 seconds = G_TicsToSeconds(clientGamedata->totalplaytime);
	DISC_StatusPrintf(false, string, " | ", "Total Playtime:");
	if (hours)
		DISC_StatusPrintf(false, string, " ", "%d Hours,", hours);
	DISC_StatusPrintf(false, string, " ", "%d Minutes, %d Seconds", minutes, seconds);
}

/*--------------------------------------------------
	void DISC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr)

		See header file for description.
--------------------------------------------------*/
void DISC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr)
{
	char *image_input_index[] = { image, s_image };
	const char *image_index[] = { NULL, NULL };

	// Display a custom detail and state...
	DISC_StatusPrintf(false, detailstr, NULL, cv_discordcustom_details.string);
	DISC_StatusPrintf(false, statestr, NULL, cv_discordcustom_state.string);

	// Display custom images...
	for (UINT16 i = 0; image_index[i] == NULL; i++)
	{
		INT32 cval = custom_cvartype_index[i]->value;
		switch (cval)
		{
			case 0: case 1: case 2: // Non-Super Characters
			case 3: case 4: case 5: // Super Characters
				char discord_char_skin[DISC_STATUS_MAX_IMAGE_STRING_SIZE];
				memset(discord_char_skin, 0, sizeof(char));
				sprintf(discord_char_skin, "%s", custom_cvar_index[i][cval]->string); // converts string to char pointer
				strlwr(TSoURDt3rd_M_RemoveStringChars(discord_char_skin, " &")); // removes cases, spaces, and '&' sign
				for (size_t s = 0; DISC_supportedSkins[s].name; s++)
				{
					if (DISC_supportedSkins[s].duoname && !strcmp(DISC_supportedSkins[s].duoname, discord_char_skin))
					{
						image_index[i] = DISC_supportedSkins[s].duoname;
						break;
					}
					else if (!strcmp(DISC_supportedSkins[s].name, discord_char_skin))
					{
						image_index[i] = DISC_supportedSkins[s].name;
						break;
					}
				}
				break;
			case 6: // Maps
				image_index[i] = DISC_supportedMaps[custom_cvar_index[i][cval]->value];
				break;
			case 7: // Misc
				image_index[i] = DISC_supportedMiscs[custom_cvar_index[i][cval]->value];
				break;
		}
		DISC_StatusPrintf(true, image_input_index[i], DISC_imageType[cval], image_index[i]);
	}

	// Display custom image strings...
	DISC_StatusPrintf(false, imagestr, NULL, cv_discordcustom_imagetext_large.string);
	DISC_StatusPrintf(false, s_imagestr, NULL, cv_discordcustom_imagetext_small.string);
}

#endif // HAVE_DISCORDSUPPORT
