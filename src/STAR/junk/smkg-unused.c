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
/// \brief NOT MEANT TO BE COMPILED!

#error "Don't compile this file."

#include "../../doomdef.h"

#include "../../w_wad.h"
#include "../../r_main.h"
#include "../../st_stuff.h" // st_borderpatchnum
#include "../../v_video.h"
#include "../../m_menu.h"
#include "../../z_zone.h"

#include "../star_vars.h"

// ------------------------ //
//        Functions
// ------------------------ //

// ======================
// LIGHTING
// ======================

void HWR_Init_Light(const char *light_patch)
{
	CONS_Printf("HWR_Init_Light()...\n");

	// Ensure the patch exists
	//corona_lumpnum = W_CheckNumForPatchName(light_patch); // DEFAULT: OFF (crashes game)
	//corona_lumpnum = W_CheckNumForName(light_patch);
	corona_lumpnum = W_GetNumForName(light_patch);
	if (corona_lumpnum == LUMPERROR)
	{
		CONS_Alert(CONS_WARNING, "HWR_Init_Light() - Specified corona lump '%s' not found!\n", light_patch);
		return;
	}

	// Load the corona patch, change its zone tag, and ensure it's valid

//#define OH_MY_GOODNESS
//#define TEST_PATCH

#ifndef TEST_PATCH
	corona_patch = HWR_GetCachedGLPatch(corona_lumpnum);
	//corona_patch = (patch_t *)W_CachePatchNum(W_GetNumForName(light_patch), PU_CACHE);
	//corona_patch = (patch_t *)W_CachePatchName(light_patch, PU_CACHE); // PU_CACHE // PU_SPRITE // PU_PATCH

#if 0
	//CONS_Printf("isdoompatch is %d\n", Picture_CheckIfDoomPatch(corona_patch));

#if 1
	#if 0
		#define WIDTH SHORT(corona_patch->width)
		#define HEIGHT SHORT(corona_patch->height)
	#else
		#define WIDTH corona_patch->width
		#define HEIGHT corona_patch->height
	#endif
	#define TOPOFFSET corona_patch->topoffset
	#define LEFTOFFSET corona_patch->leftoffset
#else
	#define WIDTH 0
	#define HEIGHT 0
	#define TOPOFFSET 0
	#define LEFTOFFSET 0
#endif
	INT32 inflags = PICFMT_FLAT32; // PICFMT_DOOMPATCH // PICFMT_FLAT32
	INT32 outflags = PICFMT_PATCH; // PICFMT_PATCH // PICFMT_FLAT32 // PICFMT_DOOMPATCH
	INT32 flags = PICFLAGS_USE_TRANSPARENTPIXEL; // PICFLAGS_USE_TRANSPARENTPIXEL

	void *realpatch = Picture_PatchConvert(inflags, 0, corona_patch, outflags, NULL, WIDTH, HEIGHT, TOPOFFSET, LEFTOFFSET, flags);
	if (realpatch)
	{
		CONS_Printf("patch exists\n");
#if 1
		Z_SetUser(realpatch, (void **)&corona_patch);
#else
		corona_patch = realpatch;
#endif
		//Z_ChangeTag(realpatch, PU_HWRCACHE_UNLOCKED);
		//Z_Free(realpatch);
	}
	else
		CONS_Printf("patch no exists\n");
#endif

#else
	//patch_t *test_patch = HWR_GetCachedGLPatch(corona_lumpnum);
	patch_t *test_patch = (patch_t *)W_CachePatchName(CORONA_PATCH, PU_CACHE); // PU_CACHE // PU_SPRITE // PU_PATCH
#endif
#ifdef OH_MY_GOODNESS
	UINT32 lumpnum = (UINT32)corona_lumpnum;
	lumpcache_t *lumpcache = wadfiles[WADFILENUM(lumpnum)]->patchcache[LUMPNUM(lumpnum)];
#endif
	if (
#ifdef OH_MY_GOODNESS
		!lumpcache ||
#endif
#ifndef TEST_PATCH
		!corona_patch ||
		!corona_patch->hardware
#else
		!test_patch ||
		!test_patch->hardware
#endif
	)
	{
		CONS_Alert(CONS_ERROR, "HWR_Init_Light() - Failed to load corona patch '%s'!\n", light_patch);
#ifndef TEST_PATCH
		Z_Free(corona_patch);
		corona_patch = NULL;
#else
		Z_Free(test_patch);
		test_patch = NULL;
#endif
		return;
	}

	const size_t tag = PU_CACHE; // PU_CACHE // PU_SPRITE // PU_PATCH // PU_HWRCACHE // PU_STATIC

#ifdef OH_MY_GOODNESS
	Z_ChangeTag(lumpcache[lumpnum], tag);
	Z_SetUser(ptr, &lumpcache[lump]);
#endif

#ifdef TEST_PATCH
	size_t len = W_LumpLengthPwad(WADFILENUM(lumpnum), LUMPNUM(lumpnum));
	void *test_void = Z_Malloc(len, PU_STATIC, NULL);

	Z_ChangeTag(test_patch, tag);
	//Z_SetUser((void *)test_patch, &test_void);
	//Z_SetUser((void *)test_patch, (void **)&corona_patch);
	//Z_Free(test_patch);
	//Z_ChangeTag(test_patch, tag);

	//Z_ChangeTag(test_void, tag);
	//corona_patch = (patch_t *)test_void;

	corona_patch = (patch_t *)test_patch;
	//Z_SetUser(test_void, (void **)&corona_patch);
	//Z_ChangeTag(corona_patch, tag);
	//Z_ChangeTag(corona_gl_patch, tag);

	//Z_ChangeTag(test_void, tag);

	if (test_void) { Z_Free(test_void); test_void = NULL; }
	//if (test_patch) { Z_Free(test_patch); test_patch = NULL; }

	if (!corona_patch || !corona_patch->hardware)
	{
		CONS_Alert(CONS_ERROR, "HWR_Init_Light() - Failed to load corona patch '%s'!\n", light_patch);
		Z_Free(corona_patch);
		corona_patch = NULL;
		corona_lumpnum = LUMPERROR;
		return;
	}
#else
#if 1
	Z_ChangeTag(corona_patch, tag);
	Z_ChangeTag(corona_patch->hardware, tag);
	//corona_gl_patch->mipmap = Z_Calloc(sizeof(GLMipmap_t), PU_HWRPATCHINFO, NULL);

	//Z_ChangeTag(ptr, tag);
	//Z_SetUser(ptr, &lumpcache[lump]);
	//Z_Free(lumpdata);
#endif
#endif

	//HWR_GetPatch(corona_patch);
	//corona_gl_patch = (GLPatch_t *)corona_patch->hardware;
	corona_gl_patch = (GLPatch_t *)Patch_AllocateHardwarePatch(corona_patch);

#if 0
	Z_Free(corona_gl_patch->mipmap);
	//corona_gl_patch->mipmap = Z_Calloc(sizeof(GLMipmap_t), PU_HWRPATCHINFO, NULL);
	corona_gl_patch->mipmap = Z_Calloc(sizeof(GLMipmap_t), PU_HWRPATCHINFO, &corona_gl_patch->mipmap);
#endif
	CONS_Printf("HWR_Init_Light() - 1.5\n");

	// Ensure mipmap exists and is valid
	if (!corona_gl_patch->mipmap)
	{
		CONS_Alert(CONS_ERROR, "HWR_Init_Light() - Mipmap not initialized for corona patch '%s'!\n", light_patch);
		Z_Free(corona_gl_patch); Z_Free(corona_patch);
		corona_gl_patch = NULL; corona_patch = NULL;
		corona_lumpnum = LUMPERROR;
		return;
	}

	corona_gl_patch->mipmap->downloaded = 0;

#if 0
	const INT16 texSize = 128;
	corona_patch->width = corona_patch->height = (INT16)texSize;
	corona_gl_patch->mipmap->width = corona_gl_patch->mipmap->height = (UINT16)texSize;
#endif

#if 0
	if (corona_gl_patch->mipmap->data)
	{
		Z_Free(corona_gl_patch->mipmap->data);
		corona_gl_patch->mipmap->data = NULL;
	}
#endif

#if 0
	CONS_Printf("width is %d, downloaded is %d\n", corona_gl_patch->mipmap->width, corona_gl_patch->mipmap->downloaded);
#if 0
	HWR_MakePatch(corona_patch, corona_gl_patch, corona_gl_patch->mipmap, true);
	Z_ChangeTag(corona_patch, tag);
	Z_ChangeTag(corona_patch->hardware, tag);
	Z_ChangeTag(corona_gl_patch->mipmap, tag); // PU_HWRPATCHINFO // tag
#endif
#endif

	//HWR_SetLight();

	// Precompute squared radius for all dynamic lights
#if 1
	for (size_t i = 0; i < NUMLIGHTS; i++)
	{
		lspr[i].dynamic_sqrradius = (lspr[i].dynamic_radius * lspr[i].dynamic_radius);
	}
#else
	int i;
	for (i = 0; i < NUMLIGHTS; i++)
	{
		light_t *p_lspr = &lspr[i];
		p_lspr->dynamic_sqrradius = (p_lspr->dynamic_radius * p_lspr->dynamic_radius);
	}
#endif
	CONS_Printf("HWR_Init_Light()...\n");
}

