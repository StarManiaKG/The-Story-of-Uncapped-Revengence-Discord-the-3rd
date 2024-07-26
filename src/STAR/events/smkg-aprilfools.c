// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-aprilfools.c
/// \brief April Fools event related functions and data

#include "../smkg-cvars.h"
#include "../smkg-jukebox.h"
#include "../ss_main.h"
#include "../m_menu.h"

#include "../../g_demo.h"
#include "../../g_game.h" // playeringame[]
#include "../../v_video.h"

#include "../../d_main.h" // autoloaded/autoloading //

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_AprilFools_ModeEnabled(void)
// Returns whether or not TSoURDt3rd is in April Fools Mode.
//
boolean TSoURDt3rd_AprilFools_ModeEnabled(void)
{
	return (aprilfoolsmode && cv_tsourdt3rd_aprilfools_ultimatemode.value);
}

//
// void TSoURD3rd_AprilFools_OnChange(void)
// Routine for the main April Fools command.
//
static void AprilFools_ChangeMenus(void)
{
	INT32 menu = 1;

	if (!aprilfoolsmode)
		return;

	if (menuactive)
	{
		M_ClearMenus(true);
		D_StartTitle();
	}
	if (demoplayback && titledemo)
		G_CheckDemoStatus();

	if (!cv_tsourdt3rd_aprilfools_ultimatemode.value)
	{
		OP_Tsourdt3rdOptionsMenu[op_isitcalledsingleplayer].status = IT_CVAR|IT_STRING;
		memmove(&gametypedesc, &defaultGametypeTitles, sizeof(gtdesc_t)); // Gametypes

		memmove(&MainMenu, &defaultMenuTitles[menu++], sizeof(menuitem_t)); // Main Menu
		memmove(&SP_MainMenu, &defaultMenuTitles[menu++], sizeof(menuitem_t)); // SP Main Menu
		memmove(&MPauseMenu, &defaultMenuTitles[menu++], sizeof(menuitem_t)); // MP Pause
		memmove(&SPauseMenu, &defaultMenuTitles[menu++], sizeof(menuitem_t)); // SP Pause

		STAR_StoreDefaultMenuStrings();
		return;
	}

	// Gametypes //
	strcpy(gametypedesc[0].notes, "Hang out with your friends!");
	strcpy(gametypedesc[1].notes, "Challenge your friends in this epic coding competition!");
	strcpy(gametypedesc[2].notes, "Mash the thok button until you find the exit sign.");
	strcpy(gametypedesc[3].notes, "Use your thok to locate targets with a ping higher than yours and keep shooting them with rail rings until they ragequit!");
	strcpy(gametypedesc[4].notes, "Join the team with the highest score and shoot in random directions until your team wins!");
	strcpy(gametypedesc[5].notes, "Normally the IT guy is the one being chased, but for some reason it's the opposite in this gametype.");
	strcpy(gametypedesc[6].notes, "Play PropHunt but without the ability to fuse with your environment!");
	strcpy(gametypedesc[7].notes, "Join the team with the most points, steal the payload, find a safe spot to hide until your friends bring back your team's payload to the base, then rush to your base!");

	// Menus //
	// Main Menu
	MainMenu[0].text						= "No Friends Mode";
	MainMenu[1].text						= "The Friend Zone";
	MainMenu[2].text						= "More Stuff";
	MainMenu[3].text						= "Mods";
	MainMenu[4].text						= "Settings";
	MainMenu[5].text						= "EXIT TO DOS";
	MainMenu[6].text 						= "DOOM EASTER EGG THING!";
	// SP Main Menu
	SP_MainMenu[0].text						= "GO!!";
	SP_MainMenu[1].text 					= "sonic runners";
	SP_MainMenu[2].text 					= "good night mode";
	SP_MainMenu[3].text						= "super mario run";
	SP_MainMenu[4].text						= "how do i jump";
	SP_MainMenu[5].text						= "are we there yet";
	// MP Pause
	MPauseMenu[0].text						= "Plugins...";
	MPauseMenu[1].text						= "Scramble Groups...";
	MPauseMenu[2].text						= SPauseMenu[2].text;
	MPauseMenu[3].text						= "Can We Play Tag?";
#ifdef HAVE_DISCORDSUPPORT
	MPauseMenu[4].text						= "Facebook Requests...";
#endif
	MPauseMenu[5].text						= "Keep Going";
	MPauseMenu[6].text						= "Pet 1 Setup";
	MPauseMenu[7].text						= "Pet 2 Setup";
	MPauseMenu[8].text						= "Watching From The Walls";
	MPauseMenu[9].text						= "Enter Playground";
	MPauseMenu[10].text						= "Join Group...";
	MPauseMenu[11].text						= "Customise Pet";
	MPauseMenu[12].text						= MainMenu[4].text;
	MPauseMenu[13].text						= "Leave Group";
	MPauseMenu[14].text						= MainMenu[5].text;
	// SP Pause
	SPauseMenu[0].text 						= "Mods";
	SPauseMenu[1].text						= "Enable Hacks";
	SPauseMenu[2].text						= "where are the emblems help";
	SPauseMenu[3].text						= "What Map??";
	SPauseMenu[4].text						= "Keep Going";
	SPauseMenu[5].text						= "Try Again";
	SPauseMenu[6].text						= "Settings";
	SPauseMenu[7].text						= "Bored Already";
	SPauseMenu[8].text						= MainMenu[5].text;

	OP_Tsourdt3rdOptionsMenu[op_isitcalledsingleplayer].status = IT_GRAYEDOUT;
}

void TSoURD3rd_AprilFools_OnChange(void)
{
	if (!aprilfoolsmode)
		return;

	AprilFools_ChangeMenus();

	if (Playing() || playeringame[consoleplayer])
	{
		if (TSoURDt3rd_AprilFools_ModeEnabled() && cursaveslot > NOSAVESLOT && !netgame)
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
