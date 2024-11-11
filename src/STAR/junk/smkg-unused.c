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

// ===================
// NUMBERS AND STRINGS
// ===================

//
// char *STAR_ConvertNumberToString(INT32 NUMBER, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionString)
// Converts Strings to Compressed Numbers
//
// Example of a Possible Return:
//	NUMBER == 280, turnIntoVersionString = true		=	Returned String = '2.8.0'
//	NUMBER == 271, turnIntoVersionString = false	=	Returned String = '271'
//
char finalNumberString[256] = "";

char *STAR_ConvertNumberToString(INT32 NUMBER, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionString)
{
	// Make Variables //
	INT32 i = startIFrom, j = startJFrom;
	char convertedNumberString[256] = ""; sprintf(convertedNumberString, "%d", NUMBER);

	// Initialize the Main String, and Iterate Through Our Two Strings //
	if (turnIntoVersionString)
	{
		while (convertedNumberString[j] != '\0')
		{
			finalNumberString[i] = convertedNumberString[j];
			i++; j++;

			if (convertedNumberString[j] != '\0') // Prevents an Extra Dot From Being Added at the End
			{
				finalNumberString[i] = '.';
				i++;
			}
		}
	}
	else
		strcpy(finalNumberString, convertedNumberString);

	// Return Our Converted String and We're Done! //
	return finalNumberString;
}

//
// INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean turnIntoVersionNumber)
// Converts Numbers to Strings, and Then Converts Them Back to Numbers
//
INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean turnIntoVersionNumber)
{
	// Return The Number, and We're Done :) //
	char numberString[256] = ""; strcpy(numberString, STAR_ConvertNumberToString(NUMBER, startI1From, startJ1From, turnIntoVersionString));
	return STAR_ConvertStringToCompressedNumber(numberString, startI2From, startJ2From, turnIntoVersionNumber);
}

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

// =====
// MENUS
// =====

menu_t MP_ExtendedServerPropertyDef =
{
	MTREE3(MN_MP_MAIN, MN_MP_CONNECT, MN_MP_EXTENDEDSERVERPROPERTIES),
	"M_MULTI",
	sizeof (MP_ExtendedServerPropertyMenu)/sizeof (menuitem_t),
	&MP_ConnectDef,
	MP_ExtendedServerPropertyMenu,
	STAR_DrawExtendedServerPropertyMenu,
	30, 30,
	0,
	NULL
};

static menuitem_t MP_ExtendedServerPropertyMenu[] = {
	{IT_KEYHANDLER | IT_NOTHING, NULL, "", STAR_HandleExtendedServerPropertyMenu, 0}
};

// Servers //
static void STAR_InitializeExtendedServerPropertyMenu(INT32 choice)
{
	STAR_ServerToExtend = choice-FIRSTSERVERLINE + serverlistpage * SERVERS_PER_PAGE;	

	M_Connect(choice);
	//M_SetupNextMenu(&MP_ExtendedServerPropertyDef);
}

static void STAR_DrawExtendedServerPropertyMenu(void)
{
	// Make Variables //
	char *gt;
	UINT32 globalflags = (serverlist[STAR_ServerToExtend].info.refusereason ? V_TRANSLUCENT : 0)|V_ALLOWLOWERCASE;

	// Draw Everything //
	M_DrawGenericMenu();
	V_DrawString(currentMenu->x, 0, globalflags, serverlist[STAR_ServerToExtend].info.servername);

	// Don't use color flags intentionally, the global yellow color will auto override the text color code
	if (serverlist[STAR_ServerToExtend].info.modifiedgame)
		V_DrawSmallString(currentMenu->x+202, 8, globalflags, "\x85" "Mod");
	if (serverlist[STAR_ServerToExtend].info.cheatsenabled)
		V_DrawSmallString(currentMenu->x+222, 8, globalflags, "\x83" "Cheats");

	V_DrawSmallString(currentMenu->x, 8, globalflags,
		                   va("Ping: %u", (UINT32)LONG(serverlist[STAR_ServerToExtend].info.time)));

	gt = serverlist[STAR_ServerToExtend].info.gametypename;

	V_DrawSmallString(currentMenu->x+46, 24, globalflags,
	                         va("Players: %02d/%02d", serverlist[STAR_ServerToExtend].info.numberofplayer, serverlist[STAR_ServerToExtend].info.maxplayer));

	if (strlen(gt) > 11)
		gt = va("Gametype: %.11s...", gt);
	else
		gt = va("Gametype: %s", gt);

	V_DrawSmallString(currentMenu->x+112, 32, globalflags, gt);
}

static void STAR_HandleExtendedServerPropertyMenu(INT32 choice)
{
	boolean exitmenu = false;

	switch (choice)
	{
		case KEY_ENTER:
			M_Connect(STAR_ServerToExtend);
			break;

		case KEY_ESCAPE:
			exitmenu = true;
			break;

		default:
			break;
	}
	
	if (exitmenu)
	{
		if (currentMenu->prevMenu == &MP_ConnectDef)
			M_SetupNextMenu(currentMenu->prevMenu);
		else
			M_ClearMenus(false);	
	}
}