// ===================
// NUMBERS AND STRINGS
// ===================

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
// INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionNumber)
// Converts Strings to Compressed Numbers
//
// Example of a Possible Return:
//	STRING == '2.8', turnIntoVersionNumber = true		=	Returned Number = 280
//	STRING == '2.7.1', turnIntoVersionNumber = false	=	Returned Number = 271
//
INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionNumber)
{
	// Make Variables //
	INT32 i = startIFrom, j = startJFrom;
	INT32 finalNumber;

	char convertedString[256] = "";

	// Initialize the Main String, and Iterate Through Our Two Strings //
	while (STRING[j] != '\0')
	{
		if (STRING[j] == '.' || STRING[j] == '"' || STRING[j] == ' ')
		{
			j++;
			continue;
		}

		convertedString[i] = STRING[j];
		i++, j++;
	}

	// Add an Extra Digit or Two if Our String Has Less Than 2 Digits, Else Return Our Compressed Number, and We're Done! //
	finalNumber = ((turnIntoVersionNumber && strlen(convertedString) <= 2) ?
					(strlen(convertedString) == 2 ?
						(STAR_CombineNumbers(2, atoi(convertedString), 0)) :
						(STAR_CombineNumbers(3, atoi(convertedString), 0, 0))) :
					(atoi(convertedString)));
	return finalNumber;
}

