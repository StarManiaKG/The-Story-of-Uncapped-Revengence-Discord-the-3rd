// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
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

// =============
// SCREEN BORDER
// =============

UINT8 *scr_borderpatch; // flat used to fill the reduced view borders set at ST_Init()

/**	\brief viewborder patches lump numbers
*/
lumpnum_t viewborderlump[8];

/**	\brief Store the lumpnumber of the viewborder patches
*/

void R_InitViewBorder(void)
{
	viewborderlump[BRDR_T] = W_GetNumForName("brdr_t");
	viewborderlump[BRDR_B] = W_GetNumForName("brdr_b");
	viewborderlump[BRDR_L] = W_GetNumForName("brdr_l");
	viewborderlump[BRDR_R] = W_GetNumForName("brdr_r");
	viewborderlump[BRDR_TL] = W_GetNumForName("brdr_tl");
	viewborderlump[BRDR_BL] = W_GetNumForName("brdr_bl");
	viewborderlump[BRDR_TR] = W_GetNumForName("brdr_tr");
	viewborderlump[BRDR_BR] = W_GetNumForName("brdr_br");
}

#if 0
/**	\brief R_FillBackScreen

	Fills the back screen with a pattern for variable screen sizes
	Also draws a beveled edge.
*/
void R_FillBackScreen(void)
{
	UINT8 *src, *dest;
	patch_t *patch;
	INT32 x, y, step, boff;

	// quickfix, don't cache lumps in both modes
	if (rendermode != render_soft)
		return;

	// draw pattern around the status bar too (when hires),
	// so return only when in full-screen without status bar.
	if (scaledviewwidth == vid.width && viewheight == vid.height)
		return;

	src = scr_borderpatch;
	dest = screens[1];

	for (y = 0; y < vid.height; y++)
	{
		for (x = 0; x < vid.width/128; x++)
		{
			M_Memcpy (dest, src+((y&127)<<7), 128);
			dest += 128;
		}

		if (vid.width&127)
		{
			M_Memcpy(dest, src+((y&127)<<7), vid.width&127);
			dest += (vid.width&127);
		}
	}

	// don't draw the borders when viewwidth is full vid.width.
	if (scaledviewwidth == vid.width)
		return;

	step = 8;
	boff = 8;

	patch = W_CacheLumpNum(viewborderlump[BRDR_T], PU_CACHE);
	for (x = 0; x < scaledviewwidth; x += step)
		V_DrawPatch(viewwindowx + x, viewwindowy - boff, 1, patch);

	patch = W_CacheLumpNum(viewborderlump[BRDR_B], PU_CACHE);
	for (x = 0; x < scaledviewwidth; x += step)
		V_DrawPatch(viewwindowx + x, viewwindowy + viewheight, 1, patch);

	patch = W_CacheLumpNum(viewborderlump[BRDR_L], PU_CACHE);
	for (y = 0; y < viewheight; y += step)
		V_DrawPatch(viewwindowx - boff, viewwindowy + y, 1, patch);

	patch = W_CacheLumpNum(viewborderlump[BRDR_R],PU_CACHE);
	for (y = 0; y < viewheight; y += step)
		V_DrawPatch(viewwindowx + scaledviewwidth, viewwindowy + y, 1,
			patch);

	// Draw beveled corners.
	V_DrawPatch(viewwindowx - boff, viewwindowy - boff, 1,
		W_CacheLumpNum(viewborderlump[BRDR_TL], PU_CACHE));
	V_DrawPatch(viewwindowx + scaledviewwidth, viewwindowy - boff, 1,
		W_CacheLumpNum(viewborderlump[BRDR_TR], PU_CACHE));
	V_DrawPatch(viewwindowx - boff, viewwindowy + viewheight, 1,
		W_CacheLumpNum(viewborderlump[BRDR_BL], PU_CACHE));
	V_DrawPatch(viewwindowx + scaledviewwidth, viewwindowy + viewheight, 1,
		W_CacheLumpNum(viewborderlump[BRDR_BR], PU_CACHE));
}
#endif

// ==========================================================================
//                                                             R_DRAW.C STUFF
// ==========================================================================

