// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
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
#include "smkg-cvars.h"
#include "smkg-defs.h"
#include "core/smkg-s_jukebox.h"
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

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_timedEvent_t tsourdt3rd_currentEvent = TSOURDT3RD_EVENT_NONE;
struct tsourdt3rd_loadingscreen_s tsourdt3rd_loadingscreen;

static saveinfo_t* cursave = NULL;

INT32 STAR_ServerToExtend = 0;
boolean SpawnTheDispenser = false;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void STAR_CONS_Printf(INT32 message_type, const char *fmt, ...)
// A function specifically dedicated towards printing out certain TSoURDt3rd and STAR stuff in the console!
//
// (May stretch out to regular CONS_Printf in the future...)
//
void STAR_CONS_Printf(INT32 message_type, const char *fmt, ...)
{
	va_list argptr;
	const char *coloring = NULL;
	static char *txt = NULL;
	static char header[8192];
	static char bparams[8192];
	boolean header_set = false;

	memset(header, 0, sizeof(header));
	memset(bparams, 0, sizeof(bparams));
	if (txt == NULL)
		txt = malloc(8192);

	va_start(argptr, fmt);
	vsnprintf(txt, 8192, fmt, argptr);
	va_end(argptr);

#ifndef _TSOURDT3RD_DEBUGGING
	if ((message_type & STAR_CONS_DEBUG) == STAR_CONS_DEBUG)
		return;
#endif

	// Set the header...
	if (message_type)
	{
		if (message_type & STAR_CONS_TSOURDT3RD)
		{
			strcpy(header, "TSoURDt3rd");
			header_set = true;
		}
		else if (message_type & STAR_CONS_DISCORD)
		{
#ifdef HAVE_DISCORDSUPPORT
			strcpy(header, discord_integration_type);
			header_set = true;
#else
			return;
#endif
		}
	}

	// Extend the header...
	if (header_set)
	{
		if (message_type & STAR_CONS_APRILFOOLS)
			strlcat(header, " April Fools", 8192);
		else if (message_type & STAR_CONS_EASTER)
			strlcat(header, " Easter", 8192);
		else if (message_type & STAR_CONS_JUKEBOX)
			strlcat(header, " Jukebox", 8192);
		if (message_type & STAR_CONS_DEBUG)
			strlcat(header, " Debugging", 8192);
		strlcat(header, ":", 8192);
	}

	// Set the coloring...
	if (message_type & STAR_CONS_NOTICE)
		coloring = "\x83";
	else if (message_type & STAR_CONS_ERROR)
		coloring = "\x85";
	else if (message_type & STAR_CONS_WARNING)
		coloring = "\x82";
	else if (message_type & STAR_CONS_DEBUG)
		coloring = "\x8f";
	else
		coloring = "\x80";

	// Appropriately reset text coloring...
	if ((message_type & STAR_CONS_COLORWHOLELINE) != STAR_CONS_COLORWHOLELINE)
		strlcat(header, "\x80", 8192);
	else
		strlcat(txt, "\x80", 8192);

	// Add the correct spacing...
	if (header_set)
		strlcat(header, " ", 8192);

	// Make sure we check for string parameters first, just so we don't do anthing crazy...
	for (INT32 i = 0; (txt[i] != '\0' && txt[i+1] != '\0'); i++)
	{
		switch (txt[i])
		{
			case '\n':
				snprintf(bparams, 8192, "\n"); txt++;
				continue;
			default:
				break;
		}
		break;
	}

	// Now, just like STJr, I am lazy and I feel like just letting CONS_Printf take care of everything else.
	// That should be fine with you. (...Right?)
	CONS_Printf("%s%s%s%s", bparams, coloring, header, txt);
}

const char *TSoURDt3rd_CON_DrawStartupScreen(void)
{
	switch (cv_tsourdt3rd_game_startup_image.value)
	{
		case 1: return "CONSBACK";
		case 2: return "BABYSONIC";
		default: return "STARTUP";
	}
}

const char *TSoURDt3rd_ReturnUsername(void)
{
#define RETURN_USERNAME(name) if (name != NULL) return name;
#ifdef HAVE_DISCORDSUPPORT
	if (discordInfo.connected) { RETURN_USERNAME(DISC_ReturnUsername()) }
#endif
	RETURN_USERNAME(player_names[consoleplayer])
	RETURN_USERNAME(cv_playername.string)
	return NULL;
#undef RETURN_USERNAME
}