//
// INT32 STAR_CombineNumbers(INT32 ARGS, INT32 FIRSTNUM, ...)
// Combines Numbers Together, Like You Would Do a String, But Doesn't Perform Math on the Numbers
//
INT32 STAR_CombineNumbers(INT32 ARGS, INT32 FIRSTNUM, ...)
{
	// Make Variables //
	va_list argptr;

	INT32 i;
	char numberString[256] = ""; sprintf(numberString, "%d", FIRSTNUM);

	// Initialize and Iterate Through the Variable List of Arguments, Combine our Number Strings Together, and Then End it //
	va_start(argptr, FIRSTNUM);
	for (i = 0; i < ARGS-1; i++)
		strlcat(numberString, va("%d", va_arg(argptr, int)), sizeof(numberString));
	va_end(argptr);

	// Convert the String Made Earlier Into a Number, Return The Number, and We're Done :) //
	return STAR_ConvertStringToCompressedNumber(numberString, 0, 0, false);
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

#if 0

texture_t FindTexture(void)
{
	// Easy Texture Finder
	if (textures[i]->hash == hash && !strncasecmp(textures[i]->name, name, 8))
}

#endif

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
			M_ClearMenus();
	}
}

// =============
// SCREEN BORDER
// =============

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

#ifdef HWRENDER

