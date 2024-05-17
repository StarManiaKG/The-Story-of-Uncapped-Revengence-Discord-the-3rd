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
/// \file  discord_cmds.c
/// \brief Discord Rich Presence commands

#if defined (HAVE_DISCORDRPC) || defined (HAVE_DISCORDGAMESDK)

#include "discord.h"

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

    {0, NULL}
};

static CV_PossibleValue_t characterimagetype_cons_t[] = {
	{0, "CS Portrait"},
	{1, "Continue Sprite"},
	{2, "Life Icon Sprite"},
	{0, NULL}
};

static CV_PossibleValue_t custom_imagetype_cons_t[] = {
	{0, "CS Portraits"},
	{1, "Continue Sprites"},
	{2, "Life Icon Sprites"},

	{3, "S. CS Portraits"},
	{4, "S. Continue Sprites"},
	{5, "S. Life Icon Sprites"},

	{6, "Maps"},
	{7, "Miscellaneous"},
	{8, "None"},
	{0, NULL}
};

static CV_PossibleValue_t custom_characterimage_cons_t[] = {
    {0, "Default"},	// ...Does ghost sonic count as a vanilla char? Maybe.
    {1, "Sonic"},
    {2, "Tails"},
    {3, "Knuckles"},
    {4, "Amy"},
    {5, "Fang"},
    {6, "Metal Sonic"},
    {7, "Sonic & Tails"}, // Bots, am I right?

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

    // My Char
    {24, "Hexhog"},

    // Friendos' Char
	{25, "Speccy"},

    {0, NULL}
};

static CV_PossibleValue_t custom_supercharacterimage_cons_t[] = {
    {0, "Sonic"},
	{1, "Sonic & Tails"},
	{0, NULL}
};

static CV_PossibleValue_t custom_mapimage_cons_t[] = {
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

    {0, NULL}
};

static CV_PossibleValue_t custom_miscimage_cons_t[] = {
	{0, "Default"},

	{1, "Intro 1"},
	{2, "Intro 2"},
	{3, "Intro 3"},
	{4, "Intro 4"},
	{5, "Intro 5"},
	{6, "Intro 6"},
	{7, "Intro 7"},
	{8, "Intro 8"},

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

	{0, NULL}
};

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

#endif // #if defined (HAVE_DISCORDRPC) || defined (HAVE_DISCORDGAMESDK)
