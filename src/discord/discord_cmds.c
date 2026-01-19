// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
// Copyright (C) 2018-2025 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_cmds.c
/// \brief Discord status commands and data functions

#ifdef HAVE_DISCORDSUPPORT

#include "discord.h"

#include "../STAR/menus/smkg-m_sys.h"

// ===============
// Possible Values
// ===============

static CV_PossibleValue_t presence_cons_t[] = {
	{0, "Off"},
	{1, "Basic"},
	{2, "On"},
	{0, NULL}
};

static CV_PossibleValue_t statustype_cons_t[] = {
	// General
	{0, "Basic"},
	{1, "Default"},
	// Specific Info
	{2, "Characters"},
	{3, "Score"},
	{4, "Emeralds"},
	{5, "Emblems"},
	{6, "Gamestate Info"},
	{7, "Player Info"},
	{8, "Playtime"},
	// Custom
	{9, "Custom"},
	{0, NULL}
};

static CV_PossibleValue_t characterimagetype_cons_t[] = {
	{0, "C.S.S Portraits"},
	{1, "Continue Sprites"},
	{2, "Life Icon Sprites"},
	{0, NULL}
};

static CV_PossibleValue_t custom_imagetype_cons_t[] = {
	// Display the sprites of characters
	{0, "C.S.S Portraits"},
	{1, "Continue Sprites"},
	{2, "Life Icon Sprites"},
	// Display the super sprites of characters
	{3, "Super C.S.S"},
	{4, "Super Continue Sprites"},
	{5, "Super Life Icon Sprites"},
	// Display various world images
	{6, "Maps"},
	{7, "Miscellaneous"},
	// NULL
	{8, "None"},
	{0, NULL}
};

static CV_PossibleValue_t custom_characterimage_cons_t[] = {
	// Vanilla skins
	{0, "Sonic"},
	{1, "Sonic & Tails"},
	{2, "Tails"},
	{3, "Knuckles"},
	{4, "Amy"},
	{5, "Fang"},
	{6, "Metal Sonic"},
	// Custom skins
	{7, "Adventure Sonic"},
	{8, "Shadow"},
	{9, "Skip"},
	{10, "Jana"},
	{11, "Surge"},
	{12, "Cacee"},
	{13, "Milne"},
	{14, "Maimy"},
	{15, "Mario"},
	{16, "Luigi"},
	{17, "Blaze"},
	{18, "Marine"},
	{19, "Tails Doll"},
	{20, "Metal Knuckles"},
	{21, "Smiles"},
	{22, "Whisper"},
	// Skins I've made
	{23, "Hexhog"},
	// Skins my friends have made
	{24, "Speccy"},
	{0, NULL}
};

static CV_PossibleValue_t custom_supercharacterimage_cons_t[] = {
	{0, "Sonic"},
	{1, "Sonic & Tails"},
	{0, NULL}
};