static const INT32 picmode2GR[] =
{
	GL_TEXFMT_P_8,                // PALETTE
	0,                            // INTENSITY          (unsupported yet)
	GL_TEXFMT_ALPHA_INTENSITY_88, // INTENSITY_ALPHA    (corona use this)
	0,                            // RGB24              (unsupported yet)
	GL_TEXFMT_RGBA,               // RGBA32             (opengl only)
};

#endif // HWRENDER

// ===============
// VERSION DRAWING
// ===============

void F_VersionDrawer(void)
{
	// An adapted thing from old menus - most games have version info on the title screen now...

	INT32 texty = vid.height - 10*vid.dup;
	INT32 trans = 5;

	if (gamestate == GS_TITLESCREEN)
	{
		trans = 10 - (finalecount - (3*TICRATE)/2)/3;
		if (trans >= 10)
			return;
		if (trans < 5)
			trans = 5;
	}

	trans = (trans<<V_ALPHASHIFT)|V_NOSCALESTART;

#define addtext(f, str) {\
	V_DrawThinString(vid.dup, texty, trans|f, str);\
	texty -= 10*vid.dup;\
}
	if (customversionstring[0] != '\0')
	{
		addtext(0, customversionstring);
		addtext(0, "Mod version:");
	}
	else
	{
// Development -- show revision / branch info
#if defined(TESTERS)
		addtext(V_SKYMAP, "Tester client");
		addtext(0, va("%s", compdate));
#elif defined(DEVELOP)
		addtext(0, va("%s %s", comprevision, compnote));
		addtext(0, D_GetFancyBranchName());

		if (compoptimized)
		{
			addtext(0, va("%s build", comptype));
		}
		else
		{
			addtext(V_ORANGEMAP, va("%s build (no optimizations)", comptype));
		}

#else // Regular build
		addtext(trans, va("%s", VERSIONSTRING));
#endif

		if (compuncommitted)
		{
			addtext(V_REDMAP|V_STRINGDANCE, "! UNCOMMITTED CHANGES !");
		}
	}
#undef addtext
}

// ==================
// EXMUSIC
// ==================

static mapheader_t *determined_map = NULL;
static const char *determined_vanilla_music = NULL;
static const char *determined_exm_music = NULL;

static char *determined_music = NULL;
static char *new_determined_music = NULL;


#define SET_EXMUSIC_MUSIC(music) \
	CONS_Printf("def %s is valid\n", music); \
	determined_exm_music = music; \
	new_determined_music = malloc(7); \
	new_determined_music = music; \

#define EXMUSIC_CHECK_EXISTANCE(slot) \
	TSoURDt3rd_S_MusicExists(slot, { SET_EXMUSIC_MUSIC(slot->name) return; });


