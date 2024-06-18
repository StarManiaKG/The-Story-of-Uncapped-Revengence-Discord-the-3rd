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
/// \file  discord_cmds.c
/// \brief Discord Rich Presence commands and data functions

#include "discord_cmds.h"

#include "../STAR/star_vars.h" // TSoURDt3rd structure //

#include "../doomstat.h"
#include "../byteptr.h"
#include "../g_game.h"
#include "../d_netcmd.h"

// ------------------------ //
//        Variables
// ------------------------ //

#ifdef HAVE_DISCORDSUPPORT

// ===============
// POSSIBLE VALUES
// ===============

static CV_PossibleValue_t statustype_cons_t[] = {
	{0, "Default"},

	{1, "Only Characters"},
	{2, "Only Score"},
	{3, "Only Emeralds"},
	{4, "Only Emblems"},
	{5, "Only Gamestates"},
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

	{24, "Hexhog"},

	{25, "Speccy"},

	{0, NULL}
};

static CV_PossibleValue_t custom_supercharacterimage_cons_t[] = {
	{0, "Sonic"},
	{1, "Sonic & Tails"},
	{0, NULL}
};

static CV_PossibleValue_t custom_mapimage_cons_t[] = {
	{1, "GFZ1"}, // Supported Singleplayer/Co-op Stages (GFZ-BCZ)
	{2, "GFZ2"},
	{3, "GFZ3"},

	{4, "THZ1"},
	{5, "THZ2"},
	{6, "THZ3"},

	{7, "DSZ1"},
	{8, "DSZ2"},
	{9, "DSZ3"},

	{10, "CEZ1"},
	{11, "CEZ2"},
	{12, "CEZ3"},

	{13, "ACZ1"},
	{14, "ACZ2"},
	{15, "ACZ3"},

	{16, "RVZ1"},

	{22, "ERZ1"},
	{23, "ERZ2"},

	{25, "BCZ1"},
	{26, "BCZ2"},
	{27, "BCZ3"},

	{30, "BS - FHZ"}, // Unlockable Extra Stages
	{31, "BS - PTZ"},
	{32, "BS - FFZ"},
	{33, "BS - TLZ"},

	{40, "CS - HHZ"}, // Unlockable Advanced Stages
	{41, "CS - AGZ"},
	{42, "CS - ATZ"},

	{50, "SSS - FFZ"}, // NiGHTS Special Stages
	{51, "SSS - TPZ"},
	{52, "SSS - FCZ"},
	{53, "SSS - CFZ"},
	{54, "SSS - DWZ"},
	{55, "SSS - MCZ"},
	{56, "SSS - ESZ"},
	{57, "SSS - BHZ"},

	{60, "MSS - 1"}, // Co-op Special Stages
	{61, "MSS - 2"},
	{62, "MSS - 3"},
	{63, "MSS - 4"},
	{64, "MSS - 5"},
	{65, "MSS - 6"},
	{66, "MSS - 7"},

	{70, "NBS - CCZ"}, // Unlockable NiGHTS Stages
	{71, "NBS - DHZ"},
	{72, "NBS - APZ1"},
	{73, "NBS - APZ2"},

	{280, "CTF - LFZ"}, // CTF Stages
	{281, "CTF - LPZ"},
	{282, "CTF - SCZ"},
	{283, "CTF - IFZ"},
	{284, "CTF - TTZ"},
	{285, "CTF - CTZ"},
	{286, "CTF - ITZ"},
	{287, "CTF - DFZ"},
	{288, "CTF - NRZ"},

	{532, "MATCH - JVZ"}, // Match, Team Match, H&S, & Tag Stages
	{533, "MATCH - NFZ"},
	{534, "MATCH - TPZ"},
	{535, "MATCH - TCZ"},
	{536, "MATCH - DTZ"},
	{537, "MATCH - ICZ"},
	{538, "MATCH - OHZ"},
	{539, "MATCH - SFZ"},
	{540, "MATCH - DBZ"},
	{541, "MATCH - CSZ"},
	{542, "MATCH - FCZ"},
	{543, "MATCH - MMZ"},

	{1000, "Tutorial"}, // Tutorial Zone
	
	{10000, "Custom"},

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

// ========
// COMMANDS
// ========

consvar_t cv_discordrp = CVAR_INIT ("discordrp", "On", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordstreamer = CVAR_INIT ("discordstreamer", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_discordasks = CVAR_INIT ("discordasks", "Yes", CV_SAVE|CV_CALL, CV_OnOff, DRPC_UpdatePresence);
consvar_t cv_discordstatusmemes = CVAR_INIT ("discordstatusmemes", "Yes", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_discordshowonstatus = CVAR_INIT ("discordshowonstatus", "Default", CV_SAVE|CV_CALL, statustype_cons_t, DRPC_UpdatePresence);
consvar_t cv_discordcharacterimagetype = CVAR_INIT ("discordcharacterimagetype", "CS Portrait", CV_SAVE, characterimagetype_cons_t, NULL);


consvar_t cv_customdiscorddetails = CVAR_INIT ("customdiscorddetails", "I'm Feeling Good!", CV_SAVE, NULL, NULL);
consvar_t cv_customdiscordstate = CVAR_INIT ("customdiscordstate", "I'm Playing Sonic Robo Blast 2!", CV_SAVE, NULL, NULL);

consvar_t cv_customdiscordlargeimagetype = CVAR_INIT ("customdiscordlargeimagetype", "CS Portraits", CV_SAVE, custom_imagetype_cons_t, NULL);
consvar_t cv_customdiscordsmallimagetype = CVAR_INIT ("customdiscordsmallimagetype", "Continue Sprites", CV_SAVE, custom_imagetype_cons_t, NULL);

// Custom Discord Status Images //
consvar_t cv_customdiscordlargeimagetext = CVAR_INIT ("customdiscordlargeimagetext", "My Favorite Character!", CV_SAVE, NULL, NULL);
consvar_t cv_customdiscordsmallimagetext = CVAR_INIT ("customdiscordsmallimagetext", "My Other Favorite Character!", CV_SAVE, NULL, NULL);

// Characters
consvar_t cv_customdiscordlargecharacterimage = CVAR_INIT ("customdiscordlargecharacterimage", "Sonic", CV_SAVE, custom_characterimage_cons_t, NULL);
consvar_t cv_customdiscordsmallcharacterimage = CVAR_INIT ("customdiscordsmallcharacterimage", "Tails", CV_SAVE, custom_characterimage_cons_t, NULL);

consvar_t cv_customdiscordlargesupercharacterimage = CVAR_INIT ("customdiscordlargesupercharacterimage", "Sonic", CV_SAVE, custom_supercharacterimage_cons_t, NULL);
consvar_t cv_customdiscordsmallsupercharacterimage = CVAR_INIT ("customdiscordsmallsupercharacterimage", "Sonic", CV_SAVE, custom_supercharacterimage_cons_t, NULL);

// Maps
consvar_t cv_customdiscordlargemapimage = CVAR_INIT ("customdiscordlargemapimage", "GFZ1", CV_SAVE, custom_mapimage_cons_t, NULL);
consvar_t cv_customdiscordsmallmapimage = CVAR_INIT ("customdiscordsmallmapimage", "Custom", CV_SAVE, custom_mapimage_cons_t, NULL);
	
// Miscellanious
consvar_t cv_customdiscordlargemiscimage = CVAR_INIT ("customdiscordlargemiscimage", "Default", CV_SAVE, custom_miscimage_cons_t, NULL);
consvar_t cv_customdiscordsmallmiscimage = CVAR_INIT ("customdiscordsmallmiscimage", "Intro 1", CV_SAVE, custom_miscimage_cons_t, NULL);

#endif // HAVE_DISCORDSUPPORT

// ------------------------ //
//        Functions
// ------------------------ //

void Joinable_OnChange(void)
{
	UINT8 buf[3];
	UINT8 *p = buf;
	UINT8 maxplayer;

	if (!server)
		return;

	maxplayer = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));

	WRITEUINT8(p, maxplayer);
	WRITEUINT8(p, cv_allownewplayer.value);
	WRITEUINT8(p, cv_discordinvites.value);

	SendNetXCmd(XD_DISCORD, &buf, 3);
}

void Got_DiscordInfo(UINT8 **cp, INT32 playernum)
{
	if (playernum != serverplayer /*&& !IsPlayerAdmin(playernum)*/)
	{
		// protect against hacked/buggy client
		CONS_Alert(CONS_WARNING, M_GetText("Illegal Discord info command received from %s\n"), player_names[playernum]);
		if (server)
			SendKick(playernum, KICK_MSG_CON_FAIL);
		return;
	}

	// Don't do anything with the information if we don't have Discord RP support
#ifdef HAVE_DISCORDSUPPORT
	if (TSoURDt3rdPlayers[serverplayer].serverPlayers.serverUsesTSoURDt3rd)
	{
		discordInfo.maxPlayers = READUINT8(*cp);
		discordInfo.joinsAllowed = (boolean)READUINT8(*cp);
		discordInfo.everyoneCanInvite = (boolean)READUINT8(*cp);
	}
	else
	{
		discordInfo.maxPlayers = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
		discordInfo.joinsAllowed = cv_allownewplayer.value;
		discordInfo.everyoneCanInvite = (boolean)cv_discordinvites.value;
		(*cp) += 3;
	}

	DRPC_UpdatePresence();
#else
	(*cp) += 3;
#endif
}
