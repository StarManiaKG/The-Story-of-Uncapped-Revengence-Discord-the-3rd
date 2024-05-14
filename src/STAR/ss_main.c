// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_main.c
/// \brief Contains all of TSoURDt3rd's main necessary info and structures

#include <time.h>

#include "ss_main.h"
#include "m_menu.h"
#include "../f_finale.h"
#include "../i_time.h"
#include "../z_zone.h"
#include "../m_menu.h"
#include "../r_main.h"
#include "../v_video.h"
#include "../p_local.h"
#include "../m_argv.h"
#include "../i_video.h" // rendermode
#include "../deh_soc.h" // TSoURDt3rd_LoadedGamedataAddon

// ------------------------ //
//        Variables
// ------------------------ //

static saveinfo_t* cursave = NULL;
static INT32 endcount;

#if 0
typedef struct
{
	char levelname[32];
	UINT8 skinnum;
	UINT8 botskin;
	UINT8 numemeralds;
	UINT8 numgameovers;
	INT32 lives;
	INT32 continuescore;
	INT32 gamemap;
} saveinfo_t;
#endif

// ======
// EVENTS
// ======

tsourdt3rdevent_t TSoURDt3rd_CurrentEvent = 0;

boolean aprilfoolsmode = false;
boolean eastermode = false;
boolean xmasmode = false, xmasoverride = false;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void STAR_CONS_Printf(star_messagetype_t starMessageType, const char *fmt, ...)
// A function specifically dedicated towards printing out certain TSoURDt3rd and STAR stuff in the console!
//
// starMessageType Parameters:
//		0/NULL						- Doesn't Add Anything Extra, Therefore Returns the Function Entirely.
//
//		STAR_CONS_TSOURDT3RD		- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_TSOURDT3RD_NOTICE	- CONS_Printf("\x83" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_TSOURDT3RD_ALERT	- CONS_Printf("\x85" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_APRILFOOLS		- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd April Fools:")) + ...
//		STAR_CONS_EASTER			- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Easter:")) + ...
//		STAR_CONS_JUKEBOX			- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Jukebox:")) + ...
//
void STAR_CONS_Printf(star_messagetype_t starMessageType, const char *fmt, ...)
{
	va_list argptr;
	static char *txt = NULL;

	if (txt == NULL)
		txt = malloc(8192);

	va_start(argptr, fmt);
	vsprintf(txt, fmt, argptr);
	va_end(argptr);

	// Now, just like STJr, I am lazy and I feel like just letting CONS_Printf take care of things.
	// That should be fine with you. (...Right?)
	switch (starMessageType)
	{
		case STAR_CONS_TSOURDT3RD: CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd:")); break;
		case STAR_CONS_TSOURDT3RD_NOTICE: CONS_Printf("\x83" "%s" "\x80 ", M_GetText("TSoURDt3rd:")); break;
		case STAR_CONS_TSOURDT3RD_ALERT: CONS_Printf("\x85" "%s" "\x80 ", M_GetText("TSoURDt3rd:")); break;

		case STAR_CONS_TSOURDT3RD_DEBUG:
		{
#ifdef TSOURDT3RD_DEBUGGING
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Debugging:")); break;
#else
			return;
#endif
		}

		case STAR_CONS_APRILFOOLS: CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd April Fools:")); break;
		case STAR_CONS_EASTER: CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Easter:")); break;

		case STAR_CONS_JUKEBOX: CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Jukebox:")); break;

		default:
		{
			CONS_Printf("\x82STAR_CONS_Printf:\x80 You must specify a specific message type!\n");
			free(txt);

			return;
		}
	}

	CONS_Printf("%s", txt);
}

const char *TSoURDt3rd_CON_DrawStartupScreen(void)
{
	switch (cv_startupscreen.value)
	{
		case 1:		return "CONSBACK";
		case 2:		return "BABYSONIC";
		default:	return "STARTUP";
	}
}

void TSoURDt3rd_D_Display(void)
{
	// 'Error out' if in April Fools' ultimate mode but have beaten the game //
	switch (gamestate)
	{
		case GS_ENDING:
		case GS_CREDITS:
		case GS_EVALUATION:
		{
			if (TSoURDt3rd_InAprilFoolsMode())
				I_Error("SIGSEGV - seventh sentinel (core dumped)");
			break;
		}

		default: break;	
	}
}

// ======
// EVENTS
// ======