#define SET_VANILLA_MUSIC(music) \
	CONS_Printf("music %s is valid - for var %s\n", music, #music); \
	determined_vanilla_music = music; \
	new_determined_music = malloc(7); \
	strlcpy(new_determined_music, music, 7); \

#define VANILLA_MUSIC_EXISTS(music) \
	SET_VANILLA_MUSIC(music) \
	break;

#define VANILLA_MUSIC_EXISTS_BY_NAME(music) \
	TSoURDt3rd_S_MusicExists(music, VANILLA_MUSIC_EXISTS(music))


#if 1
#define MUSIC_MATCHES \
	strnicmp(S_MusicName(), ((mapmusflags & MUSIC_RELOADRESET) ? determined_map->musname : mapmusname), 7)
#endif

#if 0
#define MUSIC_MATCHES \
	strnicmp(S_MusicName(), ((mapmusflags & MUSIC_RELOADRESET) ? determined_map->musname : determined_vanilla_music), 7)
#endif

#if 0
#define MUSIC_MATCHES false
#endif


static void EXMusic_DetermineMusic(void)
{
	tsourdt3rd_exmusic_findTrackResult_t exm_find_track_result;
	tsourdt3rd_world_scenarios_t scenario = tsourdt3rd_local.world.scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types = tsourdt3rd_local.world.scenario_types;

#define EXMUSIC_GET_TYPE(scenario_type, exm_track_set) { \
		memset(&exm_find_track_result, 0, sizeof(exm_find_track_result)); \
		if (scenario_type == -1 || (scenario_types & scenario_type)) \
		{ \
			if (TSoURDt3rd_EXMusic_FindTrack(cv_tsourdt3rd_audio_exmusic[exm_track_set].string, exm_track_set, &exm_find_track_result) == true) \
			{ \
				if (exm_find_track_result.series_pos >= tsourdt3rd_exmusic_starting_series_max) \
					EXMUSIC_CHECK_EXISTANCE(exm_find_track_result.lump) \
			} \
			memset(&exm_find_track_result, 0, sizeof(exm_find_track_result)); \
		} \
	}

	if (scenario & TSOURDT3RD_WORLD_SCENARIO_INTERMISSION)
	{
		if (scenario & TSOURDT3RD_WORLD_SCENARIO_BOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS, tsourdt3rd_exmusic_intermission_truefinalboss)
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS, tsourdt3rd_exmusic_intermission_finalboss)
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_intermission_boss)
		}
		EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_intermission)
		return;
	}

	if (determined_map && cv_tsourdt3rd_audio_bosses_postboss.value)
	{
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_POSTBOSS)
			TSoURDt3rd_S_MusicExists(determined_map->muspostbossname, { SET_EXMUSIC_MUSIC(determined_map->muspostbossname) return; });
	}

	if (scenario & TSOURDT3RD_WORLD_SCENARIO_BOSS)
	{
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_TRUEFINALBOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_truefinalboss_pinch)
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses_truefinalboss)
		}
		if (scenario_types & TSOURDT3RD_WORLD_SCENARIO_TYPES_FINALBOSS)
		{
			EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_finalboss_pinch)
			EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses_finalboss)
		}
		EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_RACE, tsourdt3rd_exmusic_bosses_race)
		EXMUSIC_GET_TYPE(TSOURDT3RD_WORLD_SCENARIO_TYPES_BOSSPINCH, tsourdt3rd_exmusic_bosses_pinch)
		EXMUSIC_GET_TYPE(-1, tsourdt3rd_exmusic_bosses)

		//TSoURDt3rd_S_MusicExists(determined_map->musname, { SET_EXMUSIC_MUSIC(determined_map->musname) return; });
	}

#undef EXMUSIC_GET_TYPE
}

#include "../../p_setup.h" // levelloading