static CV_PossibleValue_t custom_mapimage_cons_t[] = {
	// Supported Singleplayer/Co-op Stages (GFZ-BCZ)
	{1, "GFZ1"}, {2, "GFZ2"}, {3, "GFZ3"},
	{4, "THZ1"}, {5, "THZ2"}, {6, "THZ3"},
	{7, "DSZ1"}, {8, "DSZ2"}, {9, "DSZ3"},
	{10, "CEZ1"}, {11, "CEZ2"}, {12, "CEZ3"},
	{13, "ACZ1"}, {14, "ACZ2"}, {15, "ACZ3"},
	{16, "RVZ1"},
	{22, "ERZ1"}, {23, "ERZ2"},
	{25, "BCZ1"}, {26, "BCZ2"}, {27, "BCZ3"},
	// Unlockable Extra Stages
	{30, "EX - FHZ"},
	{31, "EX - PTZ"},
	{32, "EX - FFZ"},
	{33, "EX - TLZ"},
	// Unlockable Advanced Stages
	{40, "ADV - HHZ"},
	{41, "ADV - AGZ"},
	{42, "ADV - ATZ"},
	// NiGHTS Special Stages
	{50, "NiGHTS - FFZ"},
	{51, "NiGHTS - TPZ"},
	{52, "NiGHTS - FCZ"},
	{53, "NiGHTS - CFZ"},
	{54, "NiGHTS - DWZ"},
	{55, "NiGHTS - MCZ"},
	{56, "NiGHTS - ESZ"},
	{57, "NiGHTS - BHZ"},
	// Co-op Special Stages
	{60, "MSS - 1"},
	{61, "MSS - 2"},
	{62, "MSS - 3"},
	{63, "MSS - 4"},
	{64, "MSS - 5"},
	{65, "MSS - 6"},
	{66, "MSS - 7"},
	// Unlockable NiGHTS Stages
	{70, "NiGHTS - CCZ"},
	{71, "NiGHTS - DHZ"},
	{72, "NiGHTS - APZ1"},
	{73, "NiGHTS - APZ2"},
	// CTF Stages
	{280, "CTF - LFZ"},
	{281, "CTF - LPZ"},
	{282, "CTF - SCZ"},
	{283, "CTF - IFZ"},
	{284, "CTF - TTZ"},
	{285, "CTF - CTZ"},
	{286, "CTF - ITZ"},
	{287, "CTF - DFZ"},
	{288, "CTF - NRZ"},
	// Match, Team Match, H&S, & Tag Stages
	{532, "RINGSLINGER - JVZ"},
	{533, "RINGSLINGER - NFZ"},
	{534, "RINGSLINGER - TPZ"},
	{535, "RINGSLINGER - TCZ"},
	{536, "RINGSLINGER - DTZ"},
	{537, "RINGSLINGER - ICZ"},
	{538, "RINGSLINGER - OHZ"},
	{539, "RINGSLINGER - SFZ"},
	{540, "RINGSLINGER - DBZ"},
	{541, "RINGSLINGER - CSZ"},
	{542, "RINGSLINGER - FCZ"},
	{543, "RINGSLINGER - MMZ"},
	// Tutorial Zone
	{1000, "Tutorial Zone"},
	// NULL Zone
	{10000, "Custom Zone"},
	{0, NULL}
};

static CV_PossibleValue_t custom_miscimage_cons_t[] = {
	{0, "Title Screen"},
	// Intro scenes
	{1, "Intro - 1"},
	{2, "Intro - 2"},
	{3, "Intro - 3"},
	{4, "Intro - 4"},
	{5, "Intro - 5"},
	{6, "Intro - 6"},
	{7, "Intro - 7"},
	{8, "Intro - 8"},
	// Characters
	{9, "Sonic - 1"}, {10, "Sonic - 2"},
		{11, "Sonic - 3"}, {12, "Sonic - 4"},
		{13, "Sonic - 5"},
	{14, "Tails - 1"}, {15, "Tails - 2"},
	{16, "Knuckles - 1"}, {17, "Knuckles - 2"},
	{18, "Amy"},
	{19, "Fang"},
	{20, "Metal Sonic - 1"}, {21, "Metal Sonic - 2"},
	{22, "Eggman"},
	{0, NULL}
};

// ========
// Commands
// ========

static void DiscordIntegration_OnChange(void);
static boolean DiscordCustomString_CanUpdate(const char *valstr);

#define DISCORD_CVAR_INIT_UPDATE_PRESENCE(name, defaultstring, possiblevalue) CVAR_INIT (name, defaultstring, CV_SAVE|CV_CALL|CV_NOINIT, possiblevalue, DISC_UpdatePresence)
#define DISCORD_CVAR_INIT_WITH_CALLBACKS(name, defaultstring, possiblevalue, can_change) CVAR_INIT_WITH_CALLBACKS (name, defaultstring, CV_SAVE|CV_CALL|CV_NOINIT, possiblevalue, DISC_UpdatePresence, can_change)

consvar_t cv_discordintegration = CVAR_INIT ("discordintegration", "On", CV_SAVE|CV_CALL|CV_NOINIT, presence_cons_t, DiscordIntegration_OnChange);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE, CV_YesNo, NULL);
consvar_t cv_discordstatusmemes = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordstatusmemes", "Yes", CV_YesNo);
consvar_t cv_discordshowonstatus = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordshowonstatus", "Default", statustype_cons_t);
consvar_t cv_discordcharacterimagetype = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcharacterimagetype", "C.S.S Portraits", characterimagetype_cons_t);