// ------------------
// HWR_DrawViewBorder
// Fill the space around the view window with a Doom flat texture, draw the
// beveled edges.
// 'clearlines' is useful to clear the heads up messages, when the view
// window is reduced, it doesn't refresh all the view borders.
// ------------------
void HWR_DrawViewBorder(INT32 clearlines)
{
	INT32 x, y;
	INT32 top, side;
	INT32 baseviewwidth, baseviewheight;
	INT32 basewindowx, basewindowy;
	patch_t *patch;

	if (!clearlines)
		clearlines = BASEVIDHEIGHT; // refresh all

	// calc view size based on original game resolution
	baseviewwidth =  FixedInt(FixedDiv(FLOAT_TO_FIXED(viewwidth), vid.fdup)); //(cv_viewsize.value * BASEVIDWIDTH/10)&~7;
	baseviewheight = FixedInt(FixedDiv(FLOAT_TO_FIXED(viewheight), vid.fdup));
	top = FixedInt(FixedDiv(FLOAT_TO_FIXED(viewwindowy), vid.fdup));
	side = FixedInt(FixedDiv(FLOAT_TO_FIXED(viewwindowx), vid.fdup));

	// top
	HWR_DrawFlatFill(0, 0,
		BASEVIDWIDTH, (top < clearlines ? top : clearlines),
		st_borderpatchnum);

	// left
	if (top < clearlines)
		HWR_DrawFlatFill(0, top, side,
			(clearlines-top < baseviewheight ? clearlines-top : baseviewheight),
			st_borderpatchnum);

	// right
	if (top < clearlines)
		HWR_DrawFlatFill(side + baseviewwidth, top, side,
			(clearlines-top < baseviewheight ? clearlines-top : baseviewheight),
			st_borderpatchnum);

	// bottom
	if (top + baseviewheight < clearlines)
		HWR_DrawFlatFill(0, top + baseviewheight,
			BASEVIDWIDTH, BASEVIDHEIGHT, st_borderpatchnum);

	//
	// draw the view borders
	//

	basewindowx = (BASEVIDWIDTH - baseviewwidth)>>1;
	if (baseviewwidth == BASEVIDWIDTH)
		basewindowy = 0;
	else
		basewindowy = top;

	// top edge
	if (clearlines > basewindowy - 8)
	{
		patch = W_CachePatchNum(viewborderlump[BRDR_T], PU_PATCH);
		for (x = 0; x < baseviewwidth; x += 8)
			HWR_DrawStretchyFixedPatch(patch, ((basewindowx + x)<<FRACBITS), ((basewindowy - 8)<<FRACBITS),
				FRACUNIT, FRACUNIT,
				0, NULL);
	}

	// bottom edge
	if (clearlines > basewindowy + baseviewheight)
	{
		patch = W_CachePatchNum(viewborderlump[BRDR_B], PU_PATCH);
		for (x = 0; x < baseviewwidth; x += 8)
			HWR_DrawStretchyFixedPatch(patch, ((basewindowx + x)<<FRACBITS), ((basewindowy + baseviewheight)<<FRACBITS),
				FRACUNIT, FRACUNIT,
				0, NULL);
	}

	// left edge
	if (clearlines > basewindowy)
	{
		patch = W_CachePatchNum(viewborderlump[BRDR_L], PU_PATCH);
		for (y = 0; y < baseviewheight && basewindowy + y < clearlines;
			y += 8)
		{
			HWR_DrawStretchyFixedPatch(patch, ((basewindowx - 8)<<FRACBITS), ((basewindowy + y)<<FRACBITS),
				FRACUNIT, FRACUNIT,
				0, NULL);
		}
	}

	// right edge
	if (clearlines > basewindowy)
	{
		patch = W_CachePatchNum(viewborderlump[BRDR_R], PU_PATCH);
		for (y = 0; y < baseviewheight && basewindowy+y < clearlines;
			y += 8)
		{
			HWR_DrawStretchyFixedPatch(patch, ((basewindowx + baseviewwidth)<<FRACBITS), ((basewindowy + y)<<FRACBITS),
				FRACUNIT, FRACUNIT,
				0, NULL);
		}
	}

	// Draw beveled corners.
	if (clearlines > basewindowy - 8)
		HWR_DrawStretchyFixedPatch(W_CachePatchNum(viewborderlump[BRDR_TL],
				PU_PATCH),
			((basewindowx - 8)<<FRACBITS), ((basewindowy - 8)<<FRACBITS), FRACUNIT, FRACUNIT, 0, NULL);

	if (clearlines > basewindowy - 8)
		HWR_DrawStretchyFixedPatch(W_CachePatchNum(viewborderlump[BRDR_TR],
				PU_PATCH),
			((basewindowx + baseviewwidth)<<FRACBITS), ((basewindowy - 8)<<FRACBITS), FRACUNIT, FRACUNIT, 0, NULL);

	if (clearlines > basewindowy+baseviewheight)
		HWR_DrawStretchyFixedPatch(W_CachePatchNum(viewborderlump[BRDR_BL],
				PU_PATCH),
			((basewindowx - 8)<<FRACBITS), ((basewindowy + baseviewheight)<<FRACBITS), FRACUNIT, FRACUNIT, 0, NULL);

	if (clearlines > basewindowy + baseviewheight)
		HWR_DrawStretchyFixedPatch(W_CachePatchNum(viewborderlump[BRDR_BR],
				PU_PATCH),
			((basewindowx + baseviewwidth)<<FRACBITS),
			((basewindowy + baseviewheight)<<FRACBITS), FRACUNIT, FRACUNIT, 0, NULL);
}