//
// const char *TSoURDt3rd_EXMusic_DetermineLevelMusic(void)
// Sets level music. Should always return *something*, no matter what.
//
const char *TSoURDt3rd_EXMusic_DetermineLevelMusic(void)
{
	static char *last_exm_music = NULL;
	static boolean use_user_defined_music = false;

	if (new_determined_music) free(new_determined_music);
	new_determined_music = NULL;

#if 1
	static tsourdt3rd_world_scenarios_t last_scenario = 0;
	static tsourdt3rd_world_scenarios_types_t last_scenario_types = 0;
	tsourdt3rd_world_scenarios_t scenario;
	tsourdt3rd_world_scenarios_types_t scenario_types;

	TSoURDt3rd_WORLD_UpdateScenarios();
	scenario = tsourdt3rd_local.world.scenario;
	scenario_types = tsourdt3rd_local.world.scenario_types;

	if (last_scenario != scenario || last_scenario_types != scenario_types)
	{
		// New scneario, new music!
		use_user_defined_music = false;
	}

	last_scenario = scenario;
	last_scenario_types = scenario_types;
#endif

	//lastmaploaded
	//levelloading
	if (determined_map == NULL || determined_map != mapheaderinfo[gamemap-1] || levelloading)
	{
		// New map, reloaded map, whatever - Update map junk
		determined_map = mapheaderinfo[gamemap-1];
		determined_vanilla_music = (determined_map ? determined_map->musname : mapmusname);
		determined_exm_music = NULL;
		determined_music = NULL;

		last_exm_music = NULL;
		use_user_defined_music = false;
	}
#if 1
	else if (use_user_defined_music || strnicmp(determined_music, S_MusicName(), 7))
	{
		// User/Lua might've defined this music, so uh...
		CONS_Printf("using USER DEFINED music\n");
		use_user_defined_music = true;
		determined_music = NULL;
		determined_music = malloc(7);
		strncpy(determined_music, S_MusicName(), 7);
		return determined_music;
	}
#endif
#if 1
	else if (strnicmp(determined_vanilla_music, mapmusname, 7))
	{
		// Uh oh, map strings differ, might wanna grab the new one!
		CONS_Printf("updating determined_vanilla_music\n");
		determined_vanilla_music = mapmusname;
	}
#endif

	// Now, determine our music!
	if (TSoURDt3rd_AprilFools_ModeEnabled())
	{
		// April Fools Mode overrides literally everything.
		TSoURDt3rd_S_MusicExists("_hehe", { return "_hehe"; })
		return "";
	}
	else if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// No, don't override my music.
		return "";
	}
	else if (gamestate == GS_TITLESCREEN || titlemapinaction)
	{
		// Made it here? Play the map's default track, and we're done :)
		if (determined_map) TSoURDt3rd_S_MusicExists(determined_map->musname, { return determined_map->musname; })
		TSoURDt3rd_S_MusicExists(mapmusname, { return mapmusname; })
		TSoURDt3rd_S_MusicExists("_title", { return "_title"; })
		return "";
	}
	else if (determined_map == NULL)
	{
		// ...How?
		TSoURDt3rd_S_MusicExists(mapmusname, { return mapmusname; })
		return "";
	}

#if 0
	if (determined_exm_music == NULL || (determined_music && !strnicmp(determined_exm_music, determined_music, 7)))
	{
		// We want to make sure that the tracks are still the same.
		// If the track is different from the last exmusic track we just played,
		// then it was most likely map/user-modified, so we don't really want to change that.
		EXMusic_DetermineMusic();
	}
#else
	EXMusic_DetermineMusic();
#endif

