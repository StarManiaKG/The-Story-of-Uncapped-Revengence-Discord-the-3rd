// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_easter.c
/// \brief Easter event, related functions and data

#include "../ss_main.h"
#include "../star_vars.h"
#include "../../m_menu.h"
#include "../../d_main.h" // autoloaded //
#include "../../g_game.h" // playeringame[] //

// ------------------------ //
//        Variables
// ------------------------ //

static void STAR_Easter_EggHunt_OnChange(void);
static void STAR_Easter_EnableEggHuntBonuses_OnChange(void);

// ------------------------ //
//        Commands
// ------------------------ //

consvar_t cv_easter_allowegghunt = CVAR_INIT ("easter_allowegghunt", "No", CV_SAVE|CV_CALL|CV_NOINIT, CV_YesNo, STAR_Easter_EggHunt_OnChange);
consvar_t cv_easter_egghuntbonuses = CVAR_INIT ("easter_egghuntbonuses", "Off", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, STAR_Easter_EnableEggHuntBonuses_OnChange);

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_Easter_AllEggsCollected(void)
{
	return (EnableEasterEggHuntBonuses && currenteggs == TOTALEGGS);
}

static void STAR_Easter_EggHunt_OnChange(void)
{
	if (!eastermode)
		return;
	M_UpdateEasterStuff();

	if ((Playing() && playeringame[consoleplayer]) || netgame || (TSoURDt3rd_NoMoreExtras || autoloaded) || currenteggs == TOTALEGGS)
	{
		if (TSoURDt3rd_NoMoreExtras || autoloaded)
			CONS_Printf("Sorry, you'll need to restart your game in order to set this again.\n");
		else if ((Playing() || gamestate == GS_TITLESCREEN || menuactive) && currenteggs == TOTALEGGS)
			CONS_Printf("You already have all of the eggs!\n");
		else if (Playing() && netgame)
			CONS_Printf("Sorry, you can't change this while in a game or netgame.\n");

		CV_StealthSetValue(&cv_easter_allowegghunt, !cv_easter_allowegghunt.value);
		return;
	}
}

static void STAR_Easter_EnableEggHuntBonuses_OnChange(void)
{
	if (eastermode)
		return;
	M_UpdateEasterStuff();

	if ((netgame || currenteggs != TOTALEGGS) || (TSoURDt3rd_NoMoreExtras || autoloaded))
	{
		((TSoURDt3rd_NoMoreExtras || autoloaded) ?
			(CONS_Printf("Sorry, you'll need to restart your game in order to set this again.\n")) :

			(((Playing() || gamestate == GS_TITLESCREEN || menuactive) && currenteggs != TOTALEGGS) ?
				(CONS_Printf("You can't set this yet!\nYou didn't get all those easter eggs!\n")) :

			((Playing() && netgame) ?
				(CONS_Printf("Sorry, you can't change this while in a netgame.\n")) : 0)));

		CV_StealthSetValue(&cv_easter_egghuntbonuses, !cv_easter_egghuntbonuses.value);
	}
	else
		EnableEasterEggHuntBonuses = cv_easter_egghuntbonuses.value;
}
