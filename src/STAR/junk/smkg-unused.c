// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-unused.c
/// \brief Data portraying to, at least currently, TSoURDt3rd's unused Stuff

#include "../../doomdef.h"
#include "../../w_wad.h"

// ------------------------ //
//        Functions
// ------------------------ //

// =======
// NUMBERS
// =======

//
// INT32 **STAR_SplitNumber(INT32 NUMBER, boolean useINT16Format)
// Splits numbers Into smaller numbers, with size depending on 'useINT16Format'.
//
// Example of a Possible Return:
//	NUMBER == 280, useINT16Format = false		=	Returned Number = 28, 0
//	NUMBER == 20000000, useINT16Format = true	=	Returned Number = 20, 00, 00, 00
//

INT32 **finalSplitNumbers;

INT32 **STAR_SplitNumber(INT32 NUMBER, boolean useINT16Format)
{
	// Make Variables //
	INT32 i = 0, j = 0, c = 0;
	INT32 formatToUse = (useINT16Format ? 4 : 1);
	char numberString[256] = ""; strcpy(numberString, STAR_ConvertNumberToString(NUMBER, 0, 0, false));
	char convertedNumberString[256] = "";

	// Erase The Table if Something's There //
	while (finalSplitNumbers[c][i] != '\0')
	{
		if ((i >= formatToUse) || (finalSplitNumbers[c][i+1] == '\0'))
		{
			c++;
			i = 0;
		}

		finalSplitNumbers[c][i] = '\0';
		i++;
	}
	c = 0; i = 0;

	// Apply Our Strings to Our Numbers //
	while (numberString[j] != '\0')
	{
		convertedNumberString[j] = numberString[j];
		finalSplitNumbers[c][i] = atoi(&convertedNumberString[j]);

		if ((i >= formatToUse) || (numberString[j+1] == '\0'))
		{
			if (numberString[j+1] == '\0')
			{
				finalSplitNumbers[c][i+1] = '\0';
				break;
			}

			i = 0;
			c++; j++;
		}
		else
		{
			i++;
			j++;
		}
	}

	// Return Our Split Numbers (Which are Stored in a Table), and We're Done! //
	return finalSplitNumbers;
}

// ====
// WADS
// ====

//
// lumpnum_t W_GetNumForMusicName(const char *name)
//
// Calls W_CheckNumForName, but does NOT bomb out if not found.
// Geared towards checking for music files where the lump not being found is not a call for a crash.
//
// Ported From SRB2 Persona lol
//
lumpnum_t W_GetNumForMusicName(const char *name)
{
	lumpnum_t i;

	i = W_CheckNumForName(name);

	return i;
}

// =======
// CONSOLE
// =======

static const char *CON_LoadingStrings[LOADED_ALLDONE+1] =
{
	"Init zone memory...", //LOADED_ZINIT
	"Init game timing...", //LOADED_ISTARTUPTIMER
	"Loading main assets...", //LOADED_IWAD
	"Loading add-ons...", //LOADED_PWAD
	"Init graphics subsystem...", //LOADED_ISTARTUPGRAPHICS
	"Cache fonts...", //LOADED_HUINIT
	"Load settings...", //LOADED_CONFIG
	"Cache textures...", //LOADED_INITTEXTUREDATA
	"Cache sprites...", //LOADED_INITSPIRTES
	"Load characters...", //LOADED_INITSKINS
	"Init rendering daemon...", //LOADED_RINIT
	"Init audio subsystem...", //LOADED_SINITSFXCHANNELS
	"Cache HUD...", //LOADED_STINIT
	"Init ACSVM...", //LOADED_ACSINIT
	"Check game status...", //LOADED_DCHECKNETGAME
	"Now starting..."
}; // see also con_loadprogress_t in console.h

//
// Error handling for the loading bar, to ensure it doesn't skip any steps.
//
void CON_SetLoadingProgress(con_loadprogress_t newStep)
{
	const con_loadprogress_t expectedStep = con_startup_loadprogress + 1;

	if (newStep != expectedStep)
	{
		I_Error("Something is wrong with the loading bar! (got %d, expected %d)\n", newStep, expectedStep);
		return;
	}

	con_startup_loadprogress = newStep;

	if (con_startup_loadprogress <= LOADED_ALLDONE)
		CONS_Printf("LOADING UPDATE - %s\n", CON_LoadingStrings[con_startup_loadprogress]);

	if (con_startup_loadprogress < LOADED_ISTARTUPGRAPHICS) // rendering not possible?
		return;
	CON_DrawLoadBar(); // here we display the console text
	I_OsPolling();
	I_UpdateNoBlit();
	I_FinishUpdate(); // page flip or blit buffer
}

//
// Draws a simple white fill at the bottom of startup for load progress
//
void CON_DrawLoadBar(void)
{
	const INT16 barheight = 2;
	INT16 barwidth = 0;

	Lock_state();

	if (!con_started || !graphics_started)
	{
		Unlock_state();
		return;
	}

	CON_DrawBackpic();

	barwidth = (BASEVIDWIDTH * con_startup_loadprogress) / LOADED_ALLDONE;
	V_DrawFill(0, BASEVIDHEIGHT - barheight, barwidth, barheight, 0);

	if (con_startup_loadprogress <= LOADED_ALLDONE)
	{
		V_DrawString(4, BASEVIDHEIGHT - (barheight + 8 + 4), V_FORCEUPPERCASE, CON_LoadingStrings[con_startup_loadprogress]);
	}

	Unlock_state();
}

// =====
// MISC.
// =====

// Easy Texture Finder
if (textures[i]->hash == hash && !strncasecmp(textures[i]->name, name, 8))