void R_Init(void)
{
	//I_OutputMsg("\nR_InitViewBorder");
	R_InitViewBorder();
}

// SRB2 border patch
	// st_borderpatchnum = W_GetNumForName("GFZFLR01");
	// scr_borderpatch = W_CacheLumpNum(st_borderpatchnum, PU_HUDGFX);

/// \brief Top border
#define BRDR_T 0
/// \brief Bottom border
#define BRDR_B 1
/// \brief Left border
#define BRDR_L 2
/// \brief Right border
#define BRDR_R 3
/// \brief Topleft border
#define BRDR_TL 4
/// \brief Topright border
#define BRDR_TR 5
/// \brief Bottomleft border
#define BRDR_BL 6
/// \brief Bottomright border
#define BRDR_BR 7

extern lumpnum_t viewborderlump[8];

#if 0
/**	\brief The R_DrawViewBorder

  Draws the border around the view
	for different size windows?
*/
void R_DrawViewBorder(void)
{
	INT32 top, side, ofs;

	if (rendermode == render_none)
		return;
#ifdef HWRENDER
	if (rendermode != render_soft)
	{
		HWR_DrawViewBorder(0);
		return;
	}
	else
#endif

#ifdef DEBUG
	fprintf(stderr,"RDVB: vidwidth %d vidheight %d scaledviewwidth %d viewheight %d\n",
		vid.width, vid.height, scaledviewwidth, viewheight);
#endif

	if (scaledviewwidth == vid.width)
		return;

	top = (vid.height - viewheight)>>1;
	side = (vid.width - scaledviewwidth)>>1;

	// copy top and one line of left side
	R_VideoErase(0, top*vid.width+side);

	// copy one line of right side and bottom
	ofs = (viewheight+top)*vid.width - side;
	R_VideoErase(ofs, top*vid.width + side);

	// copy sides using wraparound
	ofs = top*vid.width + vid.width-side;
	side <<= 1;

    // simpler using our VID_Blit routine
	VID_BlitLinearScreen(screens[1] + ofs, screens[0] + ofs, side, viewheight - 1,
		vid.width, vid.width);
}
#endif

void ST_LoadGraphics(void)
{
	int i;

	// SRB2 border patch
	// st_borderpatchnum = W_GetNumForName("GFZFLR01");
	// scr_borderpatch = W_CacheLumpNum(st_borderpatchnum, PU_HUDGFX);
}

// ========
// HARDWARE
// ========

static const INT32 picmode2GR[] =
{
	GL_TEXFMT_P_8,                // PALETTE
	0,                            // INTENSITY          (unsupported yet)
	GL_TEXFMT_ALPHA_INTENSITY_88, // INTENSITY_ALPHA    (corona use this)
	0,                            // RGB24              (unsupported yet)
	GL_TEXFMT_RGBA,               // RGBA32             (opengl only)
};

// =====
// SKINS
// =====

skins = Z_Realloc(skins, sizeof(skin_t*) * (numskins + 1), PU_STATIC, NULL);
skin = skins[numskins] = Z_Calloc(sizeof(skin_t), PU_STATIC, NULL);
