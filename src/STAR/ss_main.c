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
#include "smkg-jukebox.h"
#include "smkg-misc.h"
#include "menus/smkg_m_func.h"
#include "curl/smkg-curl.h"

#include "drrr/k_menu.h" // menumessage //

#include "../f_finale.h"
#include "../i_time.h"
#include "../z_zone.h"
#include "../r_main.h"
#include "../v_video.h"
#include "../p_local.h"
#include "../m_argv.h"
#include "../i_video.h" // rendermode
#include "../deh_soc.h" // TSoURDt3rd_LoadedGamedataAddon

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

static saveinfo_t* cursave = NULL;

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

void TSoURDt3rd_Init(void)
{
	CONS_Printf("TSoURDt3rd_Init(): Initalizing TSoURDt3rd...\n");
	TSoURDt3rd_FOL_CreateDirectory("TSoURDt3rd"PATHSEP"Test"PATHSEP"Run"PATHSEP"Test"PATHSEP"Real"PATHSEP"American"PATHSEP"HOOHOOWAAEFKDHIJDSJSISJDJNFFNYEAH!");

	TSoURDt3rd_CheckTime(); // Check our computer's time!
	TSoURDt3rd_InitializePlayer(consoleplayer); // Initialize the build's player structures

	// Add our custom menu data :p
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_MainMenuDef, &TSoURDt3rd_OP_MainMenuDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_EventsDef, &TSoURDt3rd_OP_EventsDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_GameDef, &TSoURDt3rd_OP_GameDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_ControlsDef, &TSoURDt3rd_OP_ControlsDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_VideoDef, &TSoURDt3rd_OP_VideoDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_AudioDef, &TSoURDt3rd_OP_AudioDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_PlayerDef, &TSoURDt3rd_OP_PlayerDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_SavefileDef, &TSoURDt3rd_OP_SavefileDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_ServerDef, &TSoURDt3rd_OP_ServerDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_JukeboxDef, &TSoURDt3rd_OP_JukeboxDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_ExtrasDef, &TSoURDt3rd_OP_ExtrasDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_Extras_SnakeDef, &TSoURDt3rd_OP_Extras_SnakeDef);

#ifdef HAVE_DISCORDSUPPORT
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_OP_DiscordOptionsDef, &OP_DiscordOptionsDef);
	TSoURDt3rd_M_AddNewMenu(&TSoURDt3rd_TM_MISC_DiscordRequestsDef, &MISC_DiscordRequestsDef);
#endif	
}

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
			CONS_Printf("\x82STAR_CONS_Printf:\x80 You must specify a specific message type!\n");
			return;
	}

	// Now, just like STJr, I am lazy and I feel like just letting CONS_Printf take care of things.
	// That should be fine with you. (...Right?)
	CONS_Printf("%s", txt);
}

const char *TSoURDt3rd_CON_DrawStartupScreen(void)
{
	switch (cv_startupscreen.value)
	{
		case 1:
			return "CONSBACK";
		case 2:
			return "BABYSONIC";
		default:
			return "STARTUP";
	}
}

void TSoURDt3rd_D_Display(void)
{
	static boolean sent_event_message = false;

#ifdef HAVE_CURL
	// Check for any updates to TSoURDt3rd.
	TSoURDt3rd_Curl_FindUpdateRoutine();
#endif

	switch (gamestate)
	{
		case GS_ENDING:
		case GS_CREDITS:
		case GS_EVALUATION:
			if (TSoURDt3rd_AprilFools_ModeEnabled())
			{
				// Close the game if we're in April Fools' Ultimate Mode but just beat the game.
				I_Error("Definitely caused by a SIGSEGV - seventh sentinel (core dumped)");
			}
			/* FALLTHRU */

		default:
			break;	
	}

	// Check for any events.
	if (!menumessage.active && !sent_event_message && (eastermode || aprilfoolsmode || xmasmode))
	{
		DRRR_M_StartMessage(
			"A TSoURDt3rd Event is Occuring",
			"We're having a seasonal event! Have fun!",
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		sent_event_message = true;
	}
}

const char *TSoURDt3rd_ReturnUsername(void)
{
#ifdef HAVE_DISCORDSUPPORT
	if (discordInfo.ConnectionStatus == DRPC_CONNECTED)
		return DRPC_ReturnUsername();
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

			CV_RegisterVar(&cv_tsourdt3rd_aprilfools_ultimatemode);

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

		default:
			break;
	}
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
	const char *determinedMusic = TSoURDt3rd_DetermineLevelMusic();

	TSoURDt3rd->loadingScreens.loadCount = TSoURDt3rd->loadingScreens.loadPercentage = 0; // reset loading status
	TSoURDt3rd->loadingScreens.bspCount = 0; // reset bsp count
	TSoURDt3rd->loadingScreens.loadComplete = false; // reset loading finale

#ifdef HAVE_SDL
	STAR_SetWindowTitle();
#endif

	if (savemoddata)
		TSoURDt3rd_LoadedGamedataAddon = true;
	if (!netgame)
		STAR_SetSavefileProperties();

	if (!(reloadinggamestate || titlemapinaction))
	{
		// Display the loading screen...
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

		// Change the music :)
		if (strnicmp(S_MusicName(),
			((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : determinedMusic), 7))
		{
			strncpy(mapmusname, determinedMusic, 7);

			mapmusname[6] = 0;
			mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
			mapmusposition = mapheaderinfo[gamemap-1]->muspos;
		}

		// Fade music, by the way.
		if (RESETMUSIC || strnicmp(S_MusicName(),
			(mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname, 7))
		{
			S_FadeMusic(0, FixedMul(
				FixedDiv((F_GetWipeLength(wipedefs[wipe_level_toblack])-2)*NEWTICRATERATIO, NEWTICRATE), MUSICRATE));
		}

		// Set the music.
		S_Start();
	}
}


// ======
// SCENES
// ======

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
	static boolean init = false;
	static INT32 headerScroll = BASEVIDWIDTH;

	// draw a background so we don't have weird mirroring errors
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);

	if (!init)
	{
		cursave = Z_Realloc(cursave, sizeof(saveinfo_t), PU_STATIC, NULL);
		if (!cursave && cursaveslot)
			I_Error("Insufficient memory to prepare final rank");

		*timetonext = 10*TICRATE;
		headerScroll = BASEVIDWIDTH;
		init = true;
	}
	else if (--*timetonext <= 0)
	{
		if (cursave)
		{
			Z_Free(cursave);
			cursave = NULL;
		}

		init = false;
		headerScroll = BASEVIDWIDTH;
	}

	if (headerScroll)
		headerScroll--;

	V_DrawCenteredString(((BASEVIDWIDTH/2)-headerScroll), 65, V_SNAPTOBOTTOM|V_MENUCOLORMAP, "Great Job!");
	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, cv_playername.string);

	V_DrawCreditString((((BASEVIDWIDTH/2)-headerScroll))<<(FRACBITS-1), (BASEVIDHEIGHT-125)<<(FRACBITS-1), 0, cv_playername.string);

	V_DrawCenteredString(BASEVIDWIDTH/2, 65, V_MENUCOLORMAP, va("%d", cursave[cursaveslot].lives));
}
