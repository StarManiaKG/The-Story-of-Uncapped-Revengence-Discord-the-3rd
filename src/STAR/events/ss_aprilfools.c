// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_aprilfools.c
/// \brief April Fools event, related functions and data

#include "../ss_cmds.h"
#include "../ss_main.h"
#include "../m_menu.h"
#include "../s_sound.h"
#include "../../g_demo.h"

#include "../../d_main.h" // autoloaded/autoloading //
#include "../../g_game.h" // playeringame[] //

// ------------------------ //
//        Variables
// ------------------------ //

enum
{
	singleplr = 0,
	multiplr,
	secrets,
	addons,
	options,
	quitdoom,

	tsourdt3rdreadme
};

enum
{
	mpause_addons = 0,
	mpause_scramble,
	mpause_hints,
	mpause_switchmap,

#ifdef HAVE_DISCORDRPC
	mpause_discordrequests, // DISCORD STUFFS: requesting to spam your inbox please //
#endif

	mpause_continue,
	mpause_psetupsplit,
	mpause_psetupsplit2,
	mpause_spectate,
	mpause_entergame,
	mpause_switchteam,
	mpause_psetup,
	mpause_options,

	mpause_title,
	mpause_quit
};

enum
{
	spause_addons = 0,

	spause_pandora,
	spause_hints,
	spause_levelselect,

	spause_continue,
	spause_retry,
	spause_options,

	spause_title,
	spause_quit
};

static void STAR_AprilFools_OnChange(void);

// ------------------------ //
//        Commands
// ------------------------ //

consvar_t cv_aprilfools_ultimatemode = CVAR_INIT ("aprilfools_ultimatemode", "Off", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, STAR_AprilFools_OnChange);

// ------------------------ //
//        Functions
// ------------------------ //

// boolean TSoURDt3rd_InAprilFoolsMode(void)
// Checks If TSoURDt3rd is in April Fools Mode, and Returns True if so
//
boolean TSoURDt3rd_InAprilFoolsMode(void)
{
	return (aprilfoolsmode && cv_aprilfools_ultimatemode.value);
}

static void STAR_AprilFools_ChangeMenus(void)
{
	if (!aprilfoolsmode)
		return;

	if (menuactive)
		D_StartTitle();
	M_ClearMenus(true);

	if (demoplayback && titledemo)
		G_CheckDemoStatus();

	OP_Tsourdt3rdOptionsMenu[op_isitcalledsingleplayer].status =
		(!cv_aprilfools_ultimatemode.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	if (cv_aprilfools_ultimatemode.value)
	{
		// Main Menu //
		MainMenu[singleplr].text				= "No Friends Mode";
		MainMenu[multiplr].text					= "The Friend Zone";
		MainMenu[secrets].text					= "More Stuff";
		MainMenu[addons].text					= "Mods";
		MainMenu[options].text					= "Settings";
		MainMenu[quitdoom].text					= "EXIT TO DOS";

		MainMenu[tsourdt3rdreadme].text 		= "DOOM EASTER EGG THING!";

		// Multiplayer Pause Menu //
		MPauseMenu[mpause_addons].text			= "Plugins...";
		MPauseMenu[mpause_scramble].text		= "Scramble Groups...";
		MPauseMenu[mpause_hints].text			= SPauseMenu[spause_hints].text;
		MPauseMenu[mpause_switchmap].text		= "Can We Play Tag?";

#ifdef HAVE_DISCORDRPC
		MPauseMenu[mpause_discordrequests].text	= "Facebook Requests..."; // DISCORD STUFFS: set the request menu text please //
#endif

		MPauseMenu[mpause_continue].text		= "Keep Going";

		MPauseMenu[mpause_psetupsplit].text		= "Pet 1 Setup";
		MPauseMenu[mpause_psetupsplit2].text	= "Pet 2 Setup";

		MPauseMenu[mpause_spectate].text		= "Watching From The Walls";
		MPauseMenu[mpause_entergame].text		= "Enter Playground";
		MPauseMenu[mpause_switchteam].text		= "Join Group...";
		MPauseMenu[mpause_psetup].text			= "Customise Pet";

		MPauseMenu[mpause_options].text			= MainMenu[options].text;

		MPauseMenu[mpause_title].text			= "Leave Group";
		MPauseMenu[mpause_quit].text			= MainMenu[quitdoom].text;

		// Single Player Pause Menu //
		SPauseMenu[spause_addons].text 			= "Mods";

		SPauseMenu[spause_pandora].text			= "Enable Hacks";
		SPauseMenu[spause_hints].text			= "where are the emblems help";
		SPauseMenu[spause_levelselect].text		= "What Map??";

		SPauseMenu[spause_continue].text		= "Keep Going";
		SPauseMenu[spause_retry].text			= "Try Again";
		SPauseMenu[spause_options].text			= "Settings";

		SPauseMenu[spause_title].text			= "Bored Already";
		SPauseMenu[spause_quit].text			= MainMenu[quitdoom].text;

		return;
	}

	memmove(&MainMenu, &defaultMenuTitles[1], sizeof(menuitem_t));		// Main Menu
	memmove(&MPauseMenu, &defaultMenuTitles[2], sizeof(menuitem_t));	// Multiplayer Pause Menu
	memmove(&SPauseMenu, &defaultMenuTitles[3], sizeof(menuitem_t));	// Single Player Pause Menu

	STAR_StoreDefaultMenuStrings();
}

static void STAR_AprilFools_OnChange(void)
{
	if (!aprilfoolsmode)
		return;
	STAR_AprilFools_ChangeMenus();

	if (Playing() || playeringame[consoleplayer])
	{
		if (TSoURDt3rd_InAprilFoolsMode() && cursaveslot > NOSAVESLOT && !netgame)
		{
			STAR_CONS_Printf(STAR_CONS_APRILFOOLS, "You have the April Fools features enabled.\nTherefore, to prevent dumb things from happening,\nthis savefile will not save until you turn this mode off.\n");
			M_StartMessage(va("%c%s\x80\nYou have the April Fools features enabled.\nTherefore, to prevent dumb things from happening,\nthis savefile will not save until you turn this mode off.\n(Press any key to continue.)\n", ('\x80' + (V_MENUCOLORMAP|V_CHARCOLORSHIFT)), "TSoURDt3rd Notice"),NULL,MM_NOTHING);

			cursaveslot = NOSAVESLOT;
		}

		M_ResetJukebox(false);
		strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);

		mapmusname[6] = 0;
		mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
		mapmusposition = mapheaderinfo[gamemap-1]->muspos;

		S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
	}
}