consvar_t cv_discordcustom_details = DISCORD_CVAR_INIT_WITH_CALLBACKS ("discordcustom_details", TSOURDT3RD_SRB2_APP_FULL, NULL, DiscordCustomString_CanUpdate);
consvar_t cv_discordcustom_state = DISCORD_CVAR_INIT_WITH_CALLBACKS ("discordcustom_state", "Blasting these robots!", NULL, DiscordCustomString_CanUpdate);

consvar_t cv_discordcustom_imagetype_large = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_imagetype_large", "C.S.S Portraits", custom_imagetype_cons_t);
consvar_t cv_discordcustom_imagetype_small = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_imagetype_small", "Continue Sprites", custom_imagetype_cons_t);

consvar_t cv_discordcustom_characterimage_large = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_characterimage_large", "Sonic", custom_characterimage_cons_t);
consvar_t cv_discordcustom_characterimage_small = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_characterimage_small", "Tails", custom_characterimage_cons_t);

consvar_t cv_discordcustom_supercharacterimage_large = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_supercharacterimage_large", "Sonic", custom_supercharacterimage_cons_t);
consvar_t cv_discordcustom_supercharacterimage_small = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_supercharacterimage_small", "Sonic", custom_supercharacterimage_cons_t);

consvar_t cv_discordcustom_mapimage_large = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_mapimage_large", "GFZ1", custom_mapimage_cons_t);
consvar_t cv_discordcustom_mapimage_small = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_mapimage_small", "Custom Zone", custom_mapimage_cons_t);

consvar_t cv_discordcustom_miscimage_large = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_miscimage_large", "Title Screen", custom_miscimage_cons_t);
consvar_t cv_discordcustom_miscimage_small = DISCORD_CVAR_INIT_UPDATE_PRESENCE ("discordcustom_miscimage_small", "Intro - 1", custom_miscimage_cons_t);

consvar_t cv_discordcustom_imagetext_large = DISCORD_CVAR_INIT_WITH_CALLBACKS ("discordcustom_imagetext_large", "My favorite character!", NULL, DiscordCustomString_CanUpdate);
consvar_t cv_discordcustom_imagetext_small = DISCORD_CVAR_INIT_WITH_CALLBACKS ("discordcustom_imagetext_small", "My other favorite character!", NULL, DiscordCustomString_CanUpdate);

/*--------------------------------------------------
	static boolean DiscordIntegration_OnChange(void)

		Ran whenever the console variable for
		Discord Integration is updated.

		Credit to SRB2Kart Saturn for the idea!
--------------------------------------------------*/
static void DiscordIntegration_OnChange(void)
{
	if (dedicated)
	{
		return;
	}
	else if (discordInfo.initialized == false)
	{
		return;
	}

	if (cv_discordintegration.value)
	{
		DISC_Initialize();
		DISC_UpdatePresence();
		DISC_RunCallbacks();
	}
	else
	{
		DISC_Quit();
	}
}

/*--------------------------------------------------
	static boolean DiscordCustomString_CanUpdate(const char *valstr)

		Checks the given discord custom text string, to
		ensure that the strings meet Discord's string
		length requirements.
--------------------------------------------------*/
static boolean DiscordCustomString_CanUpdate(const char *valstr)
{
	const char *basic_string = "Sorry, Discord requires status strings to\nbe %s than %d characters.\n\nPlease type a %s string.";
	const char *full_string;

	if (valstr == NULL)
	{
		return false;
	}

	const size_t len = strlen(valstr);

	if (len < DISC_STATUS_MIN_STRING_SIZE) // YOU'RE TYPING TOO SHORT
	{
		full_string = va(basic_string, "longer", DISC_STATUS_MIN_STRING_SIZE, "longer");
		S_StartSoundFromEverywhere(sfx_skid);
		TSoURDt3rd_M_StartPlainMessage("String too short!", full_string);
		return false;
	}
	else if (len >= DISC_STATUS_MAX_STRING_SIZE) // YOU'RE TYPING TOO LONG
	{
		full_string = va(basic_string, "shorter", DISC_STATUS_MAX_STRING_SIZE, "shorter");
		S_StartSoundFromEverywhere(sfx_skid);
		TSoURDt3rd_M_StartPlainMessage("String too long!", full_string);
		return false;
	}
	return true;
}

#endif // HAVE_DISCORDSUPPORT
