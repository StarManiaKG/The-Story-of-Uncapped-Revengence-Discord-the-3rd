// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-easter.c
/// \brief Easter event related functions and data

#include "../ss_main.h"
#include "../star_vars.h"
#include "../smkg-st_hud.h"
#include "../menus/smkg-m_sys.h"

#include "../../g_game.h" // playeringame[] //
#include "../../lua_hud.h"
#include "../../st_stuff.h"
#include "../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

INT32 currenteggs = 0;
INT32 collectedmapeggs = 0;
INT32 numMapEggs = 0;
INT32 TOTALEGGS = 0;
INT32 foundeggs = 0;

boolean AllowEasterEggHunt = true;
boolean EnableEasterEggHuntBonuses = false;

static void STAR_Easter_EggHunt_OnChange(void);
static void STAR_Easter_EnableEggHuntBonuses_OnChange(void);

consvar_t cv_tsourdt3rd_easter_egghunt_allowed = CVAR_INIT ("tsourdt3rd_easter_egghunt_allowed", "No", CV_SAVE|CV_CALL|CV_NOINIT, CV_YesNo, STAR_Easter_EggHunt_OnChange);
consvar_t cv_tsourdt3rd_easter_egghunt_bonuses = CVAR_INIT ("tsourdt3rd_easter_egghunt_bonuses", "Off", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, STAR_Easter_EnableEggHuntBonuses_OnChange);

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_Easter_AllEggsCollected(void)
{
	return (EnableEasterEggHuntBonuses && currenteggs == TOTALEGGS);
}

static void STAR_Easter_EggHunt_OnChange(void)
{
	if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))
		return;

	if ((Playing() && playeringame[consoleplayer]) || netgame || ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata))) || currenteggs == TOTALEGGS)
	{
		if ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata)))
			CONS_Printf("Sorry, you'll need to restart your game in order to set this again.\n");
		else if ((Playing() || gamestate == GS_TITLESCREEN || menuactive) && currenteggs == TOTALEGGS)
			CONS_Printf("You already have all of the eggs!\n");
		else if (Playing() && netgame)
			CONS_Printf("Sorry, you can't change this while in a game or netgame.\n");
		CV_StealthSetValue(&cv_tsourdt3rd_easter_egghunt_allowed, !cv_tsourdt3rd_easter_egghunt_allowed.value);
		return;
	}
}

static void STAR_Easter_EnableEggHuntBonuses_OnChange(void)
{
	if ((tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))
		return;

	if ((netgame || currenteggs != TOTALEGGS) || ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata))))
	{
		(((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata))) ?
			(CONS_Printf("Sorry, you'll need to restart your game in order to set this again.\n")) :

			(((Playing() || gamestate == GS_TITLESCREEN || menuactive) && currenteggs != TOTALEGGS) ?
				(CONS_Printf("You can't set this yet!\nYou didn't get all those easter eggs!\n")) :

			((Playing() && netgame) ?
				(CONS_Printf("Sorry, you can't change this while in a netgame.\n")) : 0)));

		CV_StealthSetValue(&cv_tsourdt3rd_easter_egghunt_bonuses, !cv_tsourdt3rd_easter_egghunt_bonuses.value);
	}
	else
		EnableEasterEggHuntBonuses = cv_tsourdt3rd_easter_egghunt_bonuses.value;
}

//
// void TSoURDt3rd_Easter_ST_drawEggs(void);
// Draws TSoURDt3rd's cool Easter HUD.
//
// STAR NOTE FOR SNOOPERS: THIS CONTAINS PIECES OF CODE THAT tsourdt3rd.pk3 INITIALIZES, SETS, AND RUNS.
//							IF YOU WANT TO SEE THE REST, JUST TAKE A LOOK AT THAT PK3.
//
void TSoURDt3rd_Easter_ST_drawEggs(void)
{
	// Run Some Checks
	if (!Playing() 										// We Need to Play, Jesse
		|| (netgame || multiplayer)						// You Can't Manipulate Your Friends for This Egg Hunt
		|| (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))								// We Shouldn't Even Show This If It's Not Easter
		|| ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata)))		// No Cheating
		|| (!AllowEasterEggHunt)						// Hooray for Consent
		
		|| (F_GetPromptHideHud(hudinfo[HUD_RINGS].y))	// If Rings are Hidden, So Are the Eggs
		|| (!LUA_HudEnabled(hud_rings)))					// Same as above

		return;

	//// NOW WE RENDER, AND WE'RE DONE! :) ////
	if (numMapEggs && (collectedmapeggs != numMapEggs)) // Draw the Patches and Strings //
	{	
		// Map Eggs
		V_DrawScaledPatch(16, 64, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), tsourdt3rd_easter_leveleggs);

		V_DrawTallNum(115, 64, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), collectedmapeggs);
		V_DrawString(115, 64, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), "/");
		V_DrawTallNum(140, 64, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), numMapEggs);
		
		// Total Eggs
		V_DrawScaledPatch(16, 80, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), tsourdt3rd_easter_totaleggs);

		V_DrawTallNum(115, 80, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), currenteggs);
		V_DrawString(115, 80, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), "/");
		V_DrawTallNum(140, 80, V_SNAPTOLEFT|V_SNAPTOTOP|V_PERPLAYER|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS), TOTALEGGS);
	}
	else // Draw the Egg Notifier //
	{
		V_DrawCenteredThinString(88, 64,
			V_SNAPTOLEFT|((((currenteggs == TOTALEGGS) || (numMapEggs && (collectedmapeggs == numMapEggs))) ? (V_GREENMAP) : (V_REDMAP))|((stplyr->spectator) ? V_HUDTRANSHALF : V_HUDTRANS)),
			((currenteggs == TOTALEGGS) ? ("All Eggs Have Been Found!") : ((numMapEggs && (collectedmapeggs == numMapEggs)) ? ("All Eggs in this Map Have Been Found!") : ("There Are No Eggs in This Map!"))));
	}
}