// ======
// EVENTS
// ======

//
// void TSoURDt3rd_CheckTime(void)
// Handles checking the current time on the user's computer. Helps with starting events and the sort.
//
void TSoURDt3rd_CheckTime(void)
{
	time_t t1 = time(NULL);
	struct tm* tptr = NULL;

	if (t1 != (time_t)-1)
	{
		tptr = localtime(&t1);
		if (tptr == NULL) return;
	}

	if (tptr->tm_mon == 3 || M_CheckParm("-easter"))
	{
		// Easter (Changes Every Year Though, so just have it for all of March)
		if (!M_CheckParm("-noeaster")) tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_EASTER;
	}
	else if ((tptr->tm_mon == 3 && (tptr->tm_mday >= 1 && tptr->tm_mday <= 3)) || M_CheckParm("-aprilfools"))
	{
		// April Fools
		if (!M_CheckParm("-noaprilfools")) tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_APRILFOOLS;
	}
	else if ((tptr->tm_mon == 12 && tptr->tm_mday >= 24) || M_CheckParm("-xmas"))
	{
		// Christmas Eve to New Years
		if (!M_CheckParm("-noxmas")) tsourdt3rd_currentEvent |= TSOURDT3RD_EVENT_CHRISTMAS;
	}
	if (!tsourdt3rd_currentEvent)
		return;

	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER)
	{
		STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd_CheckTime(): Easter Mode Enabled!\n");
		CV_RegisterVar(&cv_tsourdt3rd_easter_egghunt_allowed);
		CV_RegisterVar(&cv_tsourdt3rd_easter_egghunt_bonuses);
	}
	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_APRILFOOLS)
	{
		STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd_CheckTime(): April Fools Mode Enabled!\n");
		CV_RegisterVar(&cv_tsourdt3rd_aprilfools_ultimatemode);
		TSoURDt3rd_AprilFools_StoreDefaultMenuStrings();
	}
	if (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_CHRISTMAS)
	{
		STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd_CheckTime(): Christmas Mode Enabled!\n");
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
	mobj_t *mobj = NULL;
	thinker_t *think = &thlist[THINK_MOBJ];

	if (think == NULL || think->next == NULL)
		return NULL;

	if (Playing() && gamestate == GS_LEVEL) // Obviously a world must exist!
	{
		for (think = think->next; think != &thlist[THINK_MOBJ]; think = think->next)
		{
			if (think == NULL || think->removing || think->function == (actionf_p1)P_RemoveThinkerDelayed)
				continue;

			mobj = (mobj_t *)think;
			if (P_MobjWasRemoved(mobj))
				continue;
			if (!(mobj->flags & MF_BOSS) && (mobj->type != MT_METALSONIC_RACE))
				continue;

			return mobj;
		}
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
	static INT32 headerScroll = BASEVIDWIDTH;

	// draw a background so we don't have weird mirroring errors
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);

	if (netgame)
		return;

	if (cursave && timetonext < 0)
	{
		cursave = Z_Realloc(cursave, sizeof(saveinfo_t), PU_STATIC, NULL);
		if (!cursave && cursaveslot)
			I_Error("Insufficient memory to prepare final rank");

		(*timetonext) = 10*TICRATE;
		headerScroll = BASEVIDWIDTH;
	}
	else
	{
		cursave = Z_Malloc(sizeof(saveinfo_t), PU_STATIC, NULL);
	}

	if (--*timetonext <= 0)
	{
		if (cursave)
		{
			Z_Free(cursave);
			cursave = NULL;
		}
		headerScroll = BASEVIDWIDTH;
		return;
	}

	if (*timetonext <= 3*TICRATE/2)
	{
		headerScroll--;
	}

	V_DrawCenteredString(((BASEVIDWIDTH/2)-headerScroll), 65, V_SNAPTOBOTTOM|V_MENUCOLORMAP, "Great Job!");
	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, cv_playername.string);

	V_DrawCreditString((((BASEVIDWIDTH/2)-headerScroll))<<(FRACBITS-1), (BASEVIDHEIGHT-125)<<(FRACBITS-1), 0, cv_playername.string);

	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, va("%d", cursave[cursaveslot].lives));
#endif
}