#if 0
	if (RESETMUSIC || strnicmp(S_MusicName(),
		((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname), 7))
		return ((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname);
	else
		return mapheaderinfo[gamemap-1]->musname;

#if 0
	if (strnicmp(TSoURDt3rd_EXMusic_DetermineLevelMusic(), S_MusicName(), 7))
		return mapmusname;
#endif
	return ((!mapmusname[0] || !strnicmp(mapmusname, S_MusicName(), 7)) ? mapheaderinfo[gamemap-1]->musname : mapmusname);

#else

#if 0
	if (selected_def == NULL)
	{
		if (mapmusflags & MUSIC_RELOADRESET)
			return map->musname;
		return mapmusname;
	}
#else
	goto finished;

finished:
{

#if 1
	if (new_determined_music != NULL && (determined_music == NULL || (last_exm_music && !strnicmp(determined_music, last_exm_music, 7))))
	{
		// We found a different piece of music, so let's leave!
		CONS_Printf("playing selected music!\n");
		determined_music = new_determined_music;
		last_exm_music = determined_music;
		return new_determined_music;
	}
#else
	if (new_determined_music != NULL)
	{
		// We found a different piece of music, so let's leave!
		CONS_Printf("playing selected music!\n");
		determined_music = new_determined_music;
		return new_determined_music;
	}
#endif

#if 0
	if (strnicmp(determined_vanilla_music, mapmusname, 7))
	{
		// Uh oh, map strings differ, might wanna grab the new one!
		CONS_Printf("updating determined_vanilla_music\n");
		determined_vanilla_music = mapmusname;
	}
#endif

	switch (gamestate)
	{
		case GS_EVALUATION:
		case GS_GAMEEND:
			// We don't *HAVE* to play anything here, it's just cool if we *CAN*.
			VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
			/* FALLTHRU */

		case GS_INTERMISSION:
			//VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
			VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musintername);
			switch (intertype)
			{
				case int_coop:
					VANILLA_MUSIC_EXISTS_BY_NAME("_clear");
					break;
				case int_spec:
					VANILLA_MUSIC_EXISTS_BY_NAME(stagefailed ? "CHFAIL" : "CHPASS");
					break;
				default:
					break;
			}
			/* FALLTHRU */

		default:
			if (new_determined_music != NULL) break;
#if 0
#if 1
			if ((
				RESETMUSIC ||
				MUSIC_MATCHES
			//if (RESETMUSIC
				)
				//&& levelloading
			)
			{
				if (mapmusflags & MUSIC_RELOADRESET)
					VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname)
				else
					VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname)
			}
			else
#endif
			{
				if (strnicmp(mapmusname, determined_map->musname, 7)
					&& !levelloading
				)
				{
					// Tracks are different, so let's try this one first.
					// Could've been set by 'tunes' or anything.
					// In any case, it's most likely user-defined.
					VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
				}
				VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname);
			}
#else
			if (RESETMUSIC || MUSIC_MATCHES)
			//if (RESETMUSIC)
			{
				if (mapmusflags & MUSIC_RELOADRESET)
					VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname)
				else
				{
#if 0
					if (determined_music && strnicmp(determined_vanilla_music, determined_music, 7))
					{
						// If track is different from the last track we just played,
						// then we want this one.
						if (strnicmp(determined_vanilla_music, determined_exm_music, 7)) // NO EXMUSIC
							VANILLA_MUSIC_EXISTS_BY_NAME(determined_vanilla_music);
					}
#else
					VANILLA_MUSIC_EXISTS_BY_NAME(determined_vanilla_music);
#endif

#if 0
					// DEFAULT
					VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname)
					//VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname);
#else
					VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname);
					VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname)
#endif
				}
			}
			else
			{
#if 1
				// DEFAULT
				if (determined_music && strnicmp(determined_vanilla_music, determined_music, 7))
				{
					// If track is different from the last track we just played,
					// then we want this one.
					if (strnicmp(determined_vanilla_music, determined_exm_music, 7)) // NO EXMUSIC
						VANILLA_MUSIC_EXISTS_BY_NAME(determined_vanilla_music);
				}
#else
				VANILLA_MUSIC_EXISTS_BY_NAME(determined_vanilla_music);
#endif

#if 1
				// DEFAULT
				VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname);
				VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
#else
				VANILLA_MUSIC_EXISTS_BY_NAME(mapmusname);
				VANILLA_MUSIC_EXISTS_BY_NAME(determined_map->musname);
#endif
			}
#endif
			break;
	}

#if 0
	if (new_determined_music == NULL)
	{
		return NULL;
	}
	else
	{
#if 0
		if (strnicmp(determined_vanilla_music, new_determined_music, 7))
		{
			// If track is different from the last track we just played,
			// then we want this one.
			if (strnicmp(determined_vanilla_music, determined_exm_music, 7)) // NO EXMUSIC
			{
				strlcpy(new_determined_music, determined_vanilla_music, 7);
				return new_determined_music;
			}
		}
#endif
		determined_music = new_determined_music;
	}
#else
	// DEFAULT
	determined_music = new_determined_music;
#endif

	return (new_determined_music ? new_determined_music : "");
}
#endif
#endif
}

