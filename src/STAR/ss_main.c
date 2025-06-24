// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_main.c
/// \brief Contains all of TSoURDt3rd's main necessary info and structures

/// \todo 1. rename file, 2. only keep main bios stuff here
/// \todo alternatively: move and bomb

#include <time.h>

#include "ss_main.h"
#include "star_vars.h"
#include "smkg-defs.h"
#include "smkg-jukebox.h"
#include "core/smkg-s_exmusic.h"
#include "misc/smkg-m_misc.h"

#include "../f_finale.h"
#include "../i_time.h"
#include "../z_zone.h"
#include "../r_main.h"
#include "../v_video.h"
#include "../p_local.h"
#include "../m_argv.h"
#include "../g_game.h" // player_names
#include "../m_menu.h" // saveinfo_t

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

#ifdef HAVE_SDL
#include "smkg-i_sys.h" // TSoURDt3rd_I_Pads_InitControllers() //
#endif

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_t tsourdt3rd[MAXPLAYERS];
struct tsourdt3rd_local_s tsourdt3rd_local;

tsourdt3rd_timedEvent_t tsourdt3rd_currentEvent = TSOURDT3RD_EVENT_NONE;
struct tsourdt3rd_loadingscreen_s tsourdt3rd_loadingscreen;

static saveinfo_t* cursave = NULL;

INT32 STAR_ServerToExtend = 0;
INT32 DefaultMapTrack = 0;
boolean SpawnTheDispenser = false;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Init(void)
{
	memset(&tsourdt3rd_local, 0, sizeof(struct tsourdt3rd_local_s));
	STAR_CONS_Printf(0, "\n");

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "TSoURDt3rd_Init(): Initalizing TSoURDt3rd...\n");
	TSoURDt3rd_FOL_CreateDirectory("TSoURDt3rd");

	TSoURDt3rd_CheckTime(); // Check our computer's time!

#ifdef HAVE_SDL
	TSoURDt3rd_I_Pads_InitControllers(); // Initialize our cool controller system!
#endif

	TSoURDt3rd_Jukebox_Init(); // Initialize Jukebox data
	TSoURDt3rd_S_EXMusic_Init(); // Initialize EXMusic data
	TSoURDt3rd_InitializePlayer(consoleplayer); // Initialize the build's player structures!

	STAR_CONS_Printf(0, "\n");
}

//
// void STAR_CONS_Printf(tsourdt3rd_messagetype_t starMessageType, const char *fmt, ...)
// A function specifically dedicated towards printing out certain TSoURDt3rd and STAR stuff in the console!
//
// starMessageType Parameters:
//		default						- Doesn't do anything extra, just prints to the console.
//		STAR_CONS_TSOURDT3RD		- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_TSOURDT3RD_NOTICE	- CONS_Printf("\x83" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_TSOURDT3RD_ALERT	- CONS_Printf("\x85" "%s" "\x80 ", M_GetText("TSoURDt3rd:")) + ...
//		STAR_CONS_APRILFOOLS		- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd April Fools:")) + ...
//		STAR_CONS_EASTER			- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Easter:")) + ...
//		STAR_CONS_JUKEBOX			- CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Jukebox:")) + ...
//
void STAR_CONS_Printf(tsourdt3rd_messagetype_t starMessageType, const char *fmt, ...)
{
	va_list argptr;
	static char *txt = NULL;

	if (txt == NULL)
		txt = malloc(8192);

	va_start(argptr, fmt);
	vsprintf(txt, fmt, argptr);
	va_end(argptr);

	switch (starMessageType)
	{
		case STAR_CONS_TSOURDT3RD:
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd:"));
			break;
		case STAR_CONS_TSOURDT3RD_NOTICE:
			CONS_Printf("\x83" "%s" "\x80 ", M_GetText("TSoURDt3rd:"));
			break;
		case STAR_CONS_TSOURDT3RD_ALERT:
			CONS_Printf("\x85" "%s" "\x80 ", M_GetText("TSoURDt3rd:"));
			break;
		case STAR_CONS_TSOURDT3RD_DEBUG:
#ifdef TSOURDT3RD_DEBUGGING
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Debugging:"));
			break;
#else
			return;
#endif
		case STAR_CONS_APRILFOOLS:
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd April Fools:"));
			break;
		case STAR_CONS_EASTER:
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Easter:"));
			break;
		case STAR_CONS_JUKEBOX:
			CONS_Printf("\x82" "%s" "\x80 ", M_GetText("TSoURDt3rd Jukebox:"));
			break;
		default:
			break;
	}

	// Now, just like STJr, I am lazy and I feel like just letting CONS_Printf take care of things.
	// That should be fine with you. (...Right?)
	CONS_Printf("%s", txt);
}