//
// void TSoURDt3rd_CheckTime(void)
// Handles checking the current time on the user's computer.
// Helps with starting events and the sort.
//
// Ported from Kart!
//
void TSoURDt3rd_CheckTime(void)
{
	time_t t1;
	struct tm* tptr = NULL;

	t1 = time(NULL);
	if (t1 != (time_t)-1)
		tptr = localtime(&t1);

	// Set the events //
	// Easter (Changes Every Year Though, so just have it for all of March)
	if (((tptr && tptr->tm_mon == 3)
		|| (M_CheckParm("-easter")))
			&& !M_CheckParm("-noeaster"))
		TSoURDt3rd_CurrentEvent = TSOURDT3RD_EASTER;

	// April Fools
	else if (((tptr && tptr->tm_mon == 3 && (tptr->tm_mday >= 1 && tptr->tm_mday <= 3))
		|| (M_CheckParm("-aprilfools")))
			&& !M_CheckParm("-noaprilfools"))
		TSoURDt3rd_CurrentEvent = TSOURDT3RD_APRILFOOLS;

	// Christmas Eve to New Years
	else if (((tptr && (tptr->tm_mon == 11 && tptr->tm_mday >= 24))
		|| (M_CheckParm("-xmas")))
			&& !M_CheckParm("-noxmas"))
		TSoURDt3rd_CurrentEvent = TSOURDT3RD_CHRISTMAS;

	switch (TSoURDt3rd_CurrentEvent)
	{
		case TSOURDT3RD_EASTER:
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): Easter Mode Enabled!\n");

			CV_RegisterVar(&cv_easter_allowegghunt);
			CV_RegisterVar(&cv_easter_egghuntbonuses);

			eastermode = true;
			modifiedgame = false;

			TSoURDt3rd_LoadExtras = true;

			break;
		}

		case TSOURDT3RD_APRILFOOLS:
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): April Fools Mode Enabled!\n");

			CV_RegisterVar(&cv_aprilfools_ultimatemode);

			aprilfoolsmode = true;
			modifiedgame = false;
			STAR_StoreDefaultMenuStrings();

			TSoURDt3rd_LoadExtras = true;
			break;
		}

		case TSOURDT3RD_CHRISTMAS:
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): Christmas Mode Enabled!\n");

			xmasmode = true;
			xmasoverride = true;
			modifiedgame = false;

			TSoURDt3rd_LoadExtras = true;
			break;
		}

		default: break;
	}
}

// ======
// LEVELS
// ======

//
// mobj_t *TSoURDt3rd_BossInMap(void)
// Scans the player's map for any bosses.
//
mobj_t *TSoURDt3rd_BossInMap(void)
{
	if (!(gamestate == GS_LEVEL || gamestate == GS_INTERMISSION))
		return NULL;

	for (thinker_t *th = thlist[THINK_MOBJ].next; th != &thlist[THINK_MOBJ]; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_RemoveThinkerDelayed)
			continue;

		mobj_t *mobj = (mobj_t *)th;
		if (mobj == NULL || P_MobjWasRemoved(mobj))
			continue;
		if (!(mobj->flags & MF_BOSS))
			continue;

		return (mobj_t *)th;
	}
	return NULL;
}

//
// void TSoURDt3rd_LoadLevel(boolean reloadinggamestate)
// Loads various bits of level data, exclusively for TSoURDt3rd.
//
void TSoURDt3rd_LoadLevel(boolean reloadinggamestate)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];
	const char *cmptrack = ((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname);

	TSoURDt3rd->loadingScreens.loadCount = TSoURDt3rd->loadingScreens.loadPercentage = 0; // reset loading status
	TSoURDt3rd->loadingScreens.bspCount = 0; // reset bsp count

	if (savemoddata)
		TSoURDt3rd_LoadedGamedataAddon = true;
	if (!netgame)
		STAR_SetSavefileProperties();
#ifdef HAVE_SDL
	STAR_SetWindowTitle();
#endif

	if (!(reloadinggamestate || titlemapinaction))
	{
		const char *determinedMusic = TSoURDt3rd_DetermineLevelMusic();
		boolean musicChanged = strnicmp(S_MusicName(), cmptrack, 7);

		if (rendermode != render_none)
		{
			if (cv_loadingscreen.value && TSoURDt3rd->loadingScreens.loadCount-- <= 0 && !TSoURDt3rd->loadingScreens.loadComplete)
			{
				while (TSoURDt3rd->loadingScreens.bspCount != 1 && (((TSoURDt3rd->loadingScreens.loadPercentage)<<1) < 100) && rendermode == render_soft)
				{
					TSoURDt3rd->loadingScreens.loadCount = numsubsectors/50;
					STAR_LoadingScreen();
				}

				TSoURDt3rd->loadingScreens.loadCount = TSoURDt3rd->loadingScreens.loadPercentage = 0; // reset the loading status
				TSoURDt3rd->loadingScreens.screenToUse = 0; // reset the loading screen to use

				TSoURDt3rd->loadingScreens.loadComplete = true; // loading... load complete.
			}
		}

		/* PS: As mentioned in P_LoadLevel, while oddly named, S_Start() only handles music.
			Starting it again here for our stuff should be fine, just don't do it during the titlemap :p */
		if (musicChanged)
		{
			strncpy(mapmusname, determinedMusic, 7);

			mapmusname[6] = 0;
			mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
			mapmusposition = mapheaderinfo[gamemap-1]->muspos;

			// Fade music, by the way.
			if (RESETMUSIC || strnicmp(S_MusicName(), cmptrack, 7))
			{
				S_FadeMusic(0, FixedMul(
					FixedDiv((F_GetWipeLength(wipedefs[wipe_level_toblack])-2)*NEWTICRATERATIO, NEWTICRATE), MUSICRATE));
			}
			S_Start();
		}
		else if (!strnicmp(S_MusicName(), cmptrack, 7))
			S_Start();
	}
}


