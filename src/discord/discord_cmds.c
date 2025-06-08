// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2025 by Kart Krew.
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_cmds.c
/// \brief Discord status commands and data functions

#ifdef HAVE_DISCORDSUPPORT

#include "discord.h"

// ------------------------ //
//        Variables
// ------------------------ //

// ===============
// POSSIBLE VALUES
// ===============

static CV_PossibleValue_t statustype_cons_t[] = {
	{0, "Basic"},
	{1, "Default"},
	// Specific Info
	{2, "Only Characters"},
	{3, "Only Score"},
	{4, "Only Emeralds"},
	{5, "Only Emblems"},
	{6, "Only Gamestates"},
	{7, "Only Statuses"},
	{8, "Only Playtime"},
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
	{0, "C.S.S Portraits"},
	{1, "Continue Sprites"},
	{2, "Life Icon Sprites"},
	// Display the Super Sprites of Characters
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
// COMMANDS
// ========

static void DISC_CheckStringLen(void);

consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, DISC_UpdatePresence);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_YesNo, DISC_UpdatePresence);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE, CV_YesNo, NULL);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "Default", CV_SAVE|CV_CALL, statustype_cons_t, DISC_UpdatePresence);
consvar_t cv_discordcharacterimagetype = CVAR_INIT ("discordcharacterimagetype", "C.S.S Portraits", CV_SAVE, characterimagetype_cons_t, NULL);

consvar_t cv_discordcustom_details = CVAR_INIT ("discordcustom_details", "Blasting these robots!", CV_SAVE|CV_CALL, NULL, DISC_CheckStringLen);
consvar_t cv_discordcustom_state = CVAR_INIT ("discordcustom_state", "Playing Sonic Robo Blast 2!", CV_SAVE|CV_CALL, NULL, DISC_CheckStringLen);

consvar_t cv_discordcustom_imagetype_large = CVAR_INIT ("discordcustom_imagetype_large", "C.S.S Portraits", CV_SAVE, custom_imagetype_cons_t, NULL);
consvar_t cv_discordcustom_imagetype_small = CVAR_INIT ("discordcustom_imagetype_small", "Continue Sprites", CV_SAVE, custom_imagetype_cons_t, NULL);

consvar_t cv_discordcustom_characterimage_large = CVAR_INIT ("discordcustom_characterimage_large", "Sonic", CV_SAVE, custom_characterimage_cons_t, NULL);
consvar_t cv_discordcustom_characterimage_small = CVAR_INIT ("discordcustom_characterimage_small", "Tails", CV_SAVE, custom_characterimage_cons_t, NULL);

consvar_t cv_discordcustom_supercharacterimage_large = CVAR_INIT ("discordcustom_supercharacterimage_large", "Sonic", CV_SAVE, custom_supercharacterimage_cons_t, NULL);
consvar_t cv_discordcustom_supercharacterimage_small = CVAR_INIT ("discordcustom_supercharacterimage_small", "Sonic", CV_SAVE, custom_supercharacterimage_cons_t, NULL);

consvar_t cv_discordcustom_mapimage_large = CVAR_INIT ("discordcustom_mapimage_large", "GFZ1", CV_SAVE, custom_mapimage_cons_t, NULL);
consvar_t cv_discordcustom_mapimage_small = CVAR_INIT ("discordcustom_mapimage_small", "Custom Zone", CV_SAVE, custom_mapimage_cons_t, NULL);

consvar_t cv_discordcustom_miscimage_large = CVAR_INIT ("discordcustom_miscimage_large", "Title Screen", CV_SAVE, custom_miscimage_cons_t, NULL);
consvar_t cv_discordcustom_miscimage_small = CVAR_INIT ("discordcustom_miscimage_small", "Intro - 1", CV_SAVE, custom_miscimage_cons_t, NULL);

consvar_t cv_discordcustom_imagetext_large = CVAR_INIT ("discordcustom_imagetext_large", "My favorite character!", CV_SAVE|CV_CALL, NULL, DISC_CheckStringLen);
consvar_t cv_discordcustom_imagetext_small = CVAR_INIT ("discordcustom_imagetext_small", "My other favorite character!", CV_SAVE|CV_CALL, NULL, DISC_CheckStringLen);

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	static void DISC_CheckStringLen(void)

		Checks the discord custom text strings at game startup, to
		ensure that the strings have a long enough length.
--------------------------------------------------*/
static void DISC_CheckStringLen(void)
{
	consvar_t *custom_cvartyping_index[] = {
		CV_FindVar("discordcustom_details"),
		CV_FindVar("discordcustom_state"),
		CV_FindVar("discordcustom_imagetext_large"),
		CV_FindVar("discordcustom_imagetext_small"),
		NULL
	};

	for (INT32 i = 0; custom_cvartyping_index[i]; i++)
	{
		if (!custom_cvartyping_index[i] || !CV_FindVar(custom_cvartyping_index[i]->name))
			continue;
		if (!custom_cvartyping_index[i]->string || *custom_cvartyping_index[i]->string == '\0')
			continue;

		if (strlen(custom_cvartyping_index[i]->string) < 2)
			CV_Set(custom_cvartyping_index[i], custom_cvartyping_index[i]->defaultvalue);
	}
}

#endif // HAVE_DISCORDSUPPORT
