// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Original Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Original Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-game.c
/// \brief TSoURDt3rd's gameplay menu options

#include "../../m_menu.h"
#include "../../curl/smkg-curl.h"

#include "../../../r_main.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_GameTicker(void);
static void G_CheckForTSoURDt3rdUpdates(INT32 choice);

static menuitem_t TSoURDt3rd_OP_GameMenu[] =
{
	{IT_STRING | IT_CALL, NULL, "Check for Updates...",
		G_CheckForTSoURDt3rdUpdates, 0},
	{IT_STRING | IT_CVAR, NULL,	"Check For Updates On Startup",
		&cv_tsourdt3rd_main_checkforupdatesonstartup, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "SDL", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Window Title Type",
			&cv_windowtitletype, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Custom Window Title",
			&cv_customwindowtitle, 0},
		{IT_STRING | IT_CVAR, NULL, "Memes on Window Title",
			&cv_memesonwindowtitle, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Scenes", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Startup Screen",
			&cv_startupscreen, 36},
		{IT_STRING | IT_CVAR, NULL, "Sonic Team Jr Intro",
			&cv_stjrintro, 41},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Loading Screen",
			&cv_loadingscreen, 51},
		{IT_STRING | IT_CVAR, NULL, "Loading Screen Image",
			&cv_loadingscreenimage, 56},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Pause Graphic Style",
			&cv_pausegraphicstyle, 136},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Quit Screen",
			&cv_quitscreen, 66},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Levels", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Automap Outside Devmode",
			&cv_automapoutsidedevmode, 141},
		{IT_STRING | IT_CVAR, NULL, "Allow Typical Time Over",
			&cv_allowtypicaltimeover, 131},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Objects", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Shadow Type",
			&cv_shadow, 111},
		{IT_STRING | IT_CVAR, NULL, "All Objects Have Shadows",
			&cv_allobjectshaveshadows, 116},
		{IT_STRING | IT_CVAR, NULL, "Shadow Position",
			&cv_shadowposition, 121},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL,	"Sonic CD Mode",
			&cv_tsourdt3rd_game_soniccd, 151},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Menus", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Is it Called 'Single Player'?",
			&cv_tsourdt3rd_game_isitcalledsingleplayer,	76},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_GameMenu[] =
{
	{NULL, "Check for any updates to TSoURDt3rd.", 0, 0},
	{NULL, "Should TSoURDt3rd check for updates when starting?", 0, 0},

	{NULL, NULL, 128, 128},

	{NULL, NULL, 0, 0},
	{NULL, "What should TSoURDt3rd's window title show?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Define your perfect window title.", 0, 0},
	{NULL, "Can we show memes on your window title?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "The image to display when starting TSoURDt3rd.", 0, 0},
	{NULL, "Which STJr intro should we display?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Show loading screens.", 0, 0},
	{NULL, "The image to display on the loading screen.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "The graphic to display while the game is paused.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "The image to display upon quitting TSoURDt3rd.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Should you be allowed to view the automap outside of devmode?", 0, 0},
	{NULL, "Are time-overs allowed?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "The type of shadow to display below objects.", 0, 0},
	{NULL, "Should all objects have a shadow?", 0, 0},
	{NULL, "How should each shadow be positioned?", 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Should destroyed enemies free flowers instead of flickies?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "How should '1-Player' be referred to as?", 0, 0},
};

menu_t TSoURDt3rd_OP_GameDef =
{
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_GameMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_GameMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	12, 56,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_GameDef = {
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_TM_OP_GameMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_GameTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};

enum
{
	op_sdl_stuff = 3,
    op_sdl_windowtitletype,
	op_sdl_space,
	op_sdl_customwindowtitle,
	op_sdl_windowtitlememes,

	op_scenes_loadingscreenimage = 14,

	op_levels_timeover = 22,

	op_objects_rotatingshadows = 27,

	op_general_isitcalledsingleplayer = 32
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_GameTicker(void)
{
	TSoURDt3rd_M_OptionsTick();

#ifndef HAVE_SDL
	for (INT32 i = op_sdl_stuff; i <= op_sdl_windowtitlememes; i++)
		TSoURDt3rd_OP_GameMenu[i].status = IT_DISABLED;
#else
	TSoURDt3rd_OP_GameMenu[op_sdl_customwindowtitle].status =
		(cv_windowtitletype.value >= 2 ? IT_CVAR|IT_STRING|IT_CV_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_sdl_windowtitlememes].status =
		(cv_windowtitletype.value == 1 ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
#endif

	TSoURDt3rd_OP_GameMenu[op_scenes_loadingscreenimage].status =
		(cv_loadingscreen.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_levels_timeover].status =
		(!netgame ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_objects_rotatingshadows].status =
		(cv_shadow.value == 2 ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_general_isitcalledsingleplayer].status = 
		(!cv_tsourdt3rd_aprilfools_ultimatemode.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}

static void G_CheckForTSoURDt3rdUpdates(INT32 choice)
{
	(void)choice;

	TSoURDt3rdPlayers[consoleplayer].checkedVersion = false;
	TSoURDt3rd_Curl_FindUpdateRoutine();
}