const char *TSoURDt3rd_CON_DrawStartupScreen(void)
{
	switch (cv_tsourdt3rd_game_startup_image.value)
	{
		case 1:
			return "CONSBACK";
		case 2:
			return "BABYSONIC";
		default:
			return "STARTUP";
	}
}

const char *TSoURDt3rd_ReturnUsername(void)
{
#ifdef HAVE_DISCORDSUPPORT
	if (discordInfo.ConnectionStatus & DISC_CONNECTED)
		return DISC_ReturnUsername();
#endif
	if (Playing())
		return player_names[consoleplayer];
	return cv_playername.string;
}

// ======
// EVENTS
// ======

//
// void TSoURDt3rd_CheckTime(void)
// Handles checking the current time on the user's computer.
// Helps with starting events and the sort.
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
		tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_EASTER;

	// April Fools
	else if (((tptr && tptr->tm_mon == 3 && (tptr->tm_mday >= 1 && tptr->tm_mday <= 3))
		|| (M_CheckParm("-aprilfools")))
			&& !M_CheckParm("-noaprilfools"))
		tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_APRILFOOLS;

	// Christmas Eve to New Years
	else if (((tptr && (tptr->tm_mon == 12 && tptr->tm_mday >= 24))
		|| (M_CheckParm("-xmas")))
			&& !M_CheckParm("-noxmas"))
		tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_CHRISTMAS;

	if (!tsourdt3rd_currentEvent)
		return;

	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): Easter Mode Enabled!\n");
		CV_RegisterVar(&cv_tsourdt3rd_easter_egghunt_allowed);
		CV_RegisterVar(&cv_tsourdt3rd_easter_egghunt_bonuses);
	}
	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_APRILFOOLS)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): April Fools Mode Enabled!\n");
		CV_RegisterVar(&cv_tsourdt3rd_aprilfools_ultimatemode);
		TSoURDt3rd_AprilFools_StoreDefaultMenuStrings();
	}
	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_CHRISTMAS)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "TSoURDt3rd_CheckTime(): Christmas Mode Enabled!\n");
	}

	modifiedgame = false;
}

// ======
// LEVELS
// ======

//
// mobj_t *TSoURDt3rd_BossInMap(void)
// Scans the player's map for any bosses, and returns the first one found, if any.
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
		if (!(mobj->flags & MF_BOSS) && (mobj->type != MT_METALSONIC_RACE))
			continue;

		return (mobj_t *)th;
	}
	return NULL;
}


// ======
// SCENES
// ======

//#define NEW_GAME_END_SCENE

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

void TSoURDt3rd_GameEnd(INT32 *timetonext)
{
#ifndef NEW_GAME_END_SCENE
	(void)(*timetonext);
	(void)cursave;
	return;
#else
	static boolean init = false;
	static INT32 headerScroll = BASEVIDWIDTH;

	// draw a background so we don't have weird mirroring errors
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);

	if (netgame)
		return;

	if (!init)
	{
		cursave = Z_Realloc(cursave, sizeof(saveinfo_t), PU_STATIC, NULL);
		if (!cursave && cursaveslot)
			I_Error("Insufficient memory to prepare final rank");

		(*timetonext) = 10*TICRATE;
		headerScroll = BASEVIDWIDTH;
		init = true;
	}

	if (--*timetonext <= 0)
	{
		if (cursave)
		{
			Z_Free(cursave);
			cursave = NULL;
		}
		init = false;
		headerScroll = BASEVIDWIDTH;
		return;
	}

	if (*timetonext <= 3*TICRATE/2)
		headerScroll--;

	V_DrawCenteredString(((BASEVIDWIDTH/2)-headerScroll), 65, V_SNAPTOBOTTOM|V_MENUCOLORMAP, "Great Job!");
	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, cv_playername.string);

	V_DrawCreditString((((BASEVIDWIDTH/2)-headerScroll))<<(FRACBITS-1), (BASEVIDHEIGHT-125)<<(FRACBITS-1), 0, cv_playername.string);

	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, va("%d", cursave[cursaveslot].lives));
#endif
}