// ======
// SCENES
// ======

void TSoURDt3rd_GameEnd(void)
{
	static INT32 headerScroll = BASEVIDWIDTH;

	// draw a background so we don't have weird mirroring errors
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);

	if (!endcount)
	{
		cursave = Z_Realloc(cursave, sizeof(saveinfo_t), PU_STATIC, NULL);
		if (!cursave && cursaveslot)
			I_Error("Insufficient memory to prepare final rank");

		endcount++;
		goto resetVariables;
	}
	else if (++endcount >= 10*TICRATE)
	{
		if (cursave)
		{
			Z_Free(cursave);
			cursave = NULL;
		}

		goto resetVariables;
	}
	else
	{
		if (!(endcount % 2*TICRATE/8))
		{
			if (headerScroll >= TICRATE)
				headerScroll--;
		}

		V_DrawCenteredString(((BASEVIDWIDTH/2)-headerScroll), 65, V_SNAPTOBOTTOM|V_MENUCOLORMAP, "Great Job!");
		V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, cv_playername.string);

		V_DrawCreditString((((BASEVIDWIDTH/2)-headerScroll))<<(FRACBITS-1), (BASEVIDHEIGHT-125)<<(FRACBITS-1), 0, cv_playername.string);

		V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, va("%d", cursave[cursaveslot].lives));
	}

	resetVariables:
	{
		headerScroll = BASEVIDWIDTH;
		return;
	}
}

// ======
// SCREEN
// ======

void TSoURDt3rd_SCR_DisplayTpsRate(void)
{
	INT32 tpscntcolor = 0;
	const INT32 h = vid.height-(8*vid.dupy);

	tic_t i;
	tic_t ontic = I_GetTime();
	tic_t totaltics = 0;

	static tic_t lasttic;
	static boolean ticsgraph[TICRATE];

	if (gamestate == GS_NULL)
		return;

	for (i = lasttic + 1; i < TICRATE+lasttic && i < ontic; ++i)
		ticsgraph[i % TICRATE] = false;
	ticsgraph[ontic % TICRATE] = true;

	for (i = 0; i < TICRATE; ++i)
		if (ticsgraph[i])
			++totaltics;

	if (totaltics <= TICRATE/2) tpscntcolor = V_REDMAP;
	else if (totaltics <= TICRATE-8) tpscntcolor = V_YELLOWMAP;
	else tpscntcolor = V_TPSCOLORMAP;

	if (cv_tpsrate.value == 2) // compact counter
		V_DrawRightAlignedString(vid.width, h-(8*vid.dupy),
			tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, va("%02d", totaltics));
	else if (cv_tpsrate.value == 1) // full counter
	{
		const char *drawntpsStr;
		INT32 tpswidth;
		
		drawntpsStr = va("%02d/ %02u", totaltics, TICRATE);
		tpswidth = V_StringWidth(drawntpsStr, V_NOSCALESTART);

		V_DrawString(vid.width - ((7 * 8 * vid.dupx) + V_StringWidth("TPS: ", V_NOSCALESTART)), h-(8*vid.dupy),
			V_MENUCOLORMAP|V_NOSCALESTART|V_USERHUDTRANS, "TPS:");
		V_DrawString(vid.width - tpswidth, h-(8*vid.dupy),
			tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, drawntpsStr);
	}

	lasttic = ontic;
}

INT32 TSoURDt3rd_SCR_SetPingHeight(void)
{
	INT32 pingy;

	if ((cv_ticrate.value && cv_tpsrate.value) || cv_tpsrate.value)
		pingy = 171;
	else if (!cv_ticrate.value)
		pingy = 189;
	else
		pingy = 180;

	return pingy;
}
